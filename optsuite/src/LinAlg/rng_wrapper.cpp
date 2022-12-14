/*
 * ==========================================================================
 *
 *       Filename:  rng_wrapper.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/06/2020 07:54:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Haoyang Liu (@liuhy), liuhaoyang@pku.edu.cn
 *   Organization:  BICMR, PKU
 *
 * ==========================================================================
 */

#include <numeric>
#include "OptSuite/core_n.h"
#include "OptSuite/LinAlg/rng_wrapper.h"

namespace OptSuite { namespace LinAlg {
    namespace {
        std::mt19937 generator{std::random_device{}()};
    }

    Mat randn(Size m, Size n, Scalar mean, Scalar std){
        std::normal_distribution<Scalar> dist(mean, std);
        auto normal = [&] () { return dist(generator); };
        Mat x = Mat::NullaryExpr(m, n, normal);
        return x;
    }

    Mat rand(Size m, Size n, Scalar lower, Scalar upper){
        std::uniform_real_distribution<Scalar> dist(lower, upper);
        auto unif_r = [&] () { return dist(generator); };
        Mat x = Mat::NullaryExpr(m, n, unif_r);
        return x;
    }

    Mat randi(Size m, Size n, Index upper){
        return randi(m, n, 0, upper - 1);
    }

    Mat randi(Size m, Size n, Index lower, Index upper){
        if (upper < lower) upper = lower;
        std::uniform_int_distribution<Index> dist(lower, upper);
        auto unif_i = [&] () { return static_cast<Scalar>(dist(generator)); };
        Mat x = Mat::NullaryExpr(m, n, unif_i);
        return x;
    }

    std::vector<Index> randperm(Size N, Size K){
        if (N < 0) N = 0;
        if (K == -1 || K > N) K = N;
        std::vector<Index> x(N);
        std::iota(x.begin(), x.end(), 0);

        for (Size i = 0; i < K; ++i){
            std::uniform_int_distribution<Size> dist(i, N - 1);
            Size t = dist(generator);
            std::swap(x[i], x[t]);
        }
        x.resize(K);
        return x;
    }

    SpMat sprandn(Size m, Size n, Scalar density){
        OPTSUITE_ASSERT(density > 0 && density < 1);
        Size nnz_max = Size(n * m * density);
        std::normal_distribution<Scalar> dist(0, 1);
        std::uniform_int_distribution<Index> dist_i(0, m - 1);
        std::uniform_int_distribution<Index> dist_j(0, n - 1);

        std::set<std::pair<Index, Index>> ij;

        for (int i = 0; i < nnz_max; ++i)
            ij.emplace(dist_i(generator), dist_j(generator));

        std::vector<Triplet<Scalar, Index>> t_list;
        t_list.reserve(nnz_max);

        for (auto it = ij.begin(); it != ij.end(); ++it){
            t_list.emplace_back(it->first, it->second, dist(generator));
        }

        SpMat mat(m, n);
        mat.setFromTriplets(t_list.begin(), t_list.end());

        return mat;
    }

    SpMat sprandn_c(Size m, Size n, Scalar density){
        OPTSUITE_ASSERT(density > 0 && density < 1);
        Size nnz_max_c = Size(m * density);
        Size nnz_max = nnz_max_c * n;
        std::normal_distribution<Scalar> dist(0, 1);
        std::uniform_int_distribution<Index> dist_i(0, m - 1);

        std::set<std::pair<Index, Index>> ij;

        for (int i = 0; i < nnz_max_c; ++i){
            Index ii = dist_i(generator);
            for (int j = 0; j < n; ++j)
                ij.emplace(ii, j);
        }

        std::vector<Triplet<Scalar, Index>> t_list;
        t_list.reserve(nnz_max);

        for (auto it = ij.begin(); it != ij.end(); ++it){
            t_list.emplace_back(it->first, it->second, dist(generator));
        }

        SpMat mat(m, n);
        mat.setFromTriplets(t_list.begin(), t_list.end());

        return mat;
    }

    std::mt19937& get_generator(){
        return generator;
    }

    void rng(unsigned long seed){
        generator.seed(seed);
    }
}}
