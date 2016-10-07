// Petter Strandmark 2012–2013.
#ifndef SPII_AUTO_DIFF_TERM_H
#define SPII_AUTO_DIFF_TERM_H

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <spii-thirdparty/badiff.h>
#include <spii-thirdparty/fadiff.h>

#include <spii/term.h>

namespace spii {

//
// Term which allows for automatic computation of derivatives. It is
// used in the following way:
//
//   auto term = make_shared<AutoDiffTerm<Functor, 1>>(arg1, arg2, ...)
//
// where arg1, arg2, etc. are arguments to the constructor of Functor.
//
// Note: The size arguments D... are supposed to be reasonably small,
//       as the memory allocated on the stack by this class is
//       O(sum(D...)^2).
template<typename Functor, int... D>
class AutoDiffTerm;

// Creates a differentiable term from a generic lambda or functor
// and argument sizes.
//
// Examples 
// --------
//
// class Functor1
// {
//  	template<typename R>
//		R operator()(const T* x)
//		{
//			return x[0]*x[0]
//		}
//  };
//
//  auto term_1 = make_differentiable<1>(Functor{});
//
//
//	auto lambda_a =
//		[](auto x)              // C++14 generic lambda
//		{
//			auto d0 =  x[1] - x[0]*x[0];
//			auto d1 =  1 - x[0];
//			return 100 * d0*d0 + d1*d1;
//		};
//
//	auto term_a = make_differentiable<2>(lambda_a);
//
//
//	auto lambda_b =
//		[](auto x, auto y)      // C++14 generic lambda
//		{
//			auto d0 =  y[0] - x[0]*x[0];
//			auto d1 =  1 - x[0];
//			return 100 * d0*d0 + d1*d1;
//		};
//
//	auto term_b = make_differentiable<1, 1>(lambda);
// 
// 
// The dimension arguments are only used if Dynamic is specified
// as arg_sizes.
template<int... arg_sizes, typename Functor, typename... Ints>
std::shared_ptr<Term> make_differentiable(Functor&& lambda, Ints... dimensions)
{
	typedef typename std::remove_reference<Functor>::type FunctorClass;
	typedef AutoDiffTerm<FunctorClass, arg_sizes...> TermType;
	return std::make_shared<TermType>(dimensions..., std::forward<Functor>(lambda));
}


//
// Create a has_write struct to test whether a class T has a member
// function void T::write
//
// has_write<T, B>::value == true iff "void T::write(B)" exists.
//
template<class T, class A0>
static auto test_write(A0&& a0, int) -> decltype(std::declval<T>().write(a0), void());
template<class, class A0>
static char test_write(A0&&, long);
template<class T, class Arg>
struct has_write : std::is_void<decltype(test_write<T>(std::declval<Arg>(), 0))>{};
// Test has_write.
struct HasWriteTest1{ void write(int){} };
struct HasWriteTest2{};
static_assert(has_write<HasWriteTest1, int>::value  == true, "HasWriteTest1 failed.");
static_assert(has_write<HasWriteTest2, int>::value  == false, "HasWriteTest2 failed.");

// Same thing, but for a read member function.
template<class T, class A0>
static auto test_read(A0&& a0, int) -> decltype(std::declval<T>().read(a0), void());
template<class, class A0>
static char test_read(A0&&, long);
template<class T, class Arg>
struct has_read : std::is_void<decltype(test_read<T>(std::declval<Arg>(), 0))>{};
// Test test_read.
struct HasReadTest1{ void read(std::istream&){} };
struct HasReadTest2{};
static_assert(has_read<HasReadTest1, std::istream&>::value  == true,  "HasReadTest1 failed.");
static_assert(has_read<HasReadTest2, std::istream&>::value  == false, "HasReadTest2 failed.");

template<typename Functor>
typename std::enable_if<has_write<Functor, std::ostream&>::value, void>::type 
    call_write_if_exists(std::ostream& out, const Functor& functor)
{
    functor.write(out);
}
template<typename Functor>
typename std::enable_if< ! has_write<Functor, std::ostream&>::value, void>::type 
    call_write_if_exists(std::ostream& out, const Functor& functor)
{
}

template<typename Functor>
typename std::enable_if<has_read<Functor, std::istream&>::value, void>::type 
    call_read_if_exists(std::istream& in, Functor& functor)
{
	functor.read(in);
}
template<typename Functor>
typename std::enable_if< ! has_read<Functor, std::istream&>::value, void>::type 
    call_read_if_exists(std::istream& in, const Functor& functor)
{
}
 


// to_double(x) returns the real part of x, disregarding
// any derivatives.
inline double to_double(double x)
{
	return x;
}
inline float to_double(float x)
{
	return x;
}
template<typename R>
inline double to_double(R& x)
{
	return to_double(x.x());
}

// Function differentiating a functor taking D variables.
template<typename Functor, typename T, int D>
T differentiate_functor(
	const Functor& functor,
	const T* x_in,
	T* df)
{
	using namespace fadbad;

	F<T, D> x[D];
	for (int i=0; i<D; ++i) {
		x[i] = x_in[i];
		x[i].diff(i);
	}
	F<T, D> f(functor(x));

	for (int i=0; i<D; ++i) {
		df[i] = f.d(i);
	}

	return f.x();
}

//
// 1-variable specialization
//
template<typename Functor, int D0>
class AutoDiffTerm<Functor, D0> :
	public SizedTerm<D0>
{
public:

	template<typename... Args>
	AutoDiffTerm(Args&&... args)
		: functor(std::forward<Args>(args)...)
	{
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
		using namespace fadbad;

		F<double, D0> vars[D0];
		for (int i = 0; i < D0; ++i) {
			vars[i] = variables[0][i];
			vars[i].diff(i);
		}

		F<double, D0> f(functor(vars));

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		using namespace fadbad;
		#ifdef USE_BF_DIFFERENTIATION
			typedef B< F<double, D0> > BF;

			BF vars[D0];
			for (int i = 0; i < D0; ++i) {
				vars[i] = variables[0][i];
				vars[i].x().diff(i);
			}

			BF f = (*functor)(vars);
			f.diff(0, 1);

			for (int i = 0; i < D0; ++i) {
				(*gradient)[0](i) = vars[i].d(0).x();
				for (int j = 0; j < D0; ++j) {
					(*hessian)[0][0](i, j) = vars[i].d(0).d(j);
				}
			}

			return f.x().x();
		#else
			F<double, D0> vars[D0];
			for (int i = 0; i < D0; ++i) {
				vars[i] = variables[0][i];
				vars[i].diff(i);
			}

			F<double, D0> df[D0];

			F<double, D0> f(
				differentiate_functor<Functor, F<double, D0>, D0>(
					functor,
					vars,
					df)
				);

			for (int i = 0; i < D0; ++i) {
				(*gradient)[0](i) = df[i].x();
				for (int j = 0; j < D0; ++j) {
					(*hessian)[0][0](i, j) = df[i].d(j);
				}
			}

			return f.x();
		#endif
	}

protected:
	Functor functor;
};


template<typename Functor, int D0, int D1>
class Functor2_to_1
{
public:
	Functor2_to_1(const Functor& functor_in)
		: functor(functor_in)
	{
	}

