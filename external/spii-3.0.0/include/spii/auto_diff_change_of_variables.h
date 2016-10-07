// Petter Strandmark 2012.
#ifndef SPII_AUTO_DIFF_CHANGE_OF_VARIABLES_H
#define SPII_AUTO_DIFF_CHANGE_OF_VARIABLES_H

#include <spii-thirdparty/fadiff.h>

#include <spii/change_of_variables.h>

namespace spii {

template<typename Change>
class AutoDiffChangeOfVariables :
	public ChangeOfVariables
{
public:
	AutoDiffChangeOfVariables(Change* c)
	{
		this->change = c;
	}

	virtual ~AutoDiffChangeOfVariables()
	{
		delete this->change;
	}

	virtual void t_to_x(double* x, const double* t) const
	{
		change->t_to_x(x, t);
	}

	virtual void x_to_t(double* t, const double* x) const
	{
		change->x_to_t(t, x);
	}

	virtual int x_dimension() const
	{
		return change->x_dimension();
	}

	virtual int t_dimension() const
	{
		return change->t_dimension();
	}

	virtual void update_gradient(double* t_gradient,
	                             const double* t_input,
	                             const double* x_gradient) const
	{
		int n_x = x_dimension();
		int n_t = t_dimension();

		std::vector<fadbad::F<long double> > x(n_x);
		std::vector<fadbad::F<long double> > t(n_t);
		for (int j = 0; j < n_t; ++j) {
			t[j] = t_input[j];
			t[j].diff(j, n_t);
		}

		// x = x(t)
		// Because we want to compute x'(t).
		change->t_to_x(&x[0], &t[0]);

		//for (int j = 0; j < n_t; ++j) {
		//	std::cerr << "t[" << j << "] = " << t[j].x() << '\n';
		//}
		//for (int i = 0; i < n_x; ++i) {
		//	std::cerr << "x[" << i << "] = " << x[i].x() << '\n';
		//}
		//for (int j = 0; j < n_t; ++j) {
		//	for (int i = 0; i < n_x; ++i) {
		//		std::printf("dx[%d]/dt[%d] = %.15e\n",i, j, x[i].d(j));
		//	}
		//}

		// df/dt_j = dx_1/dt_j * df/dx_1 + ... + dx_n/dt_j *  df/dx_n)
		for (int j = 0; j < n_t; ++j) {
			for (int i = 0; i < n_x; ++i) {
				t_gradient[j] +=
					x[i].d(j) * x_gradient[i];
			}
		}
	}

	void update_hessian() const;
private:
	Change* change;
};


}  // namespace spii

#endif
