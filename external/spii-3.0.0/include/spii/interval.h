// Petter Strandmark 2013.
#ifndef SPII_INTERVAL_H
#define SPII_INTERVAL_H

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

namespace spii {

template<typename R>
class Interval
{
public:
	static const R infinity;

	Interval(R _lower, R _upper) :
		lower(_lower),
		upper(_upper)
	{
	}

	Interval(R value) :
		lower(value),
		upper(value)
	{
	}

	Interval() { }

	template<typename R2>
	Interval(const Interval<R2>& interval)
	{
		*this = interval;
	}

	template<typename R2>
	Interval<R>& operator = (const Interval<R2>& interval)
	{
		this->lower = interval.lower;
		this->upper = interval.upper;
		return *this;
	}

	R get_lower() const
	{
		return lower;
	}

	R get_upper() const
	{
		return upper;
	}

	template<typename R2>
	bool operator == (const Interval<R2>& interval) const
	{
		return this->lower == interval.lower && this->upper == interval.upper;
	}

	template<typename R2>
	bool operator < (const Interval<R2>& interval) const
	{
		if (this->lower < interval.lower) {
			return true;
		}
		else if (interval.lower < this->lower) {
			return false;
		}
		return this->upper < interval.upper;
	}

	Interval<R>& operator += (const Interval<R>& interval)
	{
		this->lower += interval.lower;
		this->upper += interval.upper;
		return *this;
	}

	template<typename R2>
	Interval<R>& operator += (const R2& scalar)
	{
		this->lower += scalar;
		this->upper += scalar;
		return *this;
	}

	template<typename T>
	Interval<R> operator + (const T& rhs) const
	{
		Interval<R> result(*this);
		result += rhs;
		return result;
	}

	Interval<R>& operator -= (const Interval<R>& interval)
	{
		this->lower -= interval.upper;
		this->upper -= interval.lower;
		return *this;
	}

	template<typename R2>
	Interval<R>& operator -= (const R2& scalar)
	{
		this->lower -= scalar;
		this->upper -= scalar;
		return *this;
	}

	template<typename T>
	Interval<R> operator - (const T& rhs) const
	{
		Interval<R> result(*this);
		result -= rhs;
		return result;
	}

	Interval<R> operator - () const
	{
		Interval<R> result(-this->upper, -this->lower);
		return result;
	}

	Interval<R>& operator *= (const Interval<R>& interval)
	{
		R a = this->lower * interval.lower;
		R b = this->lower * interval.upper;
		R c = this->upper * interval.lower;
		R d = this->upper * interval.upper;
		this->lower = std::min(std::min(a, b), std::min(c, d));
		this->upper = std::max(std::min(a, b), std::max(c, d));
		return *this;
	}

	template<typename R2>
	Interval<R>& operator *= (const R2& scalar)
	{
		R a = scalar * this->lower;
		R b = scalar * this->upper;
		this->lower = std::min(a, b);
		this->upper = std::max(a, b);
		return *this;
	}

	template<typename T>
	Interval<R> operator * (const T& rhs) const
	{
		Interval<R> result(*this);
		result *= rhs;
		return result;
	}

	Interval<R>& operator /= (const Interval<R>& interval)
	{
		if (interval.lower <= 0 && interval.upper >= 0) {
			this->lower = -Interval<R>::infinity;
			this->upper =  Interval<R>::infinity;
			return *this;
		}
		R a = this->lower / interval.lower;
		R b = this->lower / interval.upper;
		R c = this->upper / interval.lower;
		R d = this->upper / interval.upper;
		this->lower = std::min(std::min(a, b), std::min(c, d));
		this->upper = std::max(std::min(a, b), std::max(c, d));
		return *this;
	}

	template<typename R2>
	Interval<R>& operator /= (const R2& scalar)
	{
		if (scalar == 0) {
			this->lower = -Interval<R>::infinity;
			this->upper =  Interval<R>::infinity;
			return *this;
		}
		R a = this->lower / scalar;
		R b = this->upper / scalar;
		this->lower = std::min(a, b);
		this->upper = std::max(a, b);
		return *this;
	}

