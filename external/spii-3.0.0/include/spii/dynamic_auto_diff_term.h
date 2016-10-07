// Petter Strandmark 2013.
#ifndef SPII_DYNAMIC_AUTO_DIFF_TERM_H
#define SPII_DYNAMIC_AUTO_DIFF_TERM_H
//
// This header specialized dynamic versions of AutoDiffTerm,
// allowing the sizes of the variables to be specified at
// runtime.
//
//    AutoDiffTerm<Functor, 2, 3, 5> my_term(arg);
//
// is equivalent to
//
//    AutoDiffTerm<Functor, Dynamic, Dynamic, Dynamic> my_term(2, 3, 5, arg);
//
// Note that the dynamic versions of AutoDiffTerm are slower
// than the equivalent static ones.
//

// The make_differentiable function also supports dynamic
// differentiation.
//
// Examples 
// --------
//
// class Functor1_2
// {
//  	template<typename R>
//		R operator()(const T* x, const T* y)
//		{
//			return x[0]*x[0] + y[0] + y[1];
//		}
//  };
//
//  auto term_1_2 = make_differentiable<Dynamic, Dynamic>(Functor{}, 1, 2);
//

#include <type_traits>
#include <typeinfo>

#include <spii-thirdparty/badiff.h>
#include <spii-thirdparty/fadiff.h>

#include <spii/auto_diff_term.h>

namespace spii {

static const int Dynamic = -1;

//
// 1-variable specialization
//

// Function differentiating a functor taking D variables.
template<typename Functor, typename T>
T dynamic_differentiate_functor(
	const Functor& functor,
	int d,
	const T* x_in,
	T* df)
{
	using namespace fadbad;
	typedef fadbad::F<T> Dual;

	std::vector<Dual> x(d);
	for (int i = 0; i < d; ++i) {
		x[i] = x_in[i];
		x[i].diff(i, d);
	}
	Dual f{functor(x.data())};

	for (int i = 0; i < d; ++i) {
		df[i] = f.d(i);
	}

	return f.x();
}

template<typename Functor>
class AutoDiffTerm<Functor, Dynamic> :
	public Term
{
public:

	template<typename... Args>
	AutoDiffTerm(int d0_, Args&&... args)
		: d0(d0_), functor(std::forward<Args>(args)...)
	{
	}

	virtual int number_of_variables() const override
	{
		return 1;
	}

	virtual int variable_dimension(int var) const override
	{
		return d0;
	}

	virtual void read(std::istream& in) override
	{
		call_read_if_exists(in, functor);
	}

	virtual void write(std::ostream& out) const override
	{
		call_write_if_exists(out, functor);
	}

	virtual double evaluate(double * const * const variables) const override
	{
		return functor(variables[0]);
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const override
	{
		typedef fadbad::F<double> Dual;
		std::vector<Dual> vars(d0);

		for (int i = 0; i < d0; ++i) {
			vars[i] = variables[0][i];
			vars[i].diff(i, d0);
		}

		Dual f{functor(vars.data())};

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		typedef fadbad::F<double> Dual;

		std::vector<Dual> vars(d0);
		for (int i = 0; i < d0; ++i) {
			vars[i] = variables[0][i];
			vars[i].diff(i, d0);
		}

		std::vector<Dual> df(d0);

		Dual f =
			dynamic_differentiate_functor<Functor, Dual>(
				functor,
				d0,
				vars.data(),
				df.data());

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = df[i].x();
			for (int j = 0; j < d0; ++j) {
				(*hessian)[0][0](i, j) = df[i].d(j);
			}
		}

		return f.x();
	}

protected:
	const int d0;
	Functor functor;
};



//
// 2-variable specialization
//
template<typename Functor>
class AutoDiffTerm<Functor, Dynamic, Dynamic>
	: public Term
{
public:

	template<typename... Args>
	AutoDiffTerm(int d0_, int d1_, Args&&... args)
		: d0{d0_}, d1{d1_}, functor(std::forward<Args>(args)...)
	{ }

	virtual int number_of_variables() const override
	{
		return 2;
	}

	virtual int variable_dimension(int var) const override
	{
		switch (var) {
			default:
			case 0: return d0;
			case 1: return d1;
		}
	}

	virtual void read(std::istream& in)
	{
		call_read_if_exists(in, functor);
	}

	virtual void write(std::ostream& out) const
	{
		call_write_if_exists(out, functor);
	}

	virtual double evaluate(double * const * const variables) const override
	{
		return functor(variables[0], variables[1]);
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const override
	{
		typedef fadbad::F<double> Dual;

		std::vector<Dual> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i, d0 + d1);
		}

		std::vector<Dual> vars1(d1);
		int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1, d0 + d1);
		}

		Dual f{functor(vars0.data(), vars1.data())};

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		typedef fadbad::B<fadbad::F<double>> BF;

		std::vector<BF> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].x().diff(i, d0 + d1);
		}

		std::vector<BF> vars1(d1);
		int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].x().diff(offset1 + i, d0 + d1);
		}

		BF f = functor(vars0.data(), vars1.data());
		f.diff(0, 1);

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = vars0[i].d(0).x();

			// D0 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[0][0](i, j) = vars0[i].d(0).d(j);
			}

			// D0 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[0][1](i, j) = vars0[i].d(0).d(offset1 + j);
			}
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = vars1[i].d(0).x();

			// D1 and Ds0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[1][0](i, j) = vars1[i].d(0).d(j);
			}

			// D1 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[1][1](i, j) = vars1[i].d(0).d(offset1 + j);
			}
		}

		return f.x().x();
	}

