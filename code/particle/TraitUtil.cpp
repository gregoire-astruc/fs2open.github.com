
#include "particle/TraitUtil.h"
#include "parse/parselo.h"
#include "math/vecmat.h"
#include "object/object.h"

#include <boost/any.hpp>

using namespace boost;

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
ValueRange<float> TraitUtil::parseValueRange<float>(const float lowerBound, const float upperBound)
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
ValueRange<int> TraitUtil::parseValueRange<int>(const int lowerBound, const int upperBound)
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


vec3d TraitUtil::getEffectPosition(const ParticleSource& source, bool* success)
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
		else
		{
			if (success != NULL)
			{
				*success = false;
			}

			// return null vector in error case.
			return vmd_zero_vector;
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
		else
		{
			if (success != NULL)
			{
				*success = false;
			}

			// return null vector in error case.
			return vmd_zero_vector;
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

vec3d TraitUtil::getEffectDirection(const ParticleSource& source, bool* success, bool addVelocity)
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
		else
		{
			if (success != NULL)
			{
				*success = false;
			}

			// return null vector in error case.
			return vmd_zero_vector;
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
		else
		{
			if (success != NULL)
			{
				*success = false;
			}

			// return null vector in error case.
			return vmd_zero_vector;
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

vec3d TraitUtil::getReflectionVector(vec3d* incoming, vec3d* normal)
{
	vec3d dest;
	float dot = vm_vec_dotprod(incoming, normal);
	vm_vec_scale_add(&dest, incoming, normal, -2.0f * dot);

	return dest;
}