	template<typename R>
	R operator()(const R* const x) const
	{
		const R* const x0 = &x[0];
		const R* const x1 = &x[D0];
		return functor(x0, x1);
	}

private:
	const Functor& functor;
};

//
// 2-variable specialization
//
template<typename Functor, int D0, int D1>
class AutoDiffTerm<Functor, D0, D1> :
	public SizedTerm<D0, D1>
{
public:

	template<typename... Args>
	AutoDiffTerm(Args&&... args)
		: functor(std::forward<Args>(args)...)
	{
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
		using namespace fadbad;

		F<double, D0 + D1> vars0[D0];
		for (int i = 0; i < D0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i);
		}

		F<double, D0 + D1> vars1[D1];
		int offset1 = D0;
		for (int i = 0; i < D1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1);
		}

		F<double, D0 + D1> f(functor(vars0, vars1));

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < D1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		using namespace fadbad;
		#ifdef USE_BF_DIFFERENTIATION
			typedef B< F<double, D0 + D1> > BF;

			BF vars0[D0];
			for (int i = 0; i < D0; ++i) {
				vars0[i] = variables[0][i];
				vars0[i].x().diff(i);
			}

			BF vars1[D1];
			int offset1 = D0;
			for (int i = 0; i < D1; ++i) {
				vars1[i] = variables[1][i];
				vars1[i].x().diff(offset1 + i);
			}

