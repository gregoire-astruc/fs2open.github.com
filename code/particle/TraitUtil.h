
#ifndef _TRAIT_UTIL_H
#define _TRAIT_UTIL_H

#include "globalincs/pstypes.h"
#include "particle/ParticleSource.h"

namespace TraitUtil
{
	template<class ValueType>
	struct ValueRange
	{
		ValueType upper;
		ValueType lower;

		ValueRange() {}

		ValueRange(const ValueType& value) : upper(value), lower(value) {}

		ValueRange(const ValueType& value1, const ValueType& value2) : upper(value1), lower(value2) {}
	};

	template<class ValueType>
	ValueType randomRange(const ValueType lower, const ValueType upper);

	template<class ValueType>
	inline ValueType randomRange(const ValueRange<ValueType>& range)
	{
		return randomRange(range.lower, range.upper);
	}
	
	template<class ValueType>
	ValueRange<ValueType> parseValueRange(const ValueType lowerBound, const ValueType upperBound);

	int parseAnimation(int& nframes);

	bool hasEffectPosition(const ParticleSource& source);
	vec3d getEffectPosition(const ParticleSource& source, bool* success = NULL);
	
	bool hasEffectDirection(const ParticleSource& source);
	vec3d getEffectDirection(const ParticleSource& source, bool* success = NULL, bool addVelocity = true);

	vec3d getReflectionVector(vec3d* incoming, vec3d* normal);
};

#endif
