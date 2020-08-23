/**
 * Copyright (C) 2020 Maxime Robeyns <maximerobeyns@gmail.com>
 *
 * Written for the ACRC, University of Bristol
 *
 * Licensed under the Educational Community License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.osedu.org/licenses/ECL-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the LIcense For The Specific Language Governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief In this file, we implement the ~175 synthetic test functions given in
 * Jamil et al. 2013 <https://arxiv.org/abs/1308.4008>
 */

#ifndef __SYNTHETIC_H_
#define __SYNTHETIC_H_

#include <cmath>
#include <cstring>
#include <stdexcept>
#include <bits/stdint-intn.h>

#include <optk/benchmark.hpp>
#include <optk/types.hpp>
#include <sys/types.h>

namespace syn {

/**
 * This enumerates the properties of the functions in this benchmark.
 */
enum class properties: char {
    continuous,
    differentiable,
    non_differentiable,
    separable,
    non_separable,
    scalable,
    non_scalable,
    multimodal,
    unimodal
};

/**
 * Owing to the similarity of the structure, and subsequent exposition, of the
 * 175 functions in Jamil et al. 2013, we use this as base class for the
 * synthetic benchmarks.
 */
class synthetic: public optk::benchmark {
    public:
        /**
         * Base constructor for synthetic benchmarks. Unless overidden by
         * derived calsses, this will expand the provided lower bound, upper
         * bound and dimensionality to a uniform bounded parameter space of
         * type sspace::sspace_t, making use of heap memory allocations.
         *
         * @param n The name of the benchmark.
         * @param dims The number of dimensions in which to run this benchmark.
         * @param lb The lower bound on the parameters constituting the search
         * space.
         * @param up The corresponding upper bound.
         * @param opt The global minimum for this function.
         * @param opt_param The optimal parameters for this function.
         */
        synthetic (
                const std::string &n,
                u_int dims,
                double lb,
                double ub,
                double opt);

        /**
         * A simpler base constructor for synthetic benchmarks which have more
         * complicated search spaces which will be manually set in the derived
         * class' constructor.
         * @param n The name of the benchmark
         * @param dims The number of dimensions of this benchmark
         * @param opt The global minimum
         */
        synthetic (const std::string &n, u_int dims, double opt);

        /**
         * This destructor frees the search space which was
         * automatically-generated in the constructor.
         */
        ~synthetic ();

        /**
         * @return The search space for this benchmark. This is passed to the
         * update_search_space method of optimisation algorithms.
         */
        sspace::sspace_t * get_search_space() { return &m_sspace; }

        /** This is specific to synthetic benchmarks, and sets the properties
         * of the derived benchmark function (e.g. non-separable, scalable
         * etc...)
         * @param p A vector of elements from the properties enum.
         */
        void set_properties (std::vector <properties> p) { m_properties = p; }
        std::vector <properties> get_properties() { return m_properties; }

        /**
         * Returns the known optimal parameter set.
         */
        inst::set get_opt_param () { return opt_params; }

        /**
         * Returns (one of) the known global minima.
         */
        double get_opt () { return m_opt; }

        /**
         * Update the value of the global minimum.
         */
        void update_opt (double opt) { m_opt = opt; }

        /** Get the number of dimensions that this problem was instantiated
         * with; or the fixed dimension that the derived function can take. */
        u_int get_dims () { return m_dims; }

        /**
         * This function validates that a set of parameter instances provided
         * to be evaluated is compatible with the search space.
         * @param x The search space provided by the optimisation algorithm.
         * @throws std::invalid_argument when the parameter set instance is
         * invalid.
         */
        void validate_param_set (inst::set x);

    protected:
        /**
         * Sets the parameters which give the optimum when evaluated. This set
         * needn't be unique; for instance in multimodal functions it is not.
         *
         * Only call once from the constructor of a derived class; multiple
         * invocations leads to memory leaks. It is 'protected' to guard
         * against this.
         * @param op The set of optimal parameter instances.
         */
        void set_opt_param (inst::set op);