			BF f = (*functor)(vars0, vars1);
			f.diff(0, 1);

			for (int i = 0; i < D0; ++i) {
				(*gradient)[0](i) = vars0[i].d(0).x();

				// D0 and D0
				for (int j = 0; j < D0; ++j) {
					(*hessian)[0][0](i, j) = vars0[i].d(0).d(j);
				}

				// D0 and D1
				for (int j = 0; j < D1; ++j) {
					(*hessian)[0][1](i, j) = vars0[i].d(0).d(offset1 + j);
				}
			}

			for (int i = 0; i < D1; ++i) {
				(*gradient)[1](i) = vars1[i].d(0).x();

				// D1 and D0
				for (int j = 0; j < D0; ++j) {
					(*hessian)[1][0](i, j) = vars1[i].d(0).d(j);
				}

				// D1 and D1
				for (int j = 0; j < D1; ++j) {
					(*hessian)[1][1](i, j) = vars1[i].d(0).d(offset1 + j);
				}
			}

			return f.x().x();
		#else
			F<double, D0 + D1> vars[D0 + D1];
			F<double, D0 + D1>   df[D0 + D1];

			// Initialize variables
			for (int i = 0; i < D0; ++i) {
				vars[i] = variables[0][i];
				vars[i].diff(i);
			}
			int offset1 = D0;
			for (int i = 0; i < D1; ++i) {
				vars[offset1 + i] = variables[1][i];
				vars[offset1 + i].diff(offset1 + i);
			}

			// Evaluate function
			typedef Functor2_to_1<Functor, D0, D1> Functor21;
			Functor21 functor21(functor);
			F<double, D0 + D1> f(
				differentiate_functor<Functor21, F<double, D0 + D1>, D0 + D1>(
					functor21,
					vars,
					df)
				);

			for (int i = 0; i < D0; ++i) {
				(*gradient)[0](i) = df[i].x();

				// D0 and D0
				for (int j = 0; j < D0; ++j) {
					(*hessian)[0][0](i, j) = df[i].d(j);
				}

				// D0 and D1
				for (int j = 0; j < D1; ++j) {
					(*hessian)[0][1](i, j) = df[i].d(offset1 + j);
				}
			}

			for (int i = 0; i < D1; ++i) {
				(*gradient)[1](i) = df[i + offset1].x();;

				// D1 and D0
				for (int j = 0; j < D0; ++j) {
					(*hessian)[1][0](i, j) = df[i + offset1].d(j);;
				}

				// D1 and D1
				for (int j = 0; j < D1; ++j) {
					(*hessian)[1][1](i, j) = df[i + offset1].d(j + offset1);
				}
			}

			return f.x();
		#endif
	}

protected:
	Functor functor;
};



template<typename Functor, int D0, int D1, int D2>
class Functor3_to_1
{
public:
	Functor3_to_1(const Functor& functor_in)
		: functor(functor_in)
	{
	}

	template<typename R>
	R operator()(const R* const x) const
	{
		const R* const x0 = &x[0];
		const R* const x1 = &x[D0];
		const R* const x2 = &x[D0 + D1];
		return functor(x0, x1, x2);
	}

private:
	const Functor& functor;
};

