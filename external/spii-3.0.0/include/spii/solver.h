// Petter Strandmark 2012–2013.
#ifndef SPII_SOLVER_H
#define SPII_SOLVER_H
// The Solver class is a lightweight class defining settings
// for a solver.
//
// The member function Solver::solve minimizes a Function
// using the settings in the Solver.
//

#include <functional>
#include <iostream>
#include <string>

#include <spii/spii.h>
#include <spii/function.h>

namespace spii {

// SolverResults contains the result of a call to Solver::solve.
struct SPII_API SolverResults
{

	// The exit condition specifies how the solver terminated.
	enum {GRADIENT_TOLERANCE, // Gradient tolerance reached.
	      FUNCTION_TOLERANCE, // Function improvement tolerance reached.
	      ARGUMENT_TOLERANCE, // Argument improvement tolerance reached.
	      NO_CONVERGENCE,     // Maximum number of iterations reached.
	      FUNCTION_NAN,       // Nan encountered.
	      FUNCTION_INFINITY,  // Infinity encountered.
		  USER_ABORT,         // The callback function aborted the optimization.
	      INTERNAL_ERROR,     // Internal error.
	      NA} exit_condition = NA;

	// Returns true if the exit_condition indicates convergence.
	bool exit_success() const
	{
		return exit_condition == GRADIENT_TOLERANCE ||
		       exit_condition == FUNCTION_TOLERANCE ||
		       exit_condition == ARGUMENT_TOLERANCE;
	}

	double startup_time                = 0;
	double function_evaluation_time    = 0;
	double stopping_criteria_time      = 0;
	double matrix_factorization_time   = 0;
	double lbfgs_update_time           = 0;
	double linear_solver_time          = 0;
	double backtracking_time           = 0;
	double log_time                    = 0;
	double total_time                  = 0;

	// The minimum value of the function being minimized is
	// in this interval. These members are only set by global
	// optmization solvers.
	double optimum_lower = - std::numeric_limits<double>::infinity();
	double optimum_upper =   std::numeric_limits<double>::infinity();
};

SPII_API std::ostream& operator<<(std::ostream& out, const SolverResults& results);

// Holds information provided by the solver via a
// callback function. 
//
// Note: All pointers in the struct may be
//       nullptr, depending on the solver.
struct CallbackInformation
{
	double objective_value = std::numeric_limits<double>::quiet_NaN();
	// The currently evaluated point.
	const Eigen::VectorXd* x        = nullptr;
	// The gradient at x.
	const Eigen::VectorXd* g        = nullptr;
	// The dense Hessian at x.
	const Eigen::MatrixXd* H_dense  = nullptr;
	// The sparse Hessian at x.
	const Eigen::SparseMatrix<double>* H_sparse = nullptr; 
};


// Used to call Solver::BKP_dense.
class SPII_API FactorizationCache;

// Used to call Solver::check_exit_conditions.
struct CheckExitConditionsCache;

#ifdef _WIN32
	SPII_API_EXTERN_TEMPLATE template class SPII_API std::function<void(const std::string&)>;
	SPII_API_EXTERN_TEMPLATE template class SPII_API std::function<bool(const CallbackInformation&)>;
#endif

class SPII_API Solver
{
public:
	Solver();
	virtual ~Solver();
	Solver(const Solver&) = default;

	virtual void solve(const Function& function, SolverResults* results) const = 0;

	// Function called every time the solver emits a log message.
	// Default: print to std::cerr.
	std::function<void(const std::string& log_message)> log_function;

	// Function called each iteration.
	// If the function returns false, the solver should abort the optimization.
	// Default: none.
	std::function<bool(const CallbackInformation& information)> callback_function;

	// Maximum number of iterations.
	int maximum_iterations = 100;

	// Gradient tolerance. The solver terminates if
	// ||g|| / ||g0|| < tol, where ||.|| is the maximum
	// norm.
	double gradient_tolerance = 1e-12;

	// Function improvement tolerance. The solver terminates
	// if |df| / (|f| + tol) < tol.
	double function_improvement_tolerance = 1e-12;

	// Argument improvement tolerance. The solver terminates
	// if ||dx|| / (||x|| + tol) < tol.
	double argument_improvement_tolerance = 1e-12;

	enum {ARMIJO, WOLFE} line_search_type = ARMIJO;

	// Interpolation type only affects Wolfe line search.
	enum {BISECTION, CUBIC}
		wolfe_interpolation_strategy = CUBIC;

	// The Armijo line search is completed when
	//
	//   f(x + alpha * p) <= f(x) + c * alpha * gTp.
	//
	// This condition is also part of Wolfe line search.
	double line_search_c   = 1e-2;

	// In each Armijo iteration, alpha *= rho.
	double line_search_rho = 0.5;

	// The Wolfe conditions are
	//
	// |g_newTp| <= -c2 * gTp
	//
	double line_search_c2 = 0.9;

protected:

