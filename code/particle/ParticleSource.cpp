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

bool ParticleSource::getArgument(SourceArgumentType type, boost::any& value) const
{
	SCP_map<SourceArgumentType, boost::any>::const_iterator valueIter = argumentMapping.find(type);

	if (valueIter == argumentMapping.end())
	{
		return false;
	}
	else
	{
		value = valueIter->second;

		return true;
	}
}

#ifndef NDEBUG

template<class T>
bool isType(const any& any)
{
	try
	{
		any_cast<T>(any);
		return true;
	}
	catch (const bad_any_cast &)
	{
		return false;
	}
}

#endif

void ParticleSource::setArgument(SourceArgumentType type, any& argument)
{
#ifndef NDEBUG
	// Check the argument type to make sure the correct types are passed and also validate the arguments
	const float EPSILON = 0.001f;

	switch (type)
	{
	case POSITION:
		Assertion(isType<vec3d>(argument), "Illegal argument type, expected vector!");
		break;
	case DIRECTION:
		{
			Assertion(isType<vec3d>(argument), "Illegal argument type, expected vector!");

			vec3d dir = any_cast<vec3d>(argument);
			Assertion(vm_vec_mag_squared(&dir) > 1.0f - EPSILON && vm_vec_mag_squared(&dir) < 1.0f + EPSILON, "Direction vector argument is not normalized!");
		}
		break;
	case OBJECT:
		Assertion(isType<object_h>(argument), "Illegal argument type, expected object pointer!");
		break;
	case PARTICLE:
		Assertion(isType<weak_ptr<particle>>(argument), "Illegal argument type, expected particle pointer!");
		break;
	case INCOMING_DIRECTION:
		Assertion(isType<weak_ptr<particle>>(argument), "Illegal argument type, expected particle pointer!");
		{
			Assertion(isType<vec3d>(argument), "Illegal argument type, expected vector!");

			vec3d dir = any_cast<vec3d>(argument);
			Assertion(vm_vec_mag_squared(&dir) > 1.0f - EPSILON && vm_vec_mag_squared(&dir) < 1.0f + EPSILON, "Incoming direction vector argument is not normalized!");
		}
		break;
	}
#endif

	argumentMapping[type] = argument;
}

void ParticleSource::update(float frametime)
{
	Assertion(!parentEffect.expired(), "The parent effect pointer is expired!");

	parentEffect.lock()->update(*this, frametime);
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

const boost::any& ParticleSource::getEffectProperty(int id) const
{
	Assertion(id >= 0 && id < (int) effectProperties.size(), "Illegal effect property index %d! Get a coder!", id);

	return effectProperties[id];
}

bool ParticleSource::isActive() const
{
	Assertion(!parentEffect.expired(), "The parent effect pointer is expired!");

	return parentEffect.lock()->isActive(*this);
}

bool ParticleSource::operator==(const ParticleSource& other) const
{
	return signature == other.signature;
}