//
// 3-variable specialization
//
template<typename Functor, int D0, int D1, int D2>
class AutoDiffTerm<Functor, D0, D1, D2> :
	public SizedTerm<D0, D1, D2>
{
public:

	template<typename... Args>
	AutoDiffTerm(Args&&... args)
		: functor(std::forward<Args>(args)...)
	{
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
		using namespace fadbad;
		typedef F<double, D0 + D1 + D2> Dual;

		Dual vars0[D0];
		for (int i = 0; i < D0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i);
		}

		Dual vars1[D1];
		int offset1 = D0;
		for (int i = 0; i < D1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1);
		}

		Dual vars2[D2];
		int offset2 = D0 + D1;
		for (int i = 0; i < D2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].diff(i + offset2);
		}

		Dual f(functor(vars0, vars1, vars2));

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < D1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		for (int i = 0; i < D2; ++i) {
			(*gradient)[2](i) = f.d(i + offset2);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		using namespace fadbad;
		typedef F<double, D0 + D1 + D2> Dual;

		Dual vars[D0 + D1 + D2];
		Dual   df[D0 + D1 + D2];

		// Initialize variables
		for (int i = 0; i < D0; ++i) {
			vars[i] = variables[0][i];
			vars[i].diff(i);
		}
		int offset1 = D0;
		for (int i = 0; i < D1; ++i) {
			vars[offset1 + i] = variables[1][i];
			vars[offset1 + i].diff(offset1 + i);
		}
		int offset2 = D0 + D1;
		for (int i = 0; i < D2; ++i) {
			vars[offset2 + i] = variables[2][i];
			vars[offset2 + i].diff(offset2 + i);
		}

		// Evaluate function
		typedef Functor3_to_1<Functor, D0, D1, D2> Functor31;
		Functor31 functor31(functor);
		F<double, D0 + D1 + D2> f(
			differentiate_functor<Functor31, F<double, D0 + D1 + D2>, D0 + D1 + D2>(
				functor31,
				vars,
				df)
			);

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = df[i].x();

			// D0 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[0][0](i, j) = df[i].d(j);
			}

			// D0 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[0][1](i, j) = df[i].d(offset1 + j);
			}

			// D0 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[0][2](i, j) = df[i].d(offset2 + j);
			}
		}

		for (int i = 0; i < D1; ++i) {
			(*gradient)[1](i) = df[i + offset1].x();;

			// D1 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[1][0](i, j) = df[i + offset1].d(j);;
			}

			// D1 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[1][1](i, j) = df[i + offset1].d(j + offset1);
			}

			// D1 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[1][2](i, j) = df[i + offset1].d(j + offset2);
			}
		}

		for (int i = 0; i < D2; ++i) {
			(*gradient)[2](i) = df[i + offset2].x();;

			// D2 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[2][0](i, j) = df[i + offset2].d(j);
			}

			// D2 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[2][1](i, j) = df[i + offset2].d(j + offset1);
			}

			// D2 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[2][2](i, j) = df[i + offset2].d(j + offset2);
			}
		}

		return f.x();
	}

protected:
	Functor functor;
};

//
// 4-variable specialization
//
template<typename Functor, int D0, int D1, int D2, int D3>
class Functor4_to_1
{
public:
	Functor4_to_1(const Functor& functor_in)
		: functor(functor_in)
	{
	}

	template<typename R>
	R operator()(const R* const x) const
	{
		const R* const x0 = &x[0];
		const R* const x1 = &x[D0];
		const R* const x2 = &x[D0 + D1];
		const R* const x3 = &x[D0 + D1 + D2];

		return functor(x0, x1, x2, x3);
	}

private:
	const Functor& functor;
};

