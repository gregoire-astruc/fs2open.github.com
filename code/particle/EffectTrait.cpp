#include "particle/EffectTrait.h"
#include "particle/particle.h"
#include "parse/parselo.h"
#include "io/timer.h"
#include "model/model.h"

using namespace boost;

struct GenericEffectState
{
	int beginTimestamp;
	int endTimestamp;
};

GenericEffectTrait::GenericEffectTrait(const SCP_string& name) : EffectTrait(name)
{
}

void GenericEffectTrait::associate(ParticleEffect& effect)
{
	genericTraitID = effect.requestTraitID();
}

boost::weak_ptr<particle> GenericEffectTrait::createParticle(vec3d* position, vec3d* vel, int animation, float lifetime, float radius, object* objp)
{
	weak_ptr<particle> particle = particle_create(position, vel, lifetime, radius, PARTICLE_BITMAP, animation, -1.0f, objp);

	if (config.trailEffectID > 0)
	{
		ParticleSource* source = ParticleSystem::getInstance()->createParticleEffect(config.trailEffectID);

		any argument = any(particle);
		source->setArgument(PARTICLE, argument);
	}

	return particle;
}

float GenericEffectTrait::randomRange(float upper, float lower)
{
	Assertion(lower <= upper, "Lower bound has to be actually lower! Got %f and %f as lower and upper bounds.", lower, upper);

	float random = ((float) rand()) / (float) RAND_MAX;
	return upper + random * (lower - upper);
}

void GenericEffectTrait::update(const ParticleSource& source, float frametime)
{
	if (spawningParticles(source))
	{
		vec3d position = getEffectPosition(source);

		createParticle(&position, &vmd_zero_vector, config.effectAnimation, 5.0f, 8.0f);
	}
}

bool GenericEffectTrait::spawningParticles(const ParticleSource& source)
{
	const any stateAny = source.getEffectProperty(this->genericTraitID);

	const GenericEffectState state = any_cast<GenericEffectState>(stateAny);

	return timestamp_elapsed(state.beginTimestamp) && !timestamp_elapsed(state.endTimestamp);
}

vec3d GenericEffectTrait::getEffectPosition(const ParticleSource& source, bool* success) const
{
	any argumentAny;

	if (success != NULL)
	{
		*success = true;
	}

	if (source.getArgument(POSITION, argumentAny))
	{
		return any_cast<vec3d>(argumentAny);
	}

	if (source.getArgument(COLLISION_INFO, argumentAny))
	{
		return any_cast<mc_info>(argumentAny).hit_point_world;
	}

	if (source.getArgument(PARTICLE, argumentAny))
	{
		weak_ptr<particle> part = any_cast<weak_ptr<particle>>(argumentAny);

		if (!part.expired())
		{
			return part.lock()->pos;
		}
	}

	if (source.getArgument(OBJECT, argumentAny))
	{
		object_h objh = any_cast<object_h>(argumentAny);

		if (objh.IsValid())
		{
			return objh.objp->pos;
		}
	}

	if (success != NULL)
	{
		*success = false;
	}

	return vmd_zero_vector;
}

vec3d GenericEffectTrait::getEffectDirection(const ParticleSource& source, bool* success) const
{
	any argumentAny;

	if (success != NULL)
	{
		*success = true;
	}

	if (source.getArgument(DIRECTION, argumentAny))
	{
		return any_cast<vec3d>(argumentAny);
	}

	if (source.getArgument(COLLISION_INFO, argumentAny))
	{
		vec3d dest;
		mc_info info = any_cast<mc_info>(argumentAny);
		vec3d normal = info.hit_normal;

		vm_vec_unrotate(&dest, &normal, info.orient);

		return dest;
	}

	if (source.getArgument(PARTICLE, argumentAny))
	{
		weak_ptr<particle> part = any_cast<weak_ptr<particle>>(argumentAny);

		if (!part.expired())
		{
			return part.lock()->velocity;
		}
	}

	if (source.getArgument(OBJECT, argumentAny))
	{
		object_h objh = any_cast<object_h>(argumentAny);

		if (objh.IsValid())
		{
			return objh.objp->phys_info.vel;
		}
	}

	if (success != NULL)
	{
		*success = false;
	}

	return vmd_zero_vector;
}

void GenericEffectTrait::initializeTrait(ParticleSource& source)
{
	GenericEffectState state;
	state.beginTimestamp = timestamp(config.beginDelay);
	state.endTimestamp = timestamp(config.beginDelay + config.effectDuration);

	any stateAny = any(state);
	source.addEffectProperty(genericTraitID, stateAny);
}

bool GenericEffectTrait::isActive(const ParticleSource& source) const
{
	// Check if we have a valid effect argument
	bool b;
	this->getEffectPosition(source, &b);

	if (!b)
	{
		return false;
	}
	// Direction is considered to be an optional argument

	const any stateAny = source.getEffectProperty(this->genericTraitID);

	const GenericEffectState state = any_cast<GenericEffectState>(stateAny);

	return timestamp_elapsed(state.endTimestamp);
}

void GenericEffectTrait::doParse()
{
	if (optional_string("+Effect Animation:"))
	{
		SCP_string fileName;
		stuff_string(fileName, F_NAME);

		config.effectAnimation = bm_load(fileName);

		config.effectAnimation = bm_load(fileName);
		if(config.effectAnimation < 0) {
			config.effectAnimation = bm_load_animation(const_cast<char*>(fileName.c_str()), NULL, NULL, NULL, true);
		}
	}

	if (optional_string("+Effect Delay:"))
	{
		float time;
		stuff_float(&time);

		if (time <= 0.0f)
		{
			Warning(LOCATION, "Got effect delay of zero or less seconds (%f)!", time);
			time = 0.0f;
		}

		config.beginDelay = fl2i(time * 1000.0f);
	}

	if (optional_string("+Effect Duration:"))
	{
		float time;
		stuff_float(&time);

		if (time <= 0.0f)
		{
			Warning(LOCATION, "Got effect duration of zero or less seconds (%f)!", time);
			time = 0.0f;
		}

		config.effectDuration = fl2i(time * 1000.0f);
	}

	if (optional_string("+Trail Effect:"))
	{
		config.trailEffectID = parse_particle_effect();
	}
}