protected:
	const int d0, d1;
	Functor functor;
};


//
// 3-variable specialization
//
template<typename Functor>
class AutoDiffTerm<Functor, Dynamic, Dynamic, Dynamic>
	: public Term
{
public:

	template<typename... Args>
	AutoDiffTerm(int d0_, int d1_, int d2_, Args&&... args)
		: d0{d0_}, d1{d1_}, d2{d2_}, functor(std::forward<Args>(args)...)
	{ }

	virtual int number_of_variables() const override
	{
		return 3;
	}

	virtual int variable_dimension(int var) const override
	{
		switch (var) {
		default:
		case 0: return d0;
		case 1: return d1;
		case 2: return d2;
		}
	}

	virtual void read(std::istream& in) override
	{
		call_read_if_exists(in, this->functor);
	}

	virtual void write(std::ostream& out) const override
	{
		call_write_if_exists(out, this->functor);
	}

	virtual double evaluate(double * const * const variables) const override
	{
		return functor(variables[0], variables[1], variables[2]);
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const override
	{
		typedef fadbad::F<double> Dual;
		const int number_of_vars = d0 + d1 + d2;

		std::vector<Dual> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i, number_of_vars);
		}

		std::vector<Dual> vars1(d1);
		int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1, number_of_vars);
		}

		std::vector<Dual> vars2(d2);
		int offset2 = d0 + d1;
		for (int i = 0; i < d2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].diff(i + offset2, number_of_vars);
		}

		Dual f(functor(vars0.data(), vars1.data(), vars2.data()));

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		for (int i = 0; i < d2; ++i) {
			(*gradient)[2](i) = f.d(i + offset2);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
		std::vector<Eigen::VectorXd>* gradient,
		std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		typedef fadbad::B<fadbad::F<double>> BF;
		const int number_of_vars = d0 + d1 + d2;

		std::vector<BF> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].x().diff(i, number_of_vars);
		}

		std::vector<BF> vars1(d1);
		int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].x().diff(offset1 + i, number_of_vars);
		}

		std::vector<BF> vars2(d2);
		int offset2 = d0 + d1;
		for (int i = 0; i < d2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].x().diff(offset2 + i, number_of_vars);
		}

		BF f = functor(vars0.data(), vars1.data(), vars2.data());
		f.diff(0, 1);

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = vars0[i].d(0).x();

			// D0 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[0][0](i, j) = vars0[i].d(0).d(j);
			}

			// D0 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[0][1](i, j) = vars0[i].d(0).d(offset1 + j);
			}

			// D0 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[0][2](i, j) = vars0[i].d(0).d(offset2 + j);
			}
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = vars1[i].d(0).x();

			// D1 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[1][0](i, j) = vars1[i].d(0).d(j);
			}

			// D1 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[1][1](i, j) = vars1[i].d(0).d(offset1 + j);
			}

			// D1 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[1][2](i, j) = vars1[i].d(0).d(offset2 + j);
			}
		}

		for (int i = 0; i < d2; ++i) {
			(*gradient)[2](i) = vars2[i].d(0).x();

			// D2 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[2][0](i, j) = vars2[i].d(0).d(j);
			}

			// D2 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[2][1](i, j) = vars2[i].d(0).d(offset1 + j);
			}

			// D2 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[2][2](i, j) = vars2[i].d(0).d(offset2 + j);
			}
		}

		return f.x().x();
	}

protected:
	const int d0, d1, d2;
	Functor functor;
};


