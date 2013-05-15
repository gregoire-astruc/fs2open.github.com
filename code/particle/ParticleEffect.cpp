#include "particle/ParticleEffect.h"

using namespace boost;

ParticleEffect::ParticleEffect(weak_ptr<ParticleSystem> parent, const SCP_string& effectName)
	: name(effectName), parentSystem(parent), nextTraitID(0)
{
}


ParticleEffect::~ParticleEffect()
{
}

int ParticleEffect::requestTraitID()
{
	return nextTraitID++;
}

void ParticleEffect::addTrait(boost::shared_ptr<EffectTrait> trait)
{
	this->effectTraits.push_back(trait);

	trait->associate(*this);
}

boost::shared_ptr<EffectTrait> ParticleEffect::getTrait(const SCP_string& name) const
{
	for (SCP_vector<shared_ptr<EffectTrait>>::const_iterator iter = effectTraits.begin();
		iter != effectTraits.end(); ++iter)
	{
		if (!(*iter)->getName().compare(name))
		{
			return *iter;
		}
	}

	return shared_ptr<EffectTrait>();
}

const SCP_string& ParticleEffect::getName() const
{
	return name;
}

bool ParticleEffect::isActive(const ParticleSource& source) const
{
	for (SCP_vector<shared_ptr<EffectTrait>>::const_iterator iter = effectTraits.begin();
		iter != effectTraits.end(); ++iter)
	{
		if ((*iter)->isActive(source))
		{
			return true;
		}
	}

	return false;
}

void ParticleEffect::update(const ParticleSource& source, float frametime)
{
	for (SCP_vector<shared_ptr<EffectTrait>>::iterator iter = effectTraits.begin();
		iter != effectTraits.end(); ++iter)
	{
		(*iter)->update(source, frametime);
	}
}