	// Computes a Newton step given a function, a gradient and a
	// Hessian.
	//
	// Stores the used cache in the cache
	bool check_exit_conditions(const double fval,
	                           const double fprev,
	                           const double gnorm,
	                           const double gnorm0,
	                           const double xnorm,
	                           const double dxnorm,
	                           const bool last_iteration_successful,
	                           CheckExitConditionsCache* cache,
	                           SolverResults* results) const;

	// Performs a line search from x along direction p. Returns
	// alpha, the multiple of p to get to the new point.
	double perform_linesearch(const Function& function,
	                          const Eigen::VectorXd& x,
	                          const double fval,
	                          const Eigen::VectorXd& g,
	                          const Eigen::VectorXd& p,
	                          Eigen::VectorXd* scratch,
	                          const double start_alpha = 1.0) const;

	// Performs a BKP block diagonal factorization, modifies it, and
	// solvers the linear system. Uses the Meschach library.
	void BKP_dense(const Eigen::MatrixXd& H,
	               const Eigen::VectorXd& g,
	               const FactorizationCache& cache,
	               Eigen::VectorXd* p,
	               SolverResults* results) const;

	// Performs a BKP block diagonal factorization, modifies it, and
	// solvers the linear system. Uses sym-ildl.
	void BKP_sym_ildl(const Eigen::MatrixXd& H,
	                  const Eigen::VectorXd& g,
	                  Eigen::VectorXd* p,
	                        SolverResults* results) const;
	void BKP_sym_ildl(const Eigen::SparseMatrix<double>& H,
	                  const Eigen::VectorXd& g,
	                  Eigen::VectorXd* p,
	                  SolverResults* results) const;
};

// Newton's method. It requires first and
// second-order derivatives. Generally converges
// quickly. It is slow and requires a lot of
// memory if the Hessian is dense.
class SPII_API NewtonSolver
	: public Solver
{
public:
	// Mode of operation. How the Hessian is stored.
	// Default: AUTO.
	enum class SparsityMode {DENSE, SPARSE, AUTO};
	SparsityMode sparsity_mode = SparsityMode::AUTO;

	// The default factorization method is the BKP block
	// diagonal modification (Nocedal and Wright, p. 55).
	// Alternatively, it is possible to use iterative diagonal
	// modification of the Hessian. This is also used for
	// sparse systems.
	enum class FactorizationMethod
	{
		ITERATIVE, // Iterative diagonal modification (dense + sparse). Production-ready.
		MESCHACH,  // BKP using the Meschach library (dense only). Production-ready.
		           // Will fall back to ’iterative’ for sparse problems.
		SYM_ILDL,  // BKP using the sym-ildl library.
	};
	FactorizationMethod factorization_method = FactorizationMethod::MESCHACH;

	virtual void solve(const Function& function, SolverResults* results) const override;
};

// L-BFGS. Requires only first-order derivatives
// and generally converges quickly. Always uses
// relatively little memory.
class SPII_API LBFGSSolver
	: public Solver
{
public:
	// Number of vectors L-BFGS should save in its history.
	int lbfgs_history_size = 10;

	// If the relative function improvement is less than this
	// value, L-BFGS will discard its history and restart.
	double lbfgs_restart_tolerance = 1e-6;

	virtual void solve(const Function& function, SolverResults* results) const override;
};

// Nelder-Mead requires no derivatives. It generally
// produces slightly more inaccurate solutions in many
// more iterations.
class SPII_API NelderMeadSolver
	: public Solver
{
public:
	// Area tolerance. The solver terminates if
	// ||a|| / ||a0|| < tol, where ||.|| is the maximum
	// norm.
	double area_tolerance = 1e-12;

	// Length tolerance. The solver terminates if
	// ||a|| / ||a0|| < tol, where ||.|| is the maximum
	// norm.
	double length_tolerance = 1e-12;

	virtual void solve(const Function& function, SolverResults* results) const override;
};

// For most problems, there is no reason to choose
// pattern search over Nelder-Mead.
class SPII_API PatternSolver
	: public Solver
{
public:
	// Area tolerance. The solver terminates if
	// ||a|| / ||a0|| < tol, where ||.|| is the maximum
	// norm.
	double area_tolerance = 1e-12;

	virtual void solve(const Function& function, SolverResults* results) const override;
};

// (Experimental) Global optimization using interval
// arithmetic.
class SPII_API GlobalSolver
	: public Solver
{
public:
	void solve_global(const Function& function,
	                  const IntervalVector& start_box,
	                  SolverResults* results) const;
	
	// Does not do anything. The global solver requires the
	// extended interface above.
	virtual void solve(const Function& function, SolverResults* results) const override;
};





// Definitions of helper classes.

struct FactorizationCacheInternal;
class SPII_API FactorizationCache
{
public:
	FactorizationCache(int n);
	~FactorizationCache();
	FactorizationCacheInternal* data;
};


struct CheckExitConditionsCache
{
public:
	CheckExitConditionsCache()
	{
		for (auto& ng: normg_history) {
			ng = 0.0;
		}
	}

	static const int amount_history_to_consider = 6;
	int norm_g_history_pos = 0;
	double normg_history[amount_history_to_consider];
};


}  // namespace spii

#endif