	template<typename T>
	Interval<R> operator / (const T& rhs) const
	{
		Interval<R> result(*this);
		result /= rhs;
		return result;
	}

private:
	R lower, upper;
};

typedef std::vector<Interval<double>> IntervalVector;

template<typename R>
const R Interval<R>::infinity = std::numeric_limits<R>::has_infinity ? 
                                std::numeric_limits<R>::infinity() :
                                std::numeric_limits<R>::max();

template<typename R>
Interval<R> operator + (const R& lhs, const Interval<R>& rhs)
{
	return rhs + lhs;
}

template<typename R>
Interval<R> operator - (const R& lhs, const Interval<R>& rhs)
{
	return lhs + (-rhs);
}

template<typename R>
Interval<R> operator * (const R& lhs, const Interval<R>& rhs)
{
	return rhs * lhs;
}

template<typename R>
Interval<R> operator / (const R& lhs, const Interval<R>& rhs)
{
	if (rhs.get_lower() <= 0 && rhs.get_upper() >= 0) {
		return Interval<R>(-Interval<R>::infinity, Interval<R>::infinity);
	}
	R a = lhs / rhs.get_lower();
	R b = lhs / rhs.get_upper();
	return Interval<R>(std::min(a, b), std::max(a, b));
}

template<typename R>
Interval<R> cos(const Interval<R>& arg)
{
	using std::ceil;
	using std::cos;
	using std::fmod;
	using std::min;
	using std::max;

	const R pi = 3.141592653589793;

	if (arg.get_upper() - arg.get_lower() >= 2*pi) {
		return Interval<R>(-1, 1);
	}

	R lower = min(cos(arg.get_lower()), cos(arg.get_upper()));
	R upper = max(cos(arg.get_lower()), cos(arg.get_upper()));

	// Transform lower bound to [0, 2pi].
	double low = fmod(arg.get_lower(), 2*pi);
	while (low < 0) low += 2*pi;
	// Transform higher bound in the same way.
	double up = arg.get_upper() + (low - arg.get_lower());

	// Check if there is a point of the form pi + k*2*pi on the interval.
	if (low <= pi && pi <= up) {
		lower = -1.0;
	}

	// Check if there is a point of the form k*2*pi on the interval.
	if (low == 0 || up > 2*pi) {
		upper = 1.0;
	}
	
	return Interval<R>(lower, upper);
}

template<typename R>
Interval<R> sin(const Interval<R>& arg)
{
	const R pi = 3.141592653589793;
	return cos(arg - pi/2.0);
}

template<typename R>
Interval<R> pow(const Interval<R>& arg, int power)
{
	using std::pow;

	if (power == 0) {
		return Interval<R>(1, 1);
	}
	else if (power < 0) {
		return 1.0 / pow(arg, -power);
	}
	else if (power % 2 == 0 && arg.get_lower() <= 0 && arg.get_upper() >= 0) {
		double a = pow(arg.get_lower(), power);
		double b = pow(arg.get_upper(), power);
		return Interval<R>(0, std::max(a, b));
	}
	else {
		double a = pow(arg.get_lower(), power);
		double b = pow(arg.get_upper(), power);
		return Interval<R>(a, b);
	}
}

template<typename R>
Interval<R> sqrt(const Interval<R>& arg)
{
	using std::sqrt;
	return Interval<R>(sqrt(arg.get_lower()), sqrt(arg.get_upper()));
}

template<typename R>
Interval<R> exp(const Interval<R>& arg)
{
	using std::exp;
	return Interval<R>(exp(arg.get_lower()), exp(arg.get_upper()));
}

template<typename R>
Interval<R> log(const Interval<R>& arg)
{
	using std::log;
	return Interval<R>(log(arg.get_lower()), log(arg.get_upper()));
}

template<typename R>
Interval<R> log10(const Interval<R>& arg)
{
	using std::log10;
	return Interval<R>(log10(arg.get_lower()), log10(arg.get_upper()));
}

template<typename R>
Interval<R> abs(const Interval<R>& arg)
{
	using std::abs;
	using std::max;
	using std::min;

	R a = abs(arg.get_lower());
	R b = abs(arg.get_upper());
	R upper = max(a, b);
	R lower;
	if (arg.get_lower() <= 0 && arg.get_upper() >= 0) {
		lower = 0;
	}
	else {
		lower = min(a, b);
	}
	return Interval<R>(lower, upper);
}

template<typename R>
std::ostream& operator << (std::ostream& out, const Interval<R>& interval)
{
	out << '(' << interval.get_lower() << ", " << interval.get_upper() << ')';
	return out;
}

}  // namespace spii
#endif