template<typename Functor, int D0, int D1, int D2, int D3>
class AutoDiffTerm<Functor, D0, D1, D2, D3> :
	public SizedTerm<D0, D1, D2, D3>
{
public:

	template<typename... Args>
	AutoDiffTerm(Args&&... args)
		: functor(std::forward<Args>(args)...)
	{
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
		using namespace fadbad;
		typedef F<double, D0 + D1 + D2 + D3> Dual;

		Dual vars0[D0];
		for (int i = 0; i < D0; ++i) {
			vars0[i] = variables[0][i];
			vars0[i].diff(i);
		}

		Dual vars1[D1];
		int offset1 = D0;
		for (int i = 0; i < D1; ++i) {
			vars1[i] = variables[1][i];
			vars1[i].diff(i + offset1);
		}

		Dual vars2[D2];
		int offset2 = D0 + D1;
		for (int i = 0; i < D2; ++i) {
			vars2[i] = variables[2][i];
			vars2[i].diff(i + offset2);
		}

		Dual vars3[D3];
		int offset3 = D0 + D1 + D2;
		for (int i = 0; i < D3; ++i) {
			vars3[i] = variables[3][i];
			vars3[i].diff(i + offset3);
		}

		Dual f(functor(vars0, vars1, vars2, vars3));

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = f.d(i);
		}

		for (int i = 0; i < D1; ++i) {
			(*gradient)[1](i) = f.d(i + offset1);
		}

		for (int i = 0; i < D2; ++i) {
			(*gradient)[2](i) = f.d(i + offset2);
		}

		for (int i = 0; i < D3; ++i) {
			(*gradient)[3](i) = f.d(i + offset3);
		}

		return f.x();
	}

	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		using namespace fadbad;
		typedef F<double, D0 + D1 + D2 + D3> Dual;

		Dual vars[D0 + D1 + D2 + D3];
		Dual   df[D0 + D1 + D2 + D3];

		// Initialize variables
		for (int i = 0; i < D0; ++i) {
			vars[i] = variables[0][i];
			vars[i].diff(i);
		}
		int offset1 = D0;
		for (int i = 0; i < D1; ++i) {
			vars[offset1 + i] = variables[1][i];
			vars[offset1 + i].diff(offset1 + i);
		}
		int offset2 = D0 + D1;
		for (int i = 0; i < D2; ++i) {
			vars[offset2 + i] = variables[2][i];
			vars[offset2 + i].diff(offset2 + i);
		}
		int offset3 = D0 + D1 + D2;
		for (int i = 0; i < D3; ++i) {
			vars[offset3 + i] = variables[3][i];
			vars[offset3 + i].diff(offset3 + i);
		}

		// Evaluate function
		typedef Functor4_to_1<Functor, D0, D1, D2, D3> Functor41;
		Functor41 functor41(functor);
		F<double, D0 + D1 + D2 + D3> f(
			differentiate_functor<Functor41, F<double, D0 + D1 + D2 + D3>, D0 + D1 + D2 + D3>(
				functor41,
				vars,
				df)
			);

		for (int i = 0; i < D0; ++i) {
			(*gradient)[0](i) = df[i].x();

			// D0 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[0][0](i, j) = df[i].d(j);
			}

			// D0 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[0][1](i, j) = df[i].d(offset1 + j);
			}

			// D0 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[0][2](i, j) = df[i].d(offset2 + j);
			}

			// D0 and D2
			for (int j = 0; j < D3; ++j) {
				(*hessian)[0][3](i, j) = df[i].d(offset3 + j);
			}
		}

		for (int i = 0; i < D1; ++i) {
			(*gradient)[1](i) = df[i + offset1].x();;

			// D1 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[1][0](i, j) = df[i + offset1].d(j);;
			}

			// D1 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[1][1](i, j) = df[i + offset1].d(j + offset1);
			}

			// D1 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[1][2](i, j) = df[i + offset1].d(j + offset2);
			}

			// D1 and D3
			for (int j = 0; j < D3; ++j) {
				(*hessian)[1][3](i, j) = df[i + offset1].d(j + offset3);
			}
		}

		for (int i = 0; i < D2; ++i) {
			(*gradient)[2](i) = df[i + offset2].x();;

			// D2 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[2][0](i, j) = df[i + offset2].d(j);
			}

			// D2 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[2][1](i, j) = df[i + offset2].d(j + offset1);
			}

			// D2 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[2][2](i, j) = df[i + offset2].d(j + offset2);
			}

			// D2 and D3
			for (int j = 0; j < D3; ++j) {
				(*hessian)[2][3](i, j) = df[i + offset2].d(j + offset3);
			}
		}

		for (int i = 0; i < D3; ++i) {
			(*gradient)[3](i) = df[i + offset3].x();;

			// D3 and D0
			for (int j = 0; j < D0; ++j) {
				(*hessian)[3][0](i, j) = df[i + offset3].d(j);
			}

			// D3 and D1
			for (int j = 0; j < D1; ++j) {
				(*hessian)[3][1](i, j) = df[i + offset3].d(j + offset1);
			}

			// D3 and D2
			for (int j = 0; j < D2; ++j) {
				(*hessian)[3][2](i, j) = df[i + offset3].d(j + offset2);
			}

			// D3 and D3
			for (int j = 0; j < D3; ++j) {
				(*hessian)[3][3](i, j) = df[i + offset3].d(j + offset3);
			}
		}

		return f.x();
	}

protected:
	Functor functor;
};


//
// General (N variable) version.
//

// Takes a double** variables and calls
//
//   functor(variables[0], variables[1], ..., variables[N])
//
template <typename Functor, int... D>
struct DoubleFunctorCaller;