//
// 4-variable specialization
//
template<typename Functor>
class AutoDiffTerm<Functor, Dynamic, Dynamic, Dynamic, Dynamic>
	: public Term
{
public:

	template<typename... Args>
	AutoDiffTerm(int d0_, int d1_, int d2_, int d3_, Args&&... args)
		: d0{d0_}, d1{d1_}, d2{d2_}, d3{d3_}, functor(std::forward<Args>(args)...)
	{ }

	virtual int number_of_variables() const override
	{
		return 4;
	}

	virtual int variable_dimension(int var) const override
	{
		switch (var) {
		default:
		case 0: return d0;
		case 1: return d1;
		case 2: return d2;
		case 3: return d3;
		}
	}

	virtual void read(std::istream& in) override
	{
		call_read_if_exists(in, this->functor);
	}

	virtual void write(std::ostream& out) const override
	{
		call_write_if_exists(out, this->functor);
	}

	virtual double evaluate(double * const * const variables) const override
	{
		return functor(variables[0], variables[1], variables[2], variables[3]);
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const override
	{
		typedef fadbad::F<double> Dual;
		const int number_of_vars = d0 + d1 + d2 + d3;

		std::vector<Dual> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i, number_of_vars);
		}

		std::vector<Dual> vars1(d1);
		int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1, number_of_vars);
		}

		std::vector<Dual> vars2(d2);
		int offset2 = d0 + d1;
		for (int i = 0; i < d2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].diff(i + offset2, number_of_vars);
		}

		std::vector<Dual> vars3(d3);
		int offset3 = d0 + d1 + d2;
		for (int i = 0; i < d3; ++i) {
			vars3[i] = variables[3][i];
			vars3[i].diff(i + offset3, number_of_vars);
		}

		Dual f(functor(vars0.data(), vars1.data(), vars2.data(), vars3.data()));

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		for (int i = 0; i < d2; ++i) {
			(*gradient)[2](i) = f.d(i + offset2);
		}

		for (int i = 0; i < d3; ++i) {
			(*gradient)[3](i) = f.d(i + offset3);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		typedef fadbad::B<fadbad::F<double>> BF;
		const int number_of_vars = d0 + d1 + d2 + d3;

		std::vector<BF> vars0(d0);
		for (int i = 0; i < d0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].x().diff(i, number_of_vars);
		}

		std::vector<BF> vars1(d1);
		const int offset1 = d0;
		for (int i = 0; i < d1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].x().diff(offset1 + i, number_of_vars);
		}

		std::vector<BF> vars2(d2);
		const int offset2 = d0 + d1;
		for (int i = 0; i < d2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].x().diff(offset2 + i, number_of_vars);
		}

		std::vector<BF> vars3(d3);
		const int offset3 = d0 + d1 + d2;
		for (int i = 0; i < d3; ++i) {
			vars3[i] = variables[3][i];
			vars3[i].x().diff(offset3 + i, number_of_vars);
		}

		BF f = functor(vars0.data(), vars1.data(), vars2.data(), vars3.data());
		f.diff(0, 1);

		for (int i = 0; i < d0; ++i) {
			(*gradient)[0](i) = vars0[i].d(0).x();

			// D0 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[0][0](i, j) = vars0[i].d(0).d(j);
			}

			// D0 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[0][1](i, j) = vars0[i].d(0).d(offset1 + j);
			}

			// D0 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[0][2](i, j) = vars0[i].d(0).d(offset2 + j);
			}

			// D0 and D3
			for (int j = 0; j < d3; ++j) {
				(*hessian)[0][3](i, j) = vars0[i].d(0).d(offset3 + j);
			}
		}

		for (int i = 0; i < d1; ++i) {
			(*gradient)[1](i) = vars1[i].d(0).x();

			// D1 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[1][0](i, j) = vars1[i].d(0).d(j);
			}

			// D1 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[1][1](i, j) = vars1[i].d(0).d(offset1 + j);
			}

			// D1 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[1][2](i, j) = vars1[i].d(0).d(offset2 + j);
			}

			// D1 and D2
			for (int j = 0; j < d3; ++j) {
				(*hessian)[1][3](i, j) = vars1[i].d(0).d(offset3 + j);
			}
		}

		for (int i = 0; i < d2; ++i) {
			(*gradient)[2](i) = vars2[i].d(0).x();

			// D2 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[2][0](i, j) = vars2[i].d(0).d(j);
			}

			// D2 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[2][1](i, j) = vars2[i].d(0).d(offset1 + j);
			}

			// D2 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[2][2](i, j) = vars2[i].d(0).d(offset2 + j);
			}

			// D2 and D3
			for (int j = 0; j < d3; ++j) {
				(*hessian)[2][3](i, j) = vars2[i].d(0).d(offset3 + j);
			}
		}

		for (int i = 0; i < d3; ++i) {
			(*gradient)[3](i) = vars3[i].d(0).x();

			// D3 and D0
			for (int j = 0; j < d0; ++j) {
				(*hessian)[3][0](i, j) = vars3[i].d(0).d(j);
			}

			// D3 and D1
			for (int j = 0; j < d1; ++j) {
				(*hessian)[3][1](i, j) = vars3[i].d(0).d(offset1 + j);
			}

			// D3 and D2
			for (int j = 0; j < d2; ++j) {
				(*hessian)[3][2](i, j) = vars3[i].d(0).d(offset2 + j);
			}

			// D3 and D3
			for (int j = 0; j < d3; ++j) {
				(*hessian)[3][3](i, j) = vars3[i].d(0).d(offset3 + j);
			}
		}

		return f.x().x();
	}

protected:
	const int d0, d1, d2, d3;
	Functor functor;
};

}  // namespace spii


#endif
