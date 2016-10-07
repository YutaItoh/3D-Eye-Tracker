// Petter Strandmark 2013.
#ifndef SPII_SYM_ILDL_CONVERSIONS_H
#define SPII_SYM_ILDL_CONVERSIONS_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseQR>
#include <Eigen/SparseLU>

#include <lilc_matrix.h>

#include <spii/spii.h>

namespace spii
{

void eigen_to_lilc(const Eigen::MatrixXd& A, lilc_matrix<double>* Alilc_input)
{
	spii_assert(Alilc_input);
	lilc_matrix<double>& Alilc = *Alilc_input;

	int m = static_cast<int>(A.rows());
	int n = static_cast<int>(A.cols());
	spii_assert(m == n);

	int count = 0;
	Alilc.resize(n, n);
	fill(Alilc.first.begin(), Alilc.first.end(), 0);
	for (int i = 0; i < n; i++) {

		for (int j = 0; j < n; j++) {

			if (j < i) {
				continue;
			}

			Alilc.m_idx[i].push_back(j);
			Alilc.m_x[i].push_back(A(i, j));
			if (i != j) 
				Alilc.list[j].push_back(i);
			count++;
		}
	}
	Alilc.nnz_count = count;
}

void eigen_to_lilc(const Eigen::SparseMatrix<double>& A, lilc_matrix<double>* Alilc_input)
{
	spii_assert(Alilc_input);
	lilc_matrix<double>& Alilc = *Alilc_input;

	auto m = A.rows();
	auto n = A.cols();
	spii_assert(m == n);

	int count = 0;
	Alilc.resize(n, n);
	fill(Alilc.first.begin(), Alilc.first.end(), 0);

	for (int k = 0; k < A.outerSize(); ++k) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
			auto i = it.row();
			auto j = it.col(); // (here it is equal to k)
			if (j < i) {
				continue;
			}

			Alilc.m_idx[i].push_back(j);
			Alilc.m_x[i].push_back(it.value());
			if (i != j) {
				Alilc.list[j].push_back(i);
			}
			count++;
		}
	}

	Alilc.nnz_count = count;
}

Eigen::MatrixXd lilc_to_eigen(const lilc_matrix<double>& Alilc, bool symmetric=false)
{
	Eigen::MatrixXd A{Alilc.n_rows(), Alilc.n_cols()};
	A.fill(0.0);

	for (int i = 0; i < Alilc.n_rows(); i++) {
		for (std::size_t ind = 0; ind < Alilc.m_idx.at(i).size(); ind++) {
			auto j = Alilc.m_idx.at(i)[ind];
			auto value = Alilc.m_x.at(i).at(ind);
			if (symmetric) {
				A(i, j) = value;
			}
			A(j, i) = value;
		}
	}
	return std::move(A);
}

void lilc_to_eigen(const lilc_matrix<double>& Alilc, Eigen::SparseMatrix<double>* A, bool symmetric=false)
{
	A->resize(Alilc.n_rows(), Alilc.n_cols());
	std::vector<Eigen::Triplet<double>> triplets;

	for (int i = 0; i < Alilc.n_rows(); i++) {
		for (std::size_t ind = 0; ind < Alilc.m_idx.at(i).size(); ind++) {
			auto j = Alilc.m_idx.at(i)[ind];
			auto value = Alilc.m_x.at(i).at(ind);
			if (symmetric && i != j) {
				triplets.emplace_back(i, j, value);
			}
			triplets.emplace_back(j, i, value);
		}
	}

	A->setFromTriplets(begin(triplets), end(triplets));
	A->makeCompressed();
}

Eigen::DiagonalMatrix<double, Eigen::Dynamic> diag_to_eigen(const block_diag_matrix<double>& Ablock)
{
	spii_assert(Ablock.n_rows() == Ablock.n_cols());
	spii_assert(Ablock.off_diag.empty());

	Eigen::DiagonalMatrix<double, Eigen::Dynamic> A{Ablock.n_rows()};
	A.setZero();

	for (std::size_t i = 0; i < Ablock.main_diag.size(); ++i) {
		A.diagonal()[i] = Ablock.main_diag[i];
	}

	return std::move(A);
}

Eigen::MatrixXd block_diag_to_eigen(const block_diag_matrix<double>& Ablock)
{
	Eigen::MatrixXd A{Ablock.n_rows(), Ablock.n_cols()};
	A.fill(0.0);

	for (std::size_t i = 0; i < Ablock.main_diag.size(); ++i) {
		A(i, i) = Ablock.main_diag[i];
	}

	for (const auto& i_and_value: Ablock.off_diag) {
		auto i = i_and_value.first;
		auto value = i_and_value.second;
		A(i+1, i) = value;
		A(i, i+1) = value;
	}

	return std::move(A);
}

void block_diag_to_eigen(const block_diag_matrix<double>& Ablock, Eigen::SparseMatrix<double>* A)
{
	A->resize(Ablock.n_rows(), Ablock.n_cols());
	std::vector<Eigen::Triplet<double>> triplets;

	for (std::size_t i = 0; i < Ablock.main_diag.size(); ++i) {
		triplets.emplace_back(i, i, Ablock.main_diag[i]);
	}

	for (const auto& i_and_value: Ablock.off_diag) {
		auto i = i_and_value.first;
		auto value = i_and_value.second;
		triplets.emplace_back(i    , i + 1, value);
		triplets.emplace_back(i + 1, i    , value);
	}

	A->setFromTriplets(begin(triplets), end(triplets));
	A->makeCompressed();
}