        u_int m_dims;
        double m_lb, m_ub, m_opt;
        std::vector<properties> m_properties;
        inst::set opt_params;
        sspace::sspace_t m_sspace;
};

/**
 * TODO return to this;
 * provided with an optimisation algorithm, this will run
 * through all the synthetic benchmarks (perhaps in parallel) and evaluate the
 * performance of the optimisation algorithm on this (e.g. calculating traces,
 * AUC etc.)
 */
class synthetic_benchmark {
    public:
    private:
};

/**
 * The ackley1 function has formula
 * \f[
 *  f(x) = -20e^{0.02\sqrt{D^{-1}\sum^D_{i=1}x^2_i}}
 *  - e^{D^{-1}\sum^D_{i=1}\cos(2\pi x_i)} + 20 + e,
 * \f]
 * with \f$D\f$ being the number of dimensions of the problem.
 * The global minimum is located at the origin;
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$, with
 * \f$f(\mathbf{x^*}) = 0\f$. The bounds are subject to
 * \f$-35 \le x_i \le 35\f$.
 */
class ackley1: public synthetic {
    public:
        /** The constructor for the ackley1 function.
         * @param d The number of dimensions for this problem. */
        ackley1 (int d);
        double evaluate(inst::set x) override;
};

/**
 * The ackley2 function has formula
 * \f[
 * f(x) = -20 e^{-0.02\sqrt{x_1^2 + x_2^2),
 * \f]
 * with only two parameters \f$x_1\f$ and \$fx_2\$f.
 * The global minimum is located at the origin;
 * \f$\mathbf{x}^* = (0, 0)\f$, with
 * \f$f(\mathbf{x^*}) = -200\f$. The bounds are subject to
 * \f$-32 \le x_i \le 32\f$.
 */
class ackley2: public synthetic {
    public:
        ackley2 ();
        double evaluate(inst::set x) override;
};

/**
 * The ackley3 function has formula
 * \f[
 * f(x) = 200 e^{-0.02\sqrt{x_1^2 + x_2^2}} + 52^{\cos(3x_1) + \sin(3x_2)},
 * \f]
 * with only two parameters \f$x_1\f$ and \$fx_2\$f.
 * The global minimum is located at \f$\mathbf{x}^* = (0,0)\f$,
 * with \f$f(\mathbf{x^*}) = 5\f$. The bounds are subject to
 * \f$-32 \le x_i \le 32\f$.
 */
class ackley3: public synthetic {
    public:
        ackley3 ();
        double evaluate(inst::set x) override;
};

/**
 * The adjiman function has formula
 * \f[
 * f(x) = \cos(x_1)\sin(x_2) - \frac{x_1}{(x_2^2+1)},
 * \f]
 * with only two parameters \f$x_1\f$ and \$fx_2\$f, subject to
 * \f$-1 \le x_1 \le 2\f$ and \f$-1 \le x_2 \le 1\f$. The global minimum is
 * located on the boundary of the search space at
 * \f$\mathbf{x}^* = (2, 0.10578)\f$, with minimum
 * \$ff(\mathbf{x}^*) = -2.02181\f$.
 */
class adjiman: public synthetic {
    public:
        adjiman ();
        double evaluate(inst::set x) override;
};

/**
 * The alpine1 function has formula
 * \f[
 * f(x) = \sum^D_{i=1}\left\vert x_i\sin(x_i) + 0.1 x_i\right\vert,
 * \f]
 * with \f$D\f$ parameters subject to \f$-10 \le x_i \le 10\f$. The global
 * minimum is located at the origin; \f$\mathbf{x}^* = (0, \ldots, 0)\f$, with
 * value \$ff(\mathbf{x}^*) = 0\f$.
 */
class alpine1: public synthetic {
    public:
        alpine1 (int dims);
        double evaluate(inst::set x) override;
};

/**
 * The alpine2 function has formula
 * \f[
 * f(x) = \prod^D_{i=1}\sqrt{x_i}\sin(x_i),
 * \f]
 * with \f$D\f$ parameters subject to \f$0 \le x_i \le 10\f$. The global
 * minimum is located at \f$\mathbf{x}^* = (7.917, \ldots, 7.917)\f$, with
 * value \$ff(\mathbf{x}^*) = 2.808^D\f$.
 */
class alpine2: public synthetic {
    public:
        alpine2 (int dims);
        double evaluate(inst::set x) override;
};

/**
 * The brad function comes from 'A Comparison of Gradient Methods for the
 * Solution of Nonlinear Parameter Estimation Problems'
 * (https://doi.org/10.1137/0707011), p170 Problem 1.
 * It has formula
 * \f[
 * y(x) = \sum^15_{i=1}\left[
 * \frac{y_i - x_1 - i}
 * {(16-i)\cdot x_2 + \min(i, 16-i)\cdot x_3}
 * \right]^2,
 * \f]
 * where \f$y_i = [0.14, 0.18, 0.22, 0.25, 0.29, 0.32, 0.35, 0.39, 0.37, 0.58,
 * 0.73, 0.96, 1.34, 2.10, 4.39]^T\f$. The three parameters are subject to
 * \f$-0.25 \le x_1 \le 0.25\f$ as well as \f$0.01 \le x_2, x_3 \le 2.5\f$.
 * The global minimum is located at \f$\mathbf{x}^* = (0.08241040, 1.133033,
 * 2.343697)$, with \f$(\mathbf{x}^*) = TODO \f$.
 */
class brad: public synthetic {
    public:
        brad ();
        double evaluate(inst::set x) override;
};

} // end namespace syn

#endif // __SYNTHETIC_H_
