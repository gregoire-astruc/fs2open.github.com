
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
	SCP_map<SourceArgumentType, boost::any> argumentMapping;
public:
	ParticleSource(boost::weak_ptr<ParticleEffect> parent);
	~ParticleSource();

	bool getArgument(SourceArgumentType type, boost::any& value) const;
	void setArgument(SourceArgumentType type, boost::any& argument);

	boost::weak_ptr<ParticleEffect> getParticleEffect() const { return parentEffect; }

	void addEffectProperty(int id, const boost::any& value);
	const boost::any& getEffectProperty(int id) const;

	void update(float frametime);

	bool isActive() const;
};

#endif // _PARTICLE_SOURCE_H