class MyPermutation
	: public Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic, int>
{
public:
	MyPermutation(const std::vector<int>& perm)
		: Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic, int>(static_cast<int>(perm.size()))
	{
		for (std::size_t i = 0; i < perm.size(); ++i) {
			m_indices(i) = perm[i];
		}
	}
};


// Solve B*x = b, where B is block-diagonal.
void solve_block_diag(block_diag_matrix<double>& B,
                      Eigen::VectorXd* x)
{
	using namespace Eigen;

	auto n = B.n_rows();
	spii_assert(B.n_cols() == n);

	bool onebyone;
	for (int i = 0; i < n; i = (onebyone ? i+1 : i+2) ) {
		onebyone = (i == n-1 || B.block_size(i) == 1);

		if ( onebyone ) {
			(*x)(i) /= B[i];
		}
		else {
			Matrix2d Bblock;
			Bblock(0, 0) = B[i];
			Bblock(0, 1) = B.off_diagonal(i);
			Bblock(1, 0) = B.off_diagonal(i);
			Bblock(1, 1) = B[i+1];
			spii_assert(Bblock(1, 0) == Bblock(0, 1));

			Vector2d x_copy;
			x_copy(0) = (*x)[i];
			x_copy(1) = (*x)[i+1];
			x_copy = Bblock.lu().solve(x_copy);
			(*x)[i]   = x_copy(0);
			(*x)[i+1] = x_copy(1);
		}
	}
}

// Solve L*x = b, where L is lower-triangular.
void solve_lower_triangular(const lilc_matrix<double>& Llilc,
                            Eigen::VectorXd* x)
{
	//Eigen::SparseMatrix<double> L;
	//lilc_to_eigen(Llilc, &L, false);
	//*x = L.triangularView<Eigen::Lower>().solve(*x);

	auto n = Llilc.n_cols();
	spii_assert(Llilc.n_rows() == n);

	for (int j = 0; j < n; ++j) {
		auto n_elements = Llilc.m_idx[j].size();
		spii_assert(Llilc.m_x[j].size() == n_elements);

		for (std::size_t k = 0; k < n_elements; ++k) {
			auto i     = Llilc.m_idx[j][k];
			auto value = Llilc.m_x[j][k];

			if (i == j) {
				(*x)[j] /= value;
			}
			else {
				(*x)[i] -= value * (*x)[j];
			}
		}
	}
}

// Solve L^T * x = b, where L is lower-triangular.
void solve_lower_triangular_transpose(const lilc_matrix<double>& Llilc,
                                      Eigen::VectorXd* x)
{
	Eigen::SparseMatrix<double> L;
	lilc_to_eigen(Llilc, &L, true);
	*x = L.triangularView<Eigen::Upper>().solve(*x);

	//auto n = Llilc.n_cols();
	//spii_assert(Llilc.n_rows() == n);

	////
	//// First transpose the matrix.
	////
	//std::vector<std::vector<int>>    m_idx(n);
	//std::vector<std::vector<double>> m_x(n);

	//for (int j = 0; j < n; ++j) {
	//	auto n_elements = Llilc.m_idx[j].size();
	//	spii_assert(Llilc.m_x[j].size() == n_elements);

	//	for (std::size_t k = 0; k < n_elements; ++k) {
	//		auto i = Llilc.m_idx[j][k];
	//		auto value = Llilc.m_x[j][k];
	//		m_idx[i].push_back(j);
	//		m_x[i].push_back(value);
	//	}
	//}

	//for (auto& vec: m_idx) {
	//	std::sort(begin(vec), end(vec));
	//}

	////
	//// Then solve.
	////
	//for (int j = n - 1; j >= 0; --j) {
	//	auto n_elements = m_idx[j].size();
	//	spii_assert(m_x[j].size() == n_elements);

	//	for (std::size_t k = 0; k < n_elements; ++k) {
	//		auto i     = m_idx[j][k];
	//		auto value = m_x[j][k];

	//		if (i == j) {
	//			(*x)[j] /= value;
	//		}
	//		else {
	//			(*x)[i] -= value * (*x)[j];
	//		}
	//	}
	//}
}

// Solve A*x = b, where
//
// A = S.inverse() * (P * L * B * L.transpose() * P.transpose()) * S.inverse()
//
void solve_system_ildl(block_diag_matrix<double>& B,
                       const lilc_matrix<double>& Llilc,
                       const Eigen::DiagonalMatrix<double, Eigen::Dynamic>& S,
                       const MyPermutation& P,
                       const Eigen::VectorXd& lhs,
                       Eigen::VectorXd* x_output)
{
	spii_assert(x_output);
	auto& x = *x_output;
	auto n = lhs.rows();
	spii_assert(B.n_rows() == n);
	spii_assert(B.n_cols() == n);
	spii_assert(Llilc.n_rows() == n);
	spii_assert(Llilc.n_cols() == n);
	spii_assert(S.rows() == n);
	spii_assert(S.cols() == n);

	x = S * lhs;
	x = P.transpose() * x;
	solve_lower_triangular(Llilc, &x);
	solve_block_diag(B, &x);
	solve_lower_triangular_transpose(Llilc, &x);
	x = P * x;
	x = S * x;
}

}

#endif
