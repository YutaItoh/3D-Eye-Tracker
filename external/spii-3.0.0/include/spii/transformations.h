// Petter Strandmark 2012.
#ifndef SPII_TRANSFORMATIONS_H
#define SPII_TRANSFORMATIONS_H

#include <cmath>
#include <stdexcept>

#include <spii/spii.h>

namespace spii {

// Transformation to force a variable to
// be greater than zero.
//
//	x_i = exp(t_i)
//  t_i = log(x_i)
//
class GreaterThanZero
{
public:
	GreaterThanZero(int dimension)
	{
		this->dimension = dimension;
	}

	template<typename R>
	void t_to_x(R* x, const R* t) const
	{
		using std::exp;

		for (size_t i = 0; i < dimension; ++i) {
			x[i] = t[i] * t[i];
		}
	}

	template<typename R>
	void x_to_t(R* t, const R* x) const
	{
		using std::sqrt;

		for (size_t i = 0; i < dimension; ++i) {
			t[i] = sqrt(x[i]);
		}
	}

	int x_dimension() const
	{
		return dimension;
	}

	int t_dimension() const
	{
		return dimension;
	}

private:
	int dimension;
};

// Transformation to force a variable to
// be in an interval [a, b].
//
//	x = a + (b-a)*(1/pi * arctan(t) + 0.5)
//  t = tan(((x - a) / (b - a) - 0.5) * pi)
//
class IntervalConstraint
{
public:

	IntervalConstraint(double a, double b)
	{
		spii_assert(b > a, "Interval: non-positive size");
		this->a = a;
		this->b = b;
	}

	template<typename R>
	void t_to_x(R* x, const R* t) const
	{
		using std::atan;
		x[0] = a + (b - a) * (0.318309886183791 * atan(t[0]) + 0.5);
	}

	template<typename R>
	void x_to_t(R* t, const R* x) const
	{
		using std::tan;
		t[0] = tan(((x[0] - a) / (b - a) - 0.5) * 3.141592653589793);
	}

	int x_dimension() const
	{
		return 1;
	}

	int t_dimension() const
	{
		return 1;
	}

private:
	double a, b;
};

// Transformation to force a variable to
// be in a box [a1, b1] * ... * [an, bn]
//
//	x_i = a_i + (b_i - a_i)*(1/pi * arctan(t_i) + 0.5)
//  t_i = tan(((x_i - a_i) / (b_i - a_i) - 0.5) * pi)
//
class Box
{
public:

	Box(int dimension, const double* constraint_lower, const double* constraint_upper)
	{
		for (int i = 0; i < dimension; ++i) {
			spii_assert(constraint_upper[i] > constraint_lower[i], "Interval: non-positive size");
		}
		this->dimension = dimension;
		this->a = constraint_lower;
		this->b = constraint_upper;
	}

	template<typename R>
	void t_to_x(R* x, const R* t) const
	{
		using std::atan;
		for (int i = 0; i < dimension; ++i) {
			x[i] = a[i] + (b[i] - a[i]) * (0.318309886183791 * atan(t[i]) + 0.5);
		}
	}

	template<typename R>
	void x_to_t(R* t, const R* x) const
	{
		using std::tan;
		for (int i = 0; i < dimension; ++i) {
			t[i] = tan(((x[i] - a[i]) / (b[i] - a[i]) - 0.5) * 3.141592653589793);
		}
	}

	int x_dimension() const
	{
		return dimension;
	}

	int t_dimension() const
	{
		return dimension;
	}

private:
	int dimension;
	const double* a;
	const double* b;
};


}  // namespace spii
#endif
