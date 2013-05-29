#include "particle/GenericEffectTrait.h"
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

void GenericEffectTrait::update(const ParticleSource& source, float frametime)
{
	if (spawningParticles(source))
	{
		vec3d position = getEffectPosition(source);

		vec3d dir = getEffectDirection(source, NULL, config.addObjectVelocity);

		vm_vec_scale(&dir, TraitUtil::randomRange(config.particleVelocity));

		weak_ptr<particle> part = createParticle(&position, &dir, config.effectAnimation, 1.0f, TraitUtil::randomRange(config.particleSize));

		if (!part.expired())
		{
			if (config.particleLifetime.lower >= 0.0f)
			{
				shared_ptr<particle> particle = part.lock();

				particle->max_life = TraitUtil::randomRange(config.particleLifetime);
			}
		}
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

	// If the source has a particle use that first
	if (source.getArgument(PARTICLE, argumentAny))
	{
		weak_ptr<particle> part = any_cast<weak_ptr<particle>>(argumentAny);

		if (!part.expired())
		{
			vec3d pos = part.lock()->pos;

			// Optionally add the POSITION argument
			if (source.getArgument(POSITION, argumentAny))
			{
				vec3d position = any_cast<vec3d>(argumentAny);
				vm_vec_add2(&pos, &position);
			}

			return pos;
		}
	}

	// If it has an object use its position
	if (source.getArgument(OBJECT, argumentAny))
	{
		object_h objh = any_cast<object_h>(argumentAny);

		if (objh.IsValid())
		{
			object* objp = objh.objp;
			vec3d pos = objp->pos;
			
			// Also optionally use the position with the objects rotation
			if (source.getArgument(POSITION, argumentAny))
			{
				vec3d position = any_cast<vec3d>(argumentAny);
				vec3d dest;

				vm_vec_unrotate(&dest, &position, &objp->orient);

				vm_vec_add2(&pos, &dest);
			}

			return pos;
		}
	}

	// The last case is just to return the position
	if (source.getArgument(POSITION, argumentAny))
	{
		return any_cast<vec3d>(argumentAny);
	}

	if (success != NULL)
	{
		*success = false;
	}

	// return null vector in error case.
	return vmd_zero_vector;
}

vec3d GenericEffectTrait::getEffectDirection(const ParticleSource& source, bool* success, bool addVelocity) const
{
	// This works similary to GenericEffectTrait::getEffectPosition

	any argumentAny;

	if (success != NULL)
	{
		*success = true;
	}

	// We only need the velocity of the particle and only if addVelocity is true
	if (addVelocity && source.getArgument(PARTICLE, argumentAny))
	{
		weak_ptr<particle> part = any_cast<weak_ptr<particle>>(argumentAny);

		if (!part.expired())
		{
			vec3d dir = part.lock()->velocity;

			// Optionally add the POSITION argument
			if (source.getArgument(DIRECTION, argumentAny))
			{
				vec3d direction = any_cast<vec3d>(argumentAny);
				vm_vec_add2(&dir, &direction);
			}

			// Normalize the direction
			vm_vec_normalize_safe(&dir);

			return dir;
		}
	}

	if (source.getArgument(OBJECT, argumentAny))
	{
		object_h objh = any_cast<object_h>(argumentAny);

		if (objh.IsValid())
		{
			object* objp = objh.objp;
			vec3d dir;

			if (addVelocity)
			{
				dir = objp->phys_info.vel;
			
				// Also optionally use the position with the objects rotation
				if (source.getArgument(DIRECTION, argumentAny))
				{
					vec3d direction = any_cast<vec3d>(argumentAny);
					vec3d dest;

					vm_vec_unrotate(&dest, &direction, &objp->orient);

					vm_vec_add2(&dir, &dest);
				}

				vm_vec_normalize_safe(&dir);

				return dir;
			}
			else if (source.getArgument(DIRECTION, argumentAny))
			{
				// Also optionally use the position with the objects rotation
				vec3d direction = any_cast<vec3d>(argumentAny);

				vm_vec_unrotate(&dir, &direction, &objp->orient);

				return dir;
			}
		}
	}

	if (source.getArgument(DIRECTION, argumentAny))
	{
		return any_cast<vec3d>(argumentAny);
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
	state.beginTimestamp = timestamp(TraitUtil::randomRange(config.beginDelay));
	state.endTimestamp = state.beginTimestamp + timestamp(TraitUtil::randomRange(config.effectDuration));

	any stateAny = any(state);
	source.addEffectProperty(genericTraitID, stateAny);
}

bool GenericEffectTrait::isActive(const ParticleSource& source) const
{
	bool b;
	this->getEffectPosition(source, &b);

	if (!b)
	{
		return false;
	}

	const any stateAny = source.getEffectProperty(this->genericTraitID);

	const GenericEffectState state = any_cast<GenericEffectState>(stateAny);

	return timestamp_elapsed(state.endTimestamp);
}

void GenericEffectTrait::doParse()
{
	required_string("+Effect Animation:");
	{
		SCP_string fileName;
		stuff_string(fileName, F_NAME);

		config.effectAnimation = bm_load(fileName);
		if(config.effectAnimation < 0) {
			config.effectAnimation = bm_load_animation(const_cast<char*>(fileName.c_str()), NULL, NULL, NULL, true);
		}
	}

	if (optional_string("+Effect Delay:"))
	{
		ValueRange<float> times = TraitUtil::parseValueRange(0.0f, std::numeric_limits<float>::max());

		config.beginDelay = ValueRange<int>(fl2i(times.lower * 1000.0f), fl2i(times.upper * 1000.0f));
	}

	required_string("+Effect Duration:");
	{
		ValueRange<float> times = TraitUtil::parseValueRange(0.0f, std::numeric_limits<float>::max());

		config.effectDuration = ValueRange<int>(fl2i(times.lower * 1000.0f), fl2i(times.upper * 1000.0f));
	}

	if (optional_string("+Particle Lifetime:"))
	{
		config.particleLifetime = TraitUtil::parseValueRange(0.0f, std::numeric_limits<float>::max());
	}
	
	required_string("+Particle Radius:");
	{
		config.particleSize = TraitUtil::parseValueRange(0.0f, std::numeric_limits<float>::max());
	}

	if (optional_string("+Particle Number:"))
	{
		config.particleNumber = TraitUtil::parseValueRange(1, std::numeric_limits<int>::max());
	}

	if (optional_string("+Particle Velocity:"))
	{
		config.particleVelocity = TraitUtil::parseValueRange(0.0f, std::numeric_limits<float>::max());
	}

	if (optional_string("+Add Object Velocity:"))
	{
		stuff_boolean(&config.addObjectVelocity);
	}

	if (optional_string("+Trail Effect:"))
	{
		config.trailEffectID = parse_particle_effect();
	}
}

template<>
float TraitUtil::randomRange<float>(const float upper, const float lower)
{
	Assertion(lower <= upper, "Lower bound has to be actually lower! Got %f and %f as lower and upper bounds.", lower, upper);
	
	float random = ((float) rand()) / (float) RAND_MAX;
	return upper + random * (lower - upper);
}

template<>
int TraitUtil::randomRange<int>(const int upper, const int lower)
{
	Assertion(lower <= upper, "Lower bound has to be actually lower! Got %d and %d as lower and upper bounds.", lower, upper);
	
	return rand() % (upper - lower) + lower;
}

template<>
ValueRange<float> TraitUtil::parseValueRange(const float lowerBound, const float upperBound)
{
	float value1;
	float value2;

	stuff_float(&value1);

	if (value1 < lowerBound)
	{
		Warning(LOCATION, "Value %f is lower than lower bound %f!", value1, lowerBound);
	}

	if (value1 > upperBound)
	{
		Warning(LOCATION, "Value %f is bigger than upper bound %f!", value1, upperBound);
	}

	CLAMP(value1, lowerBound, upperBound);

	value2 = value1;

	if (stuff_float_optional(&value2) != 0)
	{
		if (value2 < lowerBound)
		{
			Warning(LOCATION, "Value %f is lower than lower bound %f!", value2, lowerBound);
		}

		if (value2 > upperBound)
		{
			Warning(LOCATION, "Value %f is bigger than upper bound %f!", value2, upperBound);
		}

		CLAMP(value2, lowerBound, upperBound);

		if (value1 > value2)
		{
			mprintf(("Warning: Second value %f is bigger than first value %f!\n", value2, value1));

			float temp = value1;

			value1 = value2;
			value2 = temp;
		}
	}

	return ValueRange<float>(value1, value2);
}

template<>
ValueRange<int> TraitUtil::parseValueRange(const int lowerBound, const int upperBound)
{
	int value1;
	int value2;

	stuff_int(&value1);

	if (value1 < lowerBound)
	{
		Warning(LOCATION, "Value %d is lower than lower bound %f!", value1, lowerBound);
	}

	if (value1 > upperBound)
	{
		Warning(LOCATION, "Value %d is bigger than upper bound %f!", value1, upperBound);
	}

	CLAMP(value1, lowerBound, upperBound);

	value2 = value1;

	if (stuff_int_optional(&value2) != 0)
	{
		if (value2 < lowerBound)
		{
			Warning(LOCATION, "Value %d is lower than lower bound %f!", value2, lowerBound);
		}

		if (value2 > upperBound)
		{
			Warning(LOCATION, "Value %d is bigger than upper bound %f!", value2, upperBound);
		}

		CLAMP(value2, lowerBound, upperBound);

		if (value1 > value2)
		{
			mprintf(("Warning: Second value %d is bigger than first value %d!\n", value2, value1));

			int temp = value1;

			value1 = value2;
			value2 = temp;
		}
	}

	return ValueRange<int>(value1, value2);
}
