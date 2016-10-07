// Petter Strandmark 2012.
#ifndef SPII_TERM_H
#define SPII_TERM_H
// The Term class defines a single term in an objective function.
// Ususally a term is created via the AutoDiffTerm template class
// so that the derivatives do not have to be explicitly computed.

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>
using std::size_t;

#include <Eigen/Core>

#include <spii/spii.h>
#include <spii/interval.h>

namespace spii
{

class SPII_API Term
{
public:
	virtual ~Term() {};
	virtual int number_of_variables() const       = 0;
	virtual int variable_dimension(int var) const = 0;
	virtual double evaluate(double * const * const variables) const = 0;
	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const = 0;
	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const = 0;

	// This function only needs to be implemented if interval arithmetic is
	// desired.
	virtual Interval<double> evaluate_interval(const Interval<double> * const * const variables) const;
	// Overload these if input/output is required.
	virtual void read(std::istream& in);
	virtual void write(std::ostream& out) const;
};

SPII_API std::ostream& operator << (std::ostream& out, const Term& term);
SPII_API std::istream& operator >> (std::istream& in, Term& term);

//
// IntElements is a helper class that extracts the nth
// element from an integer variable template pack.
//
template<int D0, int... DN>
struct IntElements
{
	static_assert(D0 >= 1, "Dimensions must be positive.");

	static int get_position(int pos) 
	{
		if (pos == 0) {
			return D0;
		}
		else {
			return IntElements<DN...>::get_position(pos - 1);
		}
	}
};
template<int D0>
struct IntElements<D0>
{
	static_assert(D0 >= 1, "Dimensions must be positive.");

	static int get_position(int pos)
	{
		if (pos == 0) {
			return D0;
		}
		else {
			return -1;
		}
	}
};

template<int... D>
class SizedTerm :
	public Term
{
public:
	virtual int number_of_variables() const override
	{
		return sizeof...(D);
	}

	virtual int variable_dimension(int var) const override
	{
		return IntElements<D...>::get_position(var);
	}
};

}  // namespace spii
#endif
