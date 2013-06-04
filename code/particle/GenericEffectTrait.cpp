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

		any incomingAny;
		if (config.reflectDirection && source.getArgument(INCOMING_DIRECTION, incomingAny))
		{
			vec3d incoming = any_cast<vec3d>(incomingAny);

			dir = getReflectionVector(&incoming, &dir);
		}

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
