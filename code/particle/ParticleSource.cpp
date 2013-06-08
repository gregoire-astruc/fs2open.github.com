#include "particle/ParticleSource.h"
#include "object/object.h"
#include "particle/particle.h"
#include "model/model.h"

using namespace boost;

ParticleSource::ParticleSource(boost::weak_ptr<ParticleEffect> parentEffect) : parentEffect(parentEffect)
{
}


ParticleSource::~ParticleSource()
{
}

const boost::any& ParticleSource::getEffectProperty(int id) const
{
	Assertion(id >= 0 && id < (int) effectProperties.size(), "Illegal effect property index %d! Get a coder!", id);

	return effectProperties[id];
}

void ParticleSource::update(float frametime)
{
	Assertion(!parentEffect.expired(), "The parent effect pointer is expired!");

	parentEffect.lock()->update(*this, frametime);
}

bool ParticleSource::isActive() const
{
	Assertion(!parentEffect.expired(), "The parent effect pointer is expired!");

	return parentEffect.lock()->isActive(*this);
}

void ParticleSource::addEffectProperty(int id, const boost::any& value)
{
	Assertion(id >= 0, "Effect property id is invalid, get a coder!");

	// Use the vector as an int-map
	if (id < (int) effectProperties.size())
	{
		effectProperties[id] = value;
	}
	else
	{
		while (id >= (int) effectProperties.size())
		{
			effectProperties.push_back(boost::any());
		}

		// The vector is now large enough to hold the element
		effectProperties[id] = value;
	}
}