template <typename Functor, int D0, int... DN>
struct DoubleFunctorCaller<Functor, D0, DN...>
{
	template <typename... T>
	double call(const Functor& functor,
	            double * const * const variables,
				T... previous_arguments)
	{
		DoubleFunctorCaller<Functor, DN...> next_caller;
		return next_caller.call(functor, variables + 1, previous_arguments..., variables[0]);
	}
};

template <typename Functor>
struct DoubleFunctorCaller<Functor>
{
	template <typename... T>
	double call(const Functor& functor,
	            double * const * const variables,
	            T... arguments)
	{
		return functor(arguments...);
	}
};


template<int... D>
struct IntSum;

template<int D0, int... DN>
struct IntSum<D0, DN...>
{
	static const int value = D0 + IntSum<DN...>::value;
};

template<>
struct IntSum<>
{
	static const int value = 0;
};

static_assert(IntSum<5>::value == 5, "Sum test failed.");
static_assert(IntSum<5, 2>::value == 5 + 2, "Sum test failed.");
static_assert(IntSum<5, 2, 3>::value == 5 + 2 + 3, "Sum test failed.");
static_assert(IntSum<5, 2, 3, 5>::value == 5 + 2 + 3 + 5, "Sum test failed.");

// Calls functor with dual numbers.
//
template <typename Functor, typename R, int... D>
struct DualFunctorCaller;

template <typename Functor, typename R, int D0, int... DN>
struct DualFunctorCaller<Functor, R, D0, DN...>
{
	R call(const Functor& functor,
	       double * const * const variables)
	{
		return call_internal(functor, variables, 0);
	}

	template <typename... T>
	R call_internal(const Functor& functor,
	                double * const * const variables,
		            int offset,
	                T&... previous_arguments)
	{
		R x[D0];
		for (int i = 0; i < D0; ++i) {
			x[i] = (*variables)[i];
			x[i].diff(i + offset);
		}

		DualFunctorCaller<Functor, R, DN...> next_caller;
		return next_caller.call_internal(functor, variables + 1, offset + D0, previous_arguments..., x);
	}
};

template <typename Functor, typename R>
struct DualFunctorCaller<Functor, R>
{
	template <typename... T>
	R call_internal(const Functor& functor,
	                double * const * const variables,
		            int offset,
	                T&... arguments)
	{
		return functor(arguments...);
	}
};

//
// Extracts gradient from a dual number.
//
template <typename R, int... D>
struct DualGradientExtractor;

template <typename R, int D0, int... DN>
struct DualGradientExtractor<R, D0, DN...>
{
	void extract(R& dual,
	             Eigen::VectorXd* gradient,
				 int offset = 0)
	{
		for (int i = 0; i < D0; ++i) {
			(*gradient)[i] = dual.d(i + offset);
		}

		DualGradientExtractor<R, DN...> next_extractor;
		return next_extractor.extract(dual, gradient + 1, offset + D0);
	}
};

template <typename R>
struct DualGradientExtractor<R>
{
	void extract(R& dual,
	             Eigen::VectorXd* gradient,
	             int offset)
	{
		// We are done.
	}
};


template<typename Functor, int... D>
class AutoDiffTerm
	: public SizedTerm<D...>
{
public:
	template<typename... Args>
	AutoDiffTerm(Args&&... args)
		: functor(std::forward<Args>(args)...)
	{  }

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
		DoubleFunctorCaller<Functor, D...> caller;
		return caller.call(this->functor, variables);
	}


	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient) const override
	{
		typedef fadbad::F<double, IntSum<D...>::value> Dual;
		DualFunctorCaller<Functor, Dual, D...> caller;
		auto f = caller.call(this->functor, variables);

		DualGradientExtractor<Dual, D...> extractor;
		extractor.extract(f, &((*gradient)[0]));

		return f.x();
	}


	virtual double evaluate(double * const * const variables,
	                        std::vector<Eigen::VectorXd>* gradient,
	                        std::vector< std::vector<Eigen::MatrixXd> >* hessian) const override
	{
		check(false, to_string(typeid(*this).name(), ": hessian not implemented."));
		return 0;
	}

protected:
	Functor functor;
};

}  // namespace spii


#endif
