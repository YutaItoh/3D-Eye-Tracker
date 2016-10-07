#ifndef SPII_INTERVAL_TERM
#define SPII_INTERVAL_TERM

#include <spii/auto_diff_term.h>

namespace spii
{

template<typename Functor, int... D>
class IntervalTerm :
	public AutoDiffTerm<Functor, D...>
{

};

//
// 1-variable specialization
//
template<typename Functor, int D0>
class IntervalTerm<Functor, D0> :
	public AutoDiffTerm<Functor, D0>
{
public:
	
	template<typename... Args>
	IntervalTerm(Args&&... args)
		: AutoDiffTerm<Functor, D0>(std::forward<Args>(args)...)
	{ }

	virtual Interval<double> evaluate_interval(const Interval<double> * const * const variables) const
	{
		return this->functor(variables[0]);
	};
};

//
// 2-variable specialization
//
template<typename Functor, int D0, int D1>
class IntervalTerm<Functor, D0, D1> :
	public AutoDiffTerm<Functor, D0, D1>
{
public:
	template<typename... Args>
	IntervalTerm(Args&&... args)
		: AutoDiffTerm<Functor, D0, D1>(std::forward<Args>(args)...)
	{ }

	virtual Interval<double> evaluate_interval(const Interval<double> * const * const variables) const
	{
		return this->functor(variables[0], variables[1]);
	};
};

}  // namespace spii
#endif
