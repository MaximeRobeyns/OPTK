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
#include <stdexcept>

#include <optk/benchmark.hpp>
#include <sys/types.h>
#include <tests/testutils.hpp>

namespace syn {

/**
 * This enumerates the properties of the functions in this benchmark.
 */
enum class properties: char {
    continuous,
    differentiable,
    non_separable,
    scalable,
    non_scalable,
    multimodal
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
         * Sets the parameters which give the optimum when evaluated. Note that
         * this set needn't be unique; for instance in multimodal functions.
         * This set is not passed as an argument of this class' constructor to
         * improve the legibility of source code.
         * @param op The set of optimal parameter instances.
         */
        void set_opt_param (inst::set op) { opt_params = op; }
        /**
         * Returns the known optimal parameter set.
         */
        inst::set get_opt_param () { return opt_params; }

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
        u_int m_dims;
        double m_lb, m_ub, m_opt;
        std::vector<properties> m_properties;
        inst::set opt_params;
        sspace::sspace_t m_sspace;
};

/**
 * TODO return to this; provided with an optimisation algorithm, this will run
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
 * The global minima is located at the origin;
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$, with
 * \f$f(\mathbf{x^*}) = 0\f$.
 */
class ackley1: public synthetic {
    public:
        /** The constructor for the ackley1 function.
         * @param d The number of dimensions for this problem. */
        ackley1 (int d);
        double evaluate(inst::set x) override;
};

} // end namespace syn

#endif // __SYNTHETIC_H_
