
#ifndef _PARTICLE_SOURCE_H
#define _PARTICLE_SOURCE_H

#include "globalincs/pstypes.h"
#include "particle/ParticleEffect.h"

#include <boost/any.hpp>
#include <boost/smart_ptr.hpp>

enum SourceArgumentType
{
	POSITION,
	DIRECTION,
	INCOMING_DIRECTION,
	OBJECT,
	PARTICLE
};

class ParticleEffect;

class ParticleSource
{
private:
	boost::weak_ptr<ParticleEffect> parentEffect;
	
	SCP_vector<boost::any> effectProperties;
	SCP_vector<boost::any> argumentMapping;
public:
	ParticleSource(boost::weak_ptr<ParticleEffect> parent);
	~ParticleSource();

	template <SourceArgumentType TYPE>
	bool getArgument(boost::any& value) const;
	
	template <SourceArgumentType TYPE>
	void setArgument(const boost::any& argument);

	boost::weak_ptr<ParticleEffect> getParticleEffect() const { return parentEffect; }

	void addEffectProperty(int id, const boost::any& value);
	const boost::any& getEffectProperty(int id) const;

	inline void update(float frametime);

	inline bool isActive() const;
};

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

template<SourceArgumentType TYPE>
bool ParticleSource::getArgument(boost::any& value) const
{
	if (argumentMapping.size() <= TYPE)
	{
		return false;
	}

	value = argumentMapping[TYPE];

	return true;
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

template<SourceArgumentType TYPE>
void ParticleSource::setArgument(const any& argument)
{
#ifndef NDEBUG
	// Check the argument type to make sure the correct types are passed and also validate the arguments
	const float EPSILON = 0.001f;

	switch (TYPE)
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

	while (argumentMapping.size() <= (size_t) TYPE)
	{
		// Reserve place

		argumentMapping.push_back(boost::any());
	}

	argumentMapping[TYPE] = argument;
}

#endif // _PARTICLE_SOURCE_H
