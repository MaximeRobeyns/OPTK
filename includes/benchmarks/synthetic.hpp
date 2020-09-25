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
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/types.h>
#include <bits/stdint-intn.h>

#include <optk/core.hpp>
#include <optk/types.hpp>
#include <optk/benchmark.hpp>

namespace syn {

/**
 * This enumerates the properties of the functions in this benchmark.
 */
enum class properties: char {
    continuous,
    discontinuous,
    differentiable,
    non_differentiable,
    separable,
    partially_separable,
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
 * This unifies the synthetic benchmarks, and makes them all callable under one
 * method.
 */
class synthetic_benchmark: public optk::benchmark_set {
    public:
        synthetic_benchmark ();

        ~synthetic_benchmark () {}

        /**
         * provided with an optimisation algorithm, this will run
         * through all the synthetic benchmarks (perhaps in parallel) and evaluate the
         * performance of the optimisation algorithm on this (e.g. calculating traces,
         * AUC etc.)
         */
        void run (optk::optimisers *opts, optk::ctx_t *ctx) override;
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
 * f(x) = 200 e^{-0.02\sqrt{x_1^2 + x_2^2}} + 5e^{\cos(3x_1) + \sin(3x_2)},
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
 * (https://doi.org/10.1137/0707011), and is a modified version of Problem 1,
 * p170. We give it the formula
 * \f[
 * f(x) = \sum^15_{i=1}\left[
 * \frac{y_i - x_1 - i}
 * {(16-i)\cdot x_2 + \min(i, 16-i)\cdot x_3}
 * \right]^2,
 * \f]
 * where \f$y_i = [0.14, 0.18, 0.22, 0.25, 0.29, 0.32, 0.35, 0.39, 0.37, 0.58,
 * 0.73, 0.96, 1.34, 2.10, 4.39]^T\f$. The three parameters are subject to
 * \f$-0.25 \le x_1 \le 0.25\f$ as well as \f$0.01 \le x_2, x_3 \le 2.5\f$.
 * The global minimum is located at \f$\mathbf{x}^* = (0.08241040, 1.133033,
 * 2.343697)$, with \f$(\mathbf{x}^*) = 0.00821488 \f$.
 */
class brad: public synthetic {
    public:
        brad ();
        double evaluate(inst::set x) override;
};

/**
 * The bartels conn function has the formula
 * \f[
 * f(x) = \vert x_1^2 + x_2^2 + x_1x_2\vert +
 * \vert \sin(x_1)\vert + \vert \cos(x_2)\vert,
 * \f]
 * subject to \f$-500 \le x_i \le 500\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (0,0)\f$, with \f$f(\mathbf{x}^*) = 1\f$.
 */
class bartels_conn: public synthetic {
    public:
        bartels_conn ();
        double evaluate(inst::set x) override;
};

/**
 * The beale conn function has the formula
 * \f[
 * \begin{align*}
 * f(x) = &\bigg(1.5 - x_1 + x_1x_2\bigg)^2 + \bigg(2.25 - x_1 + x_1x_2^2\bigg)^2 \\
 * &+\bigg(2.625 - x_1 + x_1x_2^2\bigg)^2,
 * \end{align*}
 * \f]
 * subject to \f$-4.5 \le x_i \le 4.5\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (3,0.5)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class beale: public synthetic {
    public:
        beale ();
        double evaluate(inst::set x) override;
};

/**
 * The biggs_exp2 function has the formula
 * \f[
 * y(x) = \sum^{10}_{i=1} \left(e^{-0.1i \cdot x_1} - 5e^{-0.1i \cdot x_2}
 * - e^{-0.1i} + 5e^{-i}
 * \right)^2,
 * \f]
 * subject to \f$0 \le x_i \le 20\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1,10)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class biggs_exp2: public synthetic {
    public:
        biggs_exp2 ();
        double evaluate(inst::set x) override;
};

/**
 * The biggs_exp3 function is a three-dimensional variation on the previous
 * one, and has the formula
 * \f[
 * y(x) = \sum^{10}_{i=1} \left(e^{-0.1i \cdot x_1} - x_3e^{-0.1i \cdot x_2}
 * - e^{-0.1i} + 5e^{-i}
 * \right)^2,
 * \f]
 * subject to \f$0 \le x_i \le 20\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1,10, 5)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class biggs_exp3: public synthetic {
    public:
        biggs_exp3 ();
        double evaluate(inst::set x) override;
};

/**
 * The biggs_exp4 function adds another parameter to the previous version to
 * make it a 4-dimensional problem:
 * \f[
 * y(x) = \sum^{10}_{i=1} \left(x_3 e^{-0.1i \cdot x_1} - x_4e^{-0.1i \cdot x_2}
 * - e^{-0.1i} + 5e^{-i}
 * \right)^2,
 * \f]
 * subject to \f$0 \le x_i \le 20\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1,10, 1, 5)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class biggs_exp4: public synthetic {
    public:
        biggs_exp4 ();
        double evaluate(inst::set x) override;
};

/**
 * The biggs_exp5 function is a five dimensional problem with the following formula:
 * \f[
 * y(x) = \sum^{11}_{i=1} \left(x_3 e^{-0.1i \cdot x_1} - x_4e^{-0.1i \cdot x_2}
 * + 3 e^{-0.1 i \cdot x_5} e^{-0.1i} + 5e^{-i} - 3 e^{-0.4i}
 * \right)^2,
 * \f]
 * subject to \f$0 \le x_i \le 20\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1,10, 1, 5, 4)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class biggs_exp5: public synthetic {
    public:
        biggs_exp5 ();
        double evaluate(inst::set x) override;
};

/**
 * The biggs_exp6 function is the last of the series, with 6 parameters:
 * \f[
 * y(x) = \sum^{11}_{i=1} \left(x_3 e^{-0.1i \cdot x_1} - x_4e^{-0.1i \cdot x_2}
 * + x6 e^{-0.1 i \cdot x_5} e^{-0.1i} + 5e^{-i} - 3 e^{-0.4i}
 * \right)^2,
 * \f]
 * subject to \f$-20 \le x_i \le 20\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1,10, 1, 5, 4, 3)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class biggs_exp6: public synthetic {
    public:
        biggs_exp6 ();
        double evaluate (inst::set x) override;
};

/**
 * The bird function has the following formula:
 * \f[
 * f(X) = \sin(x_1)e^{\bigg(1-\cos(x_2)\bigg)^2} +
 * \cos(x_2)e^{(1-\sin(x_1))^2} +
 * (x_1 - x_2)^2,
 * \f]
 * subject to the bounds \f$-2\pi \le x_i \le 2\pi\f$, with the global minimum
 * located at either \f$\mathbf{x}^* = (4.70104,
 * 3.15294),(-1.58214,-3.13024)\f$, with a value of \f$f(\mathbf{x}^*) =
 * -106.764537\f$.
 */
class bird: public synthetic {
    public:
        bird ();
        double evaluate (inst::set x) override;
};

/**
 * This is the first in the series of Bohachevsky function benchmarks. This one
 * has formula
 * \f[
 * f(X) = x_1^2 + 2x_2^2 - 0.2\cos(3\pi x_1) - 0.4\cos(4\pi x_2) + 0.7,
 * \f]
 * subject to the bounds \f$-100 \le x_i \le 100\f$, with the global minimum
 * located at \f$\mathbf{x}^* = (0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class bohachevsky1: public synthetic {
    public:
        bohachevsky1 ();
        double evaluate (inst::set x) override;
};

/**
 * This non-separable variation on the first Bohachevsky function has the
 * formula:
 * \f[
 * f(X) = x_1^2 + 2x_2^2 - 0.3\cos(3\pi x_1) \cdot 0.4\cos(4\pi x_2) + 0.3,
 * \f]
 * subject to the bounds \f$-100 \le x_i \le 100\f$, with the global minimum
 * located at \f$\mathbf{x}^* = (0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class bohachevsky2: public synthetic {
    public:
        bohachevsky2 ();
        double evaluate (inst::set x) override;
};

/**
 * The final variation on the Bohachevsky function has the following formula:
 * \f[
 * f(X) = x_1^2 + 2x_2^2 - 0.3\cos(3\pi x_1 + 4\pi x2) + 0.3,
 * \f]
 * subject to the bounds \f$-100 \le x_i \le 100\f$, with the global minimum
 * located at \f$\mathbf{x}^* = (0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class bohachevsky3: public synthetic {
    public:
        bohachevsky3 ();
        double evaluate (inst::set x) override;
};

/**
 * The Booth function has the formula:
 * \f[
 * f(\mathbf{x}) = (x_! + 2x_2 - 7)^2 + (2x_1 + x_2 - 5)^2,
 * \f]
 * subject to the bounds \f$-10 \le x_i \le 10\f$, with the global minimum
 * located at \f$\mathbf{x}^* = (1,3)\f$, with \f$f(\mathbf{x}^*) = 0\f$.
 */
class booth: public synthetic {
    public:
        booth ();
        double evaluate (inst::set x) override;
};

/**
 * The Box-Betts Quadratic Sum function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{D-1}_{i=0} g(x_i)^2,
 * \f]
 * where
 * \f[
 * g(x_i) = e^{-0.1(i+1)x_1} - e^{-0.1(i+1)x_2} -
 * e^{\left[(-0.1(i+1))-e^{-(i+1)}\right]x_3},
 * \f]
 * subject to \f$0.9 \le x_1 \le 1.2\f$, \f$9 \le x_2 \le 11.2\f$ and
 * \f$0.9 \le x_3 \le 1.2\f$. The global minimum is located at
 * \f$\mathbf{x}^*(1, 10, 1)\f$ where \f$f(\mathbf{x}^*) = 0\f$.
 */
class box_betts: public synthetic {
    public:
        box_betts ();
        double evaluate (inst::set x) override;
};

/**
 * The Branin RCOS function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left(x_2 - \frac{5.1x_1^2}{4\pi^2} + \frac{5x_1}{\pi}
 * - 6\right)^2 + 10 \left(1 - \frac{1}{8\pi}\right)\cos(x_1) + 10.
 * \f]
 * The parameters are in the domain \f$-5 \le \le 10\f$ and \f$0 \le x_2 \le
 * 15\f$, and it has three global minima at \f$\mathbf{x}^* = \{(-\pi, 12.275),
 * (\pi, 2.275), (3\pi, 2.425)\}\f$, with value \f$f(\mathbf{x}^*) =
 * 0.3978873\f$.
 */
class branin1: public synthetic {
    public:
        branin1 ();
        double evaluate (inst::set x) override;
};

/**
 * The Branin RCOS 2 function has the following formula
 * \f[
 * f(\mathbf{x}) = \left(x_2 - \frac{5.1x_1^2}{4\pi^2} + \frac{5x_1}{\pi} -
 * 6\right)^2 + 10\left(1 - \frac{1}{8\pi}\right)\cos(x_1)\cos(x_2)
 * \ln (x_1^2 + x_2^2 + 1) + 10.
 * \f]
 * The domain is \f$-5 \le x_i \le 15\f$ for both variables, and the global
 * minimum is located at \f$\mathbf{x}^* = (-3.2, 12.53)\f$ with value
 * \f$f(\mathbf{x}^*) = 5.559037\f$.
 */
class branin2: public synthetic {
    public:
        branin2 ();
        double evaluate (inst::set x) override;
};

/**
 * The Brent function has the following formula
 * \f[
 * f(\mathbf{x}) = (x_1 + 10)^2 + (x_2 + 10)^2 + e^{-x_1^2-x_2^2},
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (-10,-10)\f$ with value \f$f(\mathbf{x}^* = 0\f$.
 */
class brent: public synthetic {
    public:
        brent ();
        double evaluate (inst::set x) override;
};

/**
 * The Brown funcion has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{n-1}_{i=1}(x_i^2)^{x^2_{i+1}+1} +
 * (x_{i+1}^2)^{x_i^2+1},
 * \f]
 * subject to \f$-1 \le x_i \le 4\f$. The global minimum is located at
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$, with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class brown: public synthetic {
    public:
        brown (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The bukin 2 function has the formula:
 * \f[
 * f(\mathbf{x}) = 100 (x_2 - 0.01x_1^2 + 1) + 0.01(x_1+10)^2,
 * \f]
 * subject to \f$-15 \le x_1 \le -5\f$ and \f$-3 \le x_2 \le 3\f$. The global
 * minimum is located at \f$\mathbf{x}^* = (-10, 0)\f$ with value
 * \f$f(\mathbf{x}^*) = 0\f$.
 */
class bukin2: public synthetic {
    public:
        bukin2 ();
        double evaluate (inst::set x) override;
};

/**
 * The bukin 4 function has the formula:
 * \f[
 * f(\mathbf{x}) = 100 x_2^2 + 0.01 \vert x_1 + 10 \vert,
 * \f]
 * subject to \f$-15 \le x_1 \le -5\f$ and \f$-3 \le x_2 \le 3\f$. The global
 * minimum is located at \f$\mathbf{x}^* = (-10, 0)\f$ with value
 * \f$f(\mathbf{x}^*) = 0\f$.
 */
class bukin4: public synthetic {
    public:
        bukin4 ();
        double evaluate (inst::set x) override;
};

/**
 * The bukin 6 function has the formula:
 * \f[
 * f(\mathbf{x}) =  100 \sqrt{\vert x_2 -0.01x_2^2\vert} +
 * 0.01\vert x_1 + 10 \vert,
 * \f]
 * subject to \f$-15 \le x_1 \le -5\f$ and \f$-3 \le x_2 \le 3\f$. The global
 * minimum is located at \f$\mathbf{x}^* = (-10, 0)\f$ with value
 * \f$f(\mathbf{x}^*) = 0\f$.
 */
class bukin6: public synthetic {
    public:
        bukin6 ();
        double evaluate (inst::set x) override;
};

/**
 * The three-hump camel function has the formula:
 * \f[
 * f(\mathbf{x}) = 2x_1^2 - 1.05x_1^4 + x_1^6/6 + x_1x_2 + x_2^2,
 * \f]
 * subject to \f$-5 \le x_i \le 5\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f%.
 */
class camel3: public synthetic {
    public:
        camel3 ();
        double evaluate (inst::set x) override;
};

/**
 * The six-hump camel function has the formula:
 * \f[
 * f(\mathbf{x}) = \bigg(4 - 2.1 x_1^2 + \frac{x_1^4}{3}\bigg)x_1^2 +
 * x_1x_2 + (4x_2^2 - 4)x_2^2,
 * \f]
 * subject to \f$-5 \le x_i \le 5\f$, with two global minimum located at
 * \f$\mathbf{x}^* = \{(0.08984201368301331, -0.7126564032704135),
 * (-0.08984201368301331, 0.7126564032704135)\}\f$ with value
 * \f$f(\mathbf{x}^*) = -1.031628\f%.
 */
class camel6: public synthetic {
    public:
        camel6 ();
        double evaluate (inst::set x) override;
};

// We omit the chen* functions on the basis that they are difficult to
// calculate leading to floating-point errors. When we are given control over
// the experimental design, we can constrain our optimisations to values which
// are easily represented by floating-point values; henc ethese benchmarks can
// be considered somewhat unrealistic.

/**
 * The Chichinadze function has the following formula
 * \f[
 * f(\mathbf{x}) = x_1^2 - 12x_1 + 11 + 10\cos(\pi x_1/2) + 8\sin(5\pi x_1/2) -
 * (1/5)^{1/2}\exp\bigg(-0.5(x_2 - 0.5)^2\bigg),
 * \f]
 * subject to \f$-30 \le x_i \le 30\f$, with two global minimum located at
 * \f$\mathbf{x}^* = (6.189866586965680, 0.5)\f$ with value
 * \f$f(\mathbf{x}^*) = -42.94438701899098\f%.
 */
class chichinadze: public synthetic {
    public:
        chichinadze ();
        double evaluate (inst::set x) override;
};

/**
 * The Chung Reynolds functions has the following formula:
 * \f[
 * f(\mathbf{x}) = \left(\sum^D_{i=1}x_i^2\right)^2,
 * \f]
 * subject to \f$-100 \le x_i \le 100\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class chung_reynolds: public synthetic {
    public:
        chung_reynolds (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Cola function is a 17-dimensional function given by
 * \f[
 * f(\mathbf{x}, \mathbf{y}) = \sum_{j<i}\left(r_{i,j} - d_{i,j}\right)^2,
 * \f]
 * where
 * \f[
 * x_1 = y_1 = y_2 = 0,\ x_2 = u_1,\ x_i = u_{2(i-2)},\ y_i = u_{2(i-2)+1},
 * \f]
 * that is; \f$x_i\f$ is 0 for \f$i = 1\f$ and then is equal to all the even indicies of
 * the input parameter vector, similarly \f$y_i\f$ is equal to 0 for the first
 * two elements, and then the odd-indexed elements of the input vector
 * thereafter.
 * Further, \f$r_{i,j}\f$ is given by
 * \f[
 * r_{i,j} = \sqrt{(x_i - x_j)^2 + (y_i - y_j)^2},
 * \f]
 * and \f$d\f$ is a symmetric matrix given by:
 * \f[
 * \mathbf{d} = d_{i,j} =
 * \begin{bmatrix}
 * 1.27 \\
 * 1.69 & 1.43 \\
 * 2.04 & 2.35 & 2.43 \\
 * 3.09 & 3.18 & 3.26 & 2.85 \\
 * 3.20 & 3.22 & 3.27 & 2.88 & 1.55 \\
 * 2.86 & 2.56 & 2.58 & 2.59 & 3.12 & 3.06 \\
 * 3.17 & 3.18 & 3.18 & 3.12 & 1.31 & 1.64 & 3.00 \\
 * 3.21 & 3.18 & 3.18 & 3.17 & 1.70 & 1.36 & 2.95 & 1.32 \\
 * 2.38 & 2.31 & 2.42 & 1.94 & 2.85 & 2.81 & 2.56 & 2.91 & 2.97
 * \end{bmatrix}.
 * \f]
 * The parameters of the above are subject to \f$0 \le u_1 \le 4\f$, \f$-4 \le
 * u_i \le 4\f$, \f$i = 2, 3, \cdots, n\f$ where \f$n = 17\f$. The global
 * minima occurs at \f$u_i^* = (0.651906, 1.30194, 0.099242, −0.883791,
 * −0.8796, 0.204651, −3.28414, 0.851188, −3.46245, 2.53245, −0.895246,
 * 1.40992, −3.07367, 1.96257, −2.97872, −0.807849, −1.68978)\f$, with a vaue
 * of \f$11.7464\f$.
 */
class cola: public synthetic {
    public:
        cola ();
        double evaluate (inst::set x) override;
};

/**
 * The colville function has the following formula:
 * \f[
 * \begin{align*}
 * f(\mathbf{x}) = &100(x_1 - x_2^2)^2 + (1 - x_1)^2 + \\
 * &90(x_4 - x_3^2)^2 + (1 - x_3)^2 + \\
 * 10.1((x_2 - 1)^2 + (x_4-1)^2) + \\
 * 19.8 (x_2 - 1)(x_4-1),
 * \end{align*}
 * subject to the bounds \f$-10 \le x_i \le 10\f$ with the global minimum
 * located at \f$\mathbf{x}^* = (1, \ldots, 1)\f$ with value \f$f(\mathbf{x}^*)
 * = 0\f$. \f]
 */
class colville: public synthetic {
    public:
        colville ();
        double evaluate (inst::set x) override;
};

/**
 * Corana's parabola function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^n_{i=1}
 * \begin{cases}
 * 0.15d_i \bigg(z_i - 0.05 \text{sign} (z_i)\bigg)^2 &\text{if } \vert x_i - z_i\vert < 0.05 \\
 * d_ix_i^2 &\text{otherwise},
 * \end{cases}
 * \f]
 * where
 * \f[
 * \begin{align*}
 * z_i &= 0.2 \left\lfloor \left\vert \frac{x_i}{s_i}\right\vert + 0.49999
 * \right\rfloor \text{sign}(x_i), \\
 * s_i &= 0.2, \\
 * d_i &= [1, 1000, 10, 100].
 * \end{align*}
 * \f]
 * The parameters are subject to \f$-100 \le x_i \le 100\f$ for \f$i =
 * 1,2,3,4\f$, with the global minimum occurring at \f$\mathbf{x}^* = (0,
 * \cdots, 0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 *
 * FIXME Do not use in rankings; fix the floating-point inaccuracies before
 * including in benchmark suite.
 */
class corana: public synthetic {
    public:
        corana ();
        double evaluate (inst::set x) override;
};

/**
 * The cosine mixture function (2-dimensional) has the following formula:
 * \f[
 * f(\mathbf{x}) = -0.1 \sum^n_{i=1}\cos(5\pi x_i) - \sum^n_{i=1}x_i^2
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$ and \f$n=2\f$. The global minimum is
 * locatd at \f$\mathbf{x}^* = (0, 0)\f$ and has value \f$f(\mathbf{x}^*) =
 * 0.2\f$.
 */
class cosine_mixture2: public synthetic {
    public:
        cosine_mixture2 ();
        double evaluate (inst::set x) override;
};

/**
 * The cosine mixture function has the following formula:
 * \f[
 * f(\mathbf{x}) = -0.1 \sum^n_{i=1}\cos(5\pi x_i) - \sum^n_{i=1}x_i^2
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$. The global minimum is
 * locatd at \f$\mathbf{x}^* = (0, \ldots, 0)\f$ and has value \f$f(\mathbf{x}^*) =
 * \frac{1}{n}\f$.
 */
class cosine_mixture: public synthetic {
    public:
        cosine_mixture (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Cross-in-tray function has the following formula:
 * \f[
 * f(\mathbf{x}) = -0.0001\bigg(\vert \sin(x_1)\sin(x_2)
 * e^{\vert 100 - \sqrt{x_1^2 + x_2^2}/\pi\vert}\vert + 1 \bigg)^{0.1},
 * \f]
 * subject to
 * \f$-10\le x_i \le 10\f$. There are four global minima, located at
 * \f$\mathbf{x}^* = (\pm 1.349406685353340, \pm 1.349406608602084)\f$ with
 * value \f$f(\mathbf{x}^*) = -2.062611870822739\f$.
 */
class cross_in_tray: public synthetic {
    public:
        cross_in_tray ();
        double evaluate (inst::set x) override;
};

/**
 * The Csendes function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^D_{i=1} x_i^6\left(2 + \sin\frac{1}{x_i}\right),
 * \f]
 * subject to \f$-1\le x_i \le 1\f$. The global minimum is located at
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 * Note that to avoid evaluating \f$\lim_{x_i\to 0}\sin\left(\frac{1}{x_i}\right)\f$, we
 * add the smallest representable floating point value above \c 1.0 to
 * \f$x_i\f$.
 */
class csendes: public synthetic {
    public:
        csendes (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Cube function has the formula:
 * \f[
 * f(\mathbf{x}) = 100 (x_2 - x_1^3)^2 + (1 - x_1)^2,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (1, 1)\f$, with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class cube: public synthetic {
    public:
        cube ();
        double evaluate (inst::set x) override;
};


/**
 * The Damavandi function has the following formula:
 * \f[
 * \begin{align*}
 * f(\mathbf{x}) =
 * &\bigg(1 - \left\vert \frac{\sin\big(\pi(x_1 - 2)\big) \cdot
 * \sin\big(\pi(x_2 - 2)\big)}{\pi^2(x_1-2)(x_2-2)\right\vert^5\bigg) //
 * &\bigg(2 + (x_1-7)^2 + 2(x_2 - 7)^2\bigg),
 * \end{align*}
 * \f]
 * subject to \f$0\le x_i \le 14\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (2,2)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class damavandi: public synthetic {
    public:
        damavandi ();
        double evaluate (inst::set x) override;
};

/**
 * The Deb 1 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \frac{1}{D}\sum^D_{i=1}\sin^6(5\pi x_i),
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$. There are \f$5^Df$ global minima which
 * are evenly spaced. One such minimum is found at \f$\mathbf{x}^* = (0.3,
 * \ldots, 0.3)\f$ with value \f$f(\mathbf{x}^*) = -1\f$.
 */
class deb1: public synthetic {
    public:
        deb1 (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Deb 2 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\frac{1}{D}\sum^D_{i=1} \sin^6\left(5\pi
 * \big(x_i^{3/4} - 0.05 \big)\right),
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$. There are \f$5^Df$ global minima which
 * are evenly spaced. One such minimum is found at \f$\mathbf{x}^* = (0.0796993926887,
 * \ldots, 0.0796993926887)\f$ with value \f$f(\mathbf{x}^*) = -1\f$.
 */
class deb2: public synthetic {
    public:
        deb2 (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Deckkers-Aarts function has the following formula:
 * \f[
 * f(\mathbf{x}) = 15^5x_1^2 + x_2^2 - \big(x_1^2 + x_2^2\big)^2 +
 * 10^{-5}\big(x_1^2 + x_2^2\big)^4,
 * \f]
 * subject to \f$-20 \le x_i \le 20\f$. The two global minima are located at
 * \f$\mathbf{x}^* (0, \pm15)\f$, with value \f$f(\mathbf{x}^*) = -24777\f$.
 *
 * FIXME resolve floating-point arithmetic precision errors in this function's
 * implementation.
 */
class deckkers_aarts: public synthetic {
    public:
        deckkers_aarts ();
        double evaluate (inst::set x) override;
};

/**
 * The deVillers Glasser 1 function has the following formula, adapted from :
 * 'A Continuation Approach for Solving Large-Residual Nonlinear Least Squares
 * Problems', p1145 <https://epubs.siam.org/doi/abs/10.1137/0908058>
 * \f[
 * f(\mathbf{x}) = \sum^{24}_{i=1}\left( x_1x_2^{0.1(i-1)}\sin(0.1x_3(i-1) + x_4) - y_i
 * \right)^2,
 * \f]
 * where
 * \f[
 * y_i = 60.137 \cdot 1.371^{0.1 (i-1)} \sin(3.112 \cdot 0.1(i-1) + 1.761).
 * \f]
 * This is subject to the bounds \f$-500 \le x_i \le 500\f$, with the global
 * minimum arising at \f$\mathbf{x}^* = (60.173, 1.371, 3.112, 1.761)\f$, with
 * a value of \f$f(\mathbf{x}^*) = 0\f$.
 */
class devillers_glasser1: public synthetic {
    public:
        devillers_glasser1 ();
        double evaluate (inst::set x) override;
};

/**
 * The deVillers Glasser 2 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{16}_{i=1}\left(x_1x_2^{0.1(i-1)}\tanh\big(x_4\cdot 0.1(i-1) + \sin(x_4 \cdot 0.1(i-1))\big)\cos\big(0.1(i-1)e^{x_5} - y_i \right)^2,
 * \f]
 * where
 * \f[
 * y_i = 53.81 \cdot 1.27^{0.1(i-1)}\tanh\big(3.012 \cdot 0.1(i-1) + \sin(2.13
 * \cdot 0.1(i-1)\big) \cos\big(e^{0.507} \cdot 0.1(i-1)\big).
 * \f]
 * It is subject to \f$-500 \le x_i \le 500\f$, and has a global minimum at
 * \f$\mathbf{x}^* = (53.81, 1.27, 3.012, 2.13, 0.507)\f$ with value
 * \f$f(\mathbf{x}^*) = 0\f$.
 */
class devillers_glasser2: public synthetic {
    public:
        devillers_glasser2 ();
        /**
         * FIXME improve floating point calculation accuracy for function
         * evaluations
         */
        double evaluate (inst::set x) override;
};

/**
 * The Dixon & Price function has the following formula:
 * \f[
 * f(\mathbf{x}) = (x_1-1)^2 + \sum^D_{i=2} i(2x_i^2-x_{i-1})^2,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$. The global minimum is found at the
 * input vector with elements \f$\mathbf{x}_i^* = 2^{\frac{2^i-2}{2^i}}\f$,
 * with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class dixon_price: public synthetic {
    public:
        dixon_price (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Dolan function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left(x_1+1.7x_2\right)\sin\left(x_1\right)-1.5x_3-0.1x_4
 * \cos\left(x_5+x_4-x_1\right)+0.2x_5^2-x_2-1
 * \f]
 * subject to \f$-100 \le x_i \le 100\f$. There is a global minimum at
 * \f$\mathbf{x}^* =
 * (98.964258312237106,100,100,99.224323672554704,−0.249987527588471)\f$
 * with value \f$f(\mathbf{x}^*) = −529.8714387324576\f$.
 */
class dolan: public synthetic {
    public:
        dolan ();
        double evaluate (inst::set x) override;
};

/**
 * The (type III) Complex Deceptive Problem is given by
 * \f[
 * f(\mathbf{x}) = -\left[ \frac{1}{n}\sum^n_{i=1}g_i(x_i)\right]^\beta,
 * \f]
 * where \f$\beta\f$ is a non-linearity factor. Here we take \f$\beta = 2\f$.
 * Further
 * \f[
 * g_i(x_i) = \begin{cases}
 * x_i &\text{if } x_i < 0\\
 * -\frac{x_i}{\alpha_i} + \frac{4}{5} &\text{if } 0\le x_i \le \frac{4}{5}\alpha_i \\
 *  \frac{5x_i}{\alpha_i} - 4 &\text{if } \frac{4}{5}\alpha_i \le x_i < \alpha_i \\
 *  \frac{5(x_i - \alpha_i)}{\alpha_i - 1}+1 &\text{if } \alpha_i \le x+o <
 *  \frac{1 + 4\alpha_i}{5} \\
 *  \frac{x_i - 1}{1-\alpha_i} + \frac{4}{5} &\text{otherwise.}
 * \end{cases}
 * \f]
 * For each dimension, we select \f$\alpha_i \in (0, 1)\f$. The parameters are
 * constrained such that \f$0\le x_i \le 1\f$. The global minimum arises at
 * \f$\mathbf{x}_i^* = \alpha_i\f$, with value \f%f(\mathbf{x}^*) = -1.\f$.
 */
class deceptive: public synthetic {
    public:
        deceptive (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The deflected corrugated spring benchmark has the following formula:
 * \f[
 * f(\mathbf{x}) = 0.1 \sum^n_{i=1}(x_i - \alpha)^2 - \cos \left( k
 * \sqrt{\sum^n_{i=1}(x_i - \alpha)^2}\right),
 * \f]
 * where \f$\alpha = k = 5\f$, and the parameters are subject to \f$0 \le x_i
 * \le 2\alpha\f$. The global minimum is located at \f$x^*_i = \alpha\f$ with a
 * value of \f$f(\mathbf{x}^*) = -1\f$.
 */
class deflected_corrugated_spring: public synthetic {
    public:
        deflected_corrugated_spring (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Drop Wave function has the following equation:
 * \f[
 * f(\mathbf{x}) = -\frac{1 + \cos\left(12\sqrt{x_1^2 + x_2^2}\right)}
 * {\frac{1}{2}\left(x_1^2+x_2^2\right) + 2},
 * \f]
 * where \f$-5.12 \le x_i \le 5.12\f$, with the global minimum located at the
 * origin \f$\mathbf{x}^* = (0, 0)\f$ with value \f$f(\mathbf{x}^*) = -1\f$
 */
class drop_wave: public synthetic {
    public:
        drop_wave ();
        double evaluate (inst::set x) override;
};

/**
 * The Easom function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\cos(x_1)\cos(x_2)\exp\left[-(x_1 - \pi)^2 - (x_2 -
 * \pi)^2\right],
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (\pi, \pi)\f$ with value \f$f(\mathbf{x}^*) = -1\f$.
 */
class easom: public synthetic {
    public:
        easom ();
        double evaluate (inst::set x) override;
};

/**
 * The Egg Crate function has the following formula:
 * \f[
 * f(\mathbf{x}) = x_1^2 + x_2^2 + 25\big(\sin^2(x_1) + \sin^2(x_2)\big),
 * \f]
 * subject to \f$-5\le x_i \le 5\f$, with the global minimum located at the
 * origin: \f$\mathbf{x}^* = (0,0)\f$, with value \f$(\mathbf{x}^*) = 0\f$.
 */
class egg_crate: public synthetic {
    public:
        egg_crate ();
        double evaluate (inst::set x) override;
};

/**
 * The Egg Holder function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{n-1}_{i=1}\left(-(x_{i+1} + 47)\sin \bigg(\sqrt{\vert
 * x_{i+1} + 0.5x_i + 47\vert}\bigg) - x_1\sin \bigg(\sqrt{\vertx_i - (x_{i+1}
 * + 47\vert}\bigg)\right),
 * \f]
 * subject to \f$-512 \le x_i \le 512\f$ with the global minimum located at
 * \f$\mathbf{x}^* = (512, 404.2319)\f$, with value \f$f(\mathbf{x}^*) \approx
 * -959.640662720850742\f$.
 */
class egg_holder: public synthetic {
    public:
        egg_holder ();
        double evaluate (inst::set x) override;
};

/**
 * The El-Attar-Vidyasagar-Dutta function is the following formula:
 * \f[
 * f(\mathbf{x}) = (x_1^2 + x_2 - 10)^2 + (x_1 + x_2^2 - 7)^2 + (x_1^2 + x_2^3
 * - 1)^2,
 * \f]
 * and is subject to \f$-100 \le x_i \le 100\f$, with the global minimum at
 * \f$\mathbf{x}^* = (3.40918683, -2.17143304)\f$ with a value of
 * \f$f(\mathbf{x}^* = 1.712780354\f$.
 */
class el_attar_vidyasagar_dutta: public synthetic {
    public:
        el_attar_vidyasagar_dutta ();
        double evaluate (inst::set x) override;
};

/**
 * The exponential benchmark function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\exp \left(-0.5\sum^n_{i=1}x_i^2\right),
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$, with the global minimum located at the
 * origin; \f$\mathbf{x}^* = (0, \ldots, 0)\f$, with value \f$f(\mathbf{x}^*) =
 * -1\f$.
 */
class exponential: public synthetic {
    public:
        exponential (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The exp2 function has the following formula
 * \f[
 * f(\mathbf{x}) = \sum^9_{i=0}\left(
 * \exp\bigg(\frac{-ix_1}{10}\bigg) - 5\exp\bigg(\frac{-ix_2}{10}\bigg)
 * - \exp\bigg(\frac{-i}{10}\bigg) + 5\exp(-i)
 * \right)^2,
 * \f]
 * with domain \f$0\le x_i \le 20\f$ and a global minimum located at
 * \f$\mathbf{x}^* = (1,10)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class exp2: public synthetic {
    public:
        exp2 ();
        double evaluate (inst::set x) override;
};

/**
 * The Franke function has the following equation:
 * \f[
 * f(\mathbf{x}) = \frac{3}{4} \exp \left( -\frac{(9x_1 - 2)^2}{4} -
 * \frac{(9x_2 - 2)^2}{4}\right) +
 * \frac{3}{4} \exp \left(-\frac{(9x_1 + 1)^2}{49} - \frac{9x_2 + 1}{10}
 * \right) +
 * \frac{1}{2} \exp \left(-\frac{(9x_1 - 7)^2}{4} - \frac{(9x_2 - 3)^2}{4}
 * \right) -
 * \frac{1}{5} \exp \left(-(9x_1 - 4)^2 - (9x_2 - 7)^2\right),
 * \f]
 * subject to \f$0 \le x_i \le 1\f$ with a global minimum at \f$\mathbf{x}^* =
 * (0.45571037432, 0.78419067287)\f$ with value \f$f(\mathbf{x}^*) =
 * 0.00111528244\f$.
 */
class franke: public synthetic {
    public:
        franke ();
        double evaluate (inst::set x) override;
};

/**
 * The Freudenstein Roth function has the following equation
 * \f[
 * f(\mathbf{x}) = \Big(x_1 - 13 + \big((5 - x_2)x_2 - 2\big)x_2\Big)^2 +
 * \Big(x_1 - 29 + \big((x_2 + 1)x_2 - 14\big)x_2\Big)^2,
 * \f]
 * subject to \f$-10\le x_i \le 10\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (5,4)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class freudenstein_roth: public synthetic {
    public:
        freudenstein_roth ();
        double evaluate (inst::set x) override;
};

/**
 * The gear function has the following equation:
 * \f[
 * f(\mathbf{x}) = \left(
 * \frac{1}{6.931} - \frac{\lfloor x_1 \rfloor \cdot \lfloor x_2 \rfloor}
 * {\lfloor x_3 \rfloor \cdot \lfloor x_4\rfloor}
 * \right)^2,
 * \f]
 * subject to \f$12 \le x_i \le 60\f$. There are many global minima, one being
 * at \f$\mathbf{x}^* = (16, 19, 43, 49)\f$ with value \f$f(\mathbf{x}^*) =
 * 2.700857148886513\times 10^{−12}\f$.
 */
class gear: public synthetic {
    public:
        gear ();
        double evaluate (inst::set x) override;
};

/**
 * The Giunta function has the following formula:
 * \f[
 * f(\mathbf{x}) = 0.6 + \sum^2_{i=1}\left(
 * \sin\Big(\frac{16}{15}x_i - 1\Big) +
 * \sin^2\Big(\frac{16}{15}x_i - 1\Big) +
 * \frac{1}{15}\sin\Big(4\big(\frac{16}{15}x_i - 1\big)\Big)\right),
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (0.4673200277395354, 0.4673200169591304)\f$ with value
 * \f$f(\mathbf{x}^*) = 0.06447042053690566)\f$.
 */
class giunta: public synthetic {
    public:
        giunta ();
        double evaluate (inst::set x) override;
};

/**
 * The Goldstein-Price function consists of a product of two factors
 * \f[
 * f(\mathbf{x}) = f_1(\mathbf{x}) \cdot f_2(\mathbf{x}),
 * \f]
 * where
 * \f[
 * \begin{align*}
 * f_1 &= 1 + (x_1 + x_2 + 1)^2\cdot(19 - 14x_1 + 2x_1^2 - 14x_2 + 6x_1x_2 +
 * 3x_2^2) \\
 * f_2 &= 30 + (2x_1 - 3x_2)^2\cdot(18 - 32x_1 + 12x_1^2 + 48x_2 - 36x_1x_2 +
 * 27x_2^2),
 * \end{align*}
 * \f]
 * subject to \f$-2 \le x_i \le 2\f$, with the global minimum located at
 * \f$\mathbf{x}^* = (0. -1)\f$, with \f$f(\mathbf{x}^*) = 3\f$.
 */
class goldstein_price: public synthetic {
    public:
        goldstein_price ();
        double evaluate (inst::set x) override;
};

/**
 * The Griewank function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^n_{i=1}\frac{x_i^2}{4000} -
 * \prod\cos\Big(\frac{x_i}{\sqrt{i}}\Big)+1,
 * \f]
 * subject to \f$-50\le x_i \le 20\f$, with the global minima located at
 * \f$\mathbf{x}^* = (0, \ldots, 0)\f$ with value \f$f(\mathbf{x}^* = 0\f$.
 */
class griewank: public synthetic {
    public:
        griewank (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Gulf Research Problem is given the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{99}_{i=1}\left(
 * \exp \bigg(-\frac{(u_1 - x_2)^{x_3}}{x_1} \bigg) - \frac{i}{100}
 * \right)^2,
 * \f]
 * where
 * \f[
 * u_i = 25 + \sqrt{\left(-50 \ln 'left(\frac{i}{100}\right)\right)},
 * \f]
 * subject to
 * \f$0.1\le x_1 \le 100\f$, \f$0 \le x_2 \le 25.6\f$ and \f$0\le x_3 \le
 * 5\f\f$ with the global minimum located at \f$\mathbf{x}^* = (50, 25, 1.5)\f$
 * with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class gulf: public synthetic {
    public:
        gulf ();
        double evaluate (inst::set x) override;
};

/**
 * The Hansen function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^4_{i=1}(i+1)\cos (ix_1 + i + 1) \cdot \sum^4_{j=1}(j +
 * 1)\cos\big((j+1)x_2 + j + 1\big),
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$, with multiple global minima, one of
 * them being located at \f$\mathbf{x}^* = (-7.58989583, -7.70831466)\f$, with
 * value \f$f(\mathbf{x}^*) = -176.54\f$.
 */
class hansen: public synthetic {
    public:
        hansen ();
        double evaluate (inst::set x) override;
};

/**
 * The Hartman 3 function has the following formula:
 * \f[
 * f(\mathbf{x}) = - \sum^4_{i=1} c_i \exp \left[
 * -\sum^3_{j=1}a_{ij}(x_j - p_{ij})^2
 * \right],
 * \f]
 * where
 * \f[
 * \begin{align*}
 * \mathbf{A} = [A_{ij}] &= \begin{bmatrix}
 * 3 & 10 & 30 \\ 0.1 & 10 & 35 \\ 3 & 10 & 30 \\ 0.1 & 10 & 35
 * \end{bmatrix} \\
 * \mathbf{c} = c_i &= \begin{bmatrix} 1 \\ 1.2 \\ 3 \\ 3.2 \end{bmatrix} \\
 * \mathbf{P} = p_i &= \begin{bmatrix}
 * 0.36890 & 0.11700 & 0.26730 \\
 * 0.46990 & 0.43870 & 0.74700 \\
 * 0.10910 & 0.87320 & 0.55470 \\
 * 0.03815 & 0.57430 & 0.88280
 * \end{bmatrix}
 * \end{align*}
 * \f]
 * subject to \f$0 \le x_i \le 1\f$ for \f$j \in \{1,2,3\}\f$. The global
 * minimum is located at \f$\mathbf{x}^* = (0.1, 0.55592003, 0.85218259)\f$
 * with value \f$f(\mathbf{x}^*) = -3.86278214782076\f$.
 */
class hartman3: public synthetic {
    public:
        hartman3 ();
        double evaluate (inst::set x) override;
};

/**
 * The Hartman 6 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\sum^4_{i=1}c_i \exp \left[
 * \sum^6_{j=1}a_{ij} (x_j - p_{ij})^2
 * \right],
 * \f]
 * where
 * \f[
 * \begin{align*}
 * \mathbf{A} = [a_{ij}] &= \begin{bmatrix}
 * 10 & 3 & 17 & 3.5 & 17 & 8 \\
 * 0.05 & 10 & 17 & 0.1 & 8 & 14 \\
 * 3 & 3.5 & 1.7 & 10 & 17 & 8 \\
 * 17 & 8 & 0.05 & 10 & 0.1 & 14
 * \end{bmatrix} \\
 * \mathbf{c} = c_i &= \begin{bmatrix}1 & 1.2 & 3 & 3.2 \end{bmatrix} \\
 * \mathbf{p} = p_{ij} &= \begin{bmatrix}
 *
 * \end{bmatrix}
 * 0.1312 & 0.1696 & 0.5569 & 0.0124 & 0.8283 & 0.5886 \\
 * 0.2329 & 0.4135 & 0.8307 & 0.3736 & 0.1004 & 0.9991 \\
 * 0.2348 & 0.1451 & 0.3522 & 0.2883 & 0.3047 & 0.665 \\
 * 0.4047 & 0.8828 & 0.8732 & 0.5743 & 0.1091 & 0.0381
 * \end{bmatrix}
 * \end{align*}
 * \f]
 * subject to \f$0\le x_i \le 1\f$ for \f$i\in [1, 6]\f$. The global minimum is
 * located at \f$\mathbf{x}^* = (0.20168952, 0.15001069, 0.47687398,
 * 0.27533243, 0.31165162, 0.65730054)\f$ with value \f$f(\mathbf{x}^*) =
 * -3.32236801141551\f$.
 */
class hartman6: public synthetic {
    public:
        hartman6 ();
        double evaluate (inst::set x) override;
};

/**
 * The Helical Valley function has the following formula:
 * \f[
 * f(\mathbf{x}) = 100\left[(x_2 - 10\theta)^2 + \left(\sqrt{x_1^2 + x_2^2}
 * -1\right) \right] + x_3^2,
 * \f]
 * where
 * \f[
 * \theta = \begin{cases}
 * \frac{1}{2\pi}\tan^{-1}\left(\frac{x_1}{x_2}\right), &\text{if } x_1 \ge 0\\
 * \frac{1}{2\pi}\tan^{-1}\left(\frac{x_1}{x_2} + 0.5\right) & \text{if } x_1 < 0.
 * \end{cases}
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ with the global minimum located at
 * \f$\mathbf{x}^* = (1,0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class helical_valley: public synthetic {
    public:
        helical_valley ();
        double evaluate (inst::set x) override;
};

/**
 * The Himmelblau function has the following formula:
 * \f[
 * f(\mathbf{x}) = \big(x_1^2 + x_2 - 11\big)^2 +
 * \big(x_1 + x_2^2 - 7\big)^2,
 * \f]
 * subject to \f$-5 \le x_i \le 5\f$ with the global minimum located at
 * \f$\mathbf{x}^* = (3, 2)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class himmelblau: public synthetic {
    public:
        himmelblau ();
        double evaluate (inst::set x) override;
};

/**
 * The Holder-Table function has the following equation:
 * \f[
 * f(\mathbf{x}) = - \left\vert \sin(x_1) \cos(x_2)\exp \left\vert
 * 1 - \frac{\sqrt{x_1^2 + x_2^2}}{\pi}
 * \right\vert\right\vert,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ with four global minima, one of which is
 * at \f$\mathbf{x}^* = (8.055023472141116, 9.664590028909654)\f$ with value
 * \f$f(\mathbf{x}^*) = -19.20850256788675\f$.
 */
class holder_table: public synthetic {
    public:
        holder_table ();
        double evaluate (inst::set x) override;
};

/**
 * The Hosaki function has the following equation:
 * \f[
 * f(\mathbf{x}) = \bigg(
 * 1 - 8x_1 + 7x_1^2 - \frac{7}{3}x_1^3 + \frac{1}{4}x_1^4
 * \bigg) x_2^2 e^{-x_2},
 * \f]
 * subject to \f$0 \le x_1 \le 5\f$ and \f$0 \le
 * x_2 \le 6\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (4,2)\f$ with value
 * \f$f(\mathbf{x}^*) = -2.3458\f$.
 */
class hosaki: public synthetic {
    public:
        hosaki ();
        double evaluate (inst::set x) override;
};

/**
 * The Jennrich-Sampson function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{10}_{i=1}\bigg(2 + 2i -\big(e^{ix_1}+e^{ix_2}\big)\bigg)^2,
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$ with the global minimum located at
 * \f$\mathbf{x}^* = (0.257825, 0.257825)\f$ with value \f$f(\mathbf{x}^*) =
 * 124.3621824\f$.
 */
class jennrich_sampson: public synthetic {
    public:
        jennrich_sampson ();
        double evaluate (inst::set x) override;
};

/**
 * Judge's function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{20}_{i=1}\left(
 * (x_1 + B_ix_2 + C_ix_2^2) - A_i
 * \right)^2
 * \f],
 * where the vectors A, B and C are given by:
 * \f[
 * \begin{align*}
 * A &= \begin{bmatrix} 4.284 & 4.149 & 3.877 & 0.533 & 2.211 & 2.389 & 2.145 &
 * 3.231 & 1.998 & 1.379 & 2.106 & 1.428 & 1.011 & 2.179 & 2.858 & 1.388 &
 * 1.651 & 1.593 & 1.046 & 2.152 \end{bmatrix} \\
 * B &= \begin{bmatrix} 0.286 & 0.973 & 0.384 & 0.276 & 0.973 & 0.543 & 0.957 &
 * 0.948 & 0.543 & 0.797 & 0.936 & 0.889 & 0.006 & 0.828 & 0.399 & 0.617 &
 * 0.939 & 0.784 & 0.072 & 0.889 \end{bmatrix} \\
 * C &= \begin{bmatrix} 0.645 & 0.585 & 0.310 & 0.058 & 0.455 & 0.779 & 0.259 &
 * 0.202 & 0.028 & 0.099 & 0.142 & 0.296 & 0.175 & 0.180 & 0.842 & 0.039 &
 * 0.103 & 0.620 & 0.158 & 0.704 \end{bmatrix}
 * \end{align*}
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\$, with a global minimum
 * located at \f$\mathbf{x}^* = (0.86479, 1.2357)\f$ with value
 * \f$f(\mathbf{x}^*) = 16.0817307\f$.
 */
class judge: public synthetic {
    public:
        judge ();
        double evaluate (inst::set x) override;
};

/**
 * The Langermann 2 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\sum^m_{i=1}c_i e^{-\frac{1}{\pi} \sum^D_{j=1}(x_j-a_{ij})^2}
 * \cos\left(\pi\sum^D_{j=1}(x_j-a_{ij})^2\right),
 * \f]
 * where \f$\mathbf{a}\f$, \f$\mathbf{b}\f$ and \f$\mathbf{c}\f$ are the
 * following vectors:
 * \f[
 * \begin{align*}
 * \mathbf{a} &= \begin{bmatrix} 3 & 5 & 2 & 1 & 7 \end{bmatrix} \\
 * \mathbf{b} &= \begin{bmatrix} 5 & 2 & 1 & 4 & 9 \end{bmatrix} \\
 * \mathbf{c} &= \begin{bmatrix} 1 & 2 & 5 & 2 & 3 \end{bmatrix}.
 * \f]
 * The parameters are subject to \f$0 \le x_i \le 10\f$ and there is a global
 * minimum located at \f$\mathbf{x}^* = (2.00299219, 1.006096)\f$ with value
 * \f$f(\mathbf{x}^*) = -5.1621259\f$.
 */
class langermann2: public synthetic {
    public:
        langermann2 ();
        double evaluate (inst::set x) override;
};

/**
 * The Langerman-5 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\sum^m_{i=1}c_i e^{-\frac{1}{\pi} \sum^D_{j=1}(x_j-a_{ij})^2}
 * \cos\left(\pi\sum^D_{j=1}(x_j-a_{ij})^2\right),
 * \f]
 * where the matrix \f$\mathbf{A}\f$ and the column vector \f$\mathbf{c}\f$ are
 * given by
 * \f[
 * \begin{align*}
 * \mathbf{A} = [A_{ij}] &= \begin{bmatrix}
 * 9.681 & 0.667 & 4.783 & 9.095 & 3.517 & 9.325 & 6.544 & 0.211 & 5.122 & 2.020 //
 * 9.400 & 2.041 & 3.788 & 7.931 & 2.882 & 2.672 & 3.568 & 1.284 & 7.033 & 7.374 //
 * 8.025 & 9.152 & 5.114 & 7.621 & 4.564 & 4.711 & 2.996 & 6.126 & 0.734 & 4.982 //
 * 2.196 & 0.415 & 5.649 & 6.979 & 9.510 & 9.166 & 6.304 & 6.054 & 9.377 & 1.426 //
 * 8.074 & 8.777 & 3.467 & 1.863 & 6.708 & 6.349 & 4.534 & 0.276 & 7.633 & 1.5
 * \end{bmatrix} \\
 * \mathbf{c} = c_i &= \begin{bmatrix} 0.806 \\ 0.517 \\ 1.5 \\ 0.908 \\ 0.9 \end{bmatrix}
 * \end{align*}
 * \f]
 * subject to \f$0 \le x_i \le 10\f$ for \f$i \in [0, D-1]\f$ with \f$m = 5\f$.
 * The global mimimum is at \f$f(\mathbf{x}^*) = -1.4\f$.
 *
 * @todo find location of global minimum.
 */
class langermann5: public synthetic {
    public:
        langermann5 ();
        double evaluate (inst::set x) override;
};

/**
 * The Lennard Jones 6 benchmark function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{n/3-1}_{i=1} \sum^{n/3}_{i=1}
 * \delta_{e_{ij}} (e_{ij}^{-3} - 2)e_{ij}^{-3}
 * \f]
 * where
 * \f[
 * \begin{align*}
 * \delta_{e_{ij}} &= \begin{cases} 0 &\text{if } e_{ij} \le 0 \\ 1 &\text{othersise,} \\
 * e_{ij} &= (x_{3i} - x_{3j})^2 + (x_{3i+1} - x_{3j+1})^2 + (x_{3i+2} - x_{3i+2})^2
 * \end{align*}
 * \f]
 * subject to \f$-3 \le x_i \le 3\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (-2.66666470373, 2.73904387714, 1.42304625988,
 * -1.95553276732, 2.81714839844, 2.12175295546)\f$, with value
 *  \f$f(\mathbf{x}^*) = -1\f$.
 */
class lennard_jones: public synthetic {
    public:
        lennard_jones ();
        double evaluate (inst::set x) override;
};

/**
 * The keane function has the following formula:
 * \f[
 * f(\mathbf{x}) = \frac{\sin^2(x_1-x_2)\sin^2(x_1+x_2)}
 * {\sqrt{x_1^2 + x_2^2}},
 * \f]
 * subject to \f$0 \le x_i \le 10\f$, with one global minimum located at
 * \f$\mathbf{x}^* = (0, 1.39325)\f$ with value \f$f(\mathbf{x}^*) =
 * 0.673668\f$.
 */
class keane: public synthetic {
    public:
        keane ();
        double evaluate (inst::set x) override;
};

/**
 * The Leon function has the following formula:
 * \f[
 * f(\mathbf{x}) = 100\big(x_2 - x_1^2\big)^2 + (1 - x_1)^2,
 * \f]
 * subject to \f$-1.2 \le x_i \le 1.2\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (1,1)\f$ wiht value \f$f(\mathbf{x}^*) = 0\f$.
 */
class leon: public synthetic {
    public:
        leon ();
        double evaluate (inst::set x) override;
};

/**
 * The Levy 03 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sin^2(\pi w_1) + \sum^{d-1}_{i=1}(w_i-1)^2\left(
 * 1 + 10\sin^2(\pi w_i+1)\right) +
 * (w_d - 1)^2 \left(1 + \sin^2(2\pi w_d)\right),
 * \f]
 * where
 * \f[
 * w_i = 1 + \frac{x_i - 1}{4}, \forall i \in \{1, \ldots, d\}.
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$. The global minimum is located at
 * \f$\mathbf{x}^* = (1, \ldots, 1)\f$ with a value of \f$f(\mathbf{x}^* =
 * 0\f$.
 */
class levy3: public synthetic {
    public:
        levy3 (int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Levy 5 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^5_{i=1} \big(i\cos(i-1)x_1 + i\big)
 * \sum^5_{i=1} \big(i\cos(i + 1)x_2 + i\big) +
 * (5x_1 + 1.42513)^2 + (5x2 + 0.80032)^2,
 * \f]
 * subject to \f$-2 \le x_i \le 2\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (-0.34893137569, -0.79113519694)\f$ with a value of
 * \f$f(\mathbf{x}^*) = -135.27125929718\f$.
 */
class levy5: public synthetic {
    public:
        levy5 ();
        double evaluate (inst::set x) override;
};

/**
 * The Levy 13 benchmark function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sin^2(3\pi x_1) +
 * \big((x_1-1)^2\cdot (1 + \sin^2(3\pi x_2))\big) +
 * \big((x_2-1)^2\cdot (1 + \sin^2(2\pi x_2))\big),
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (1,1)\f$ with a value of \f$f(\mathbf{x}) = 0\f$.
 */
class levy13: public synthetic {
    public:
        levy13 ();
        double evaluate (inst::set x) override;
};

/**
 * The Matyas benchmark function has the following formula:
 * \f[
 * f(\mathbf{x}) = 0.26\big(x_1^2 + x_2^2\big) - 0.48 x_1x_2,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ with a global minimum located at
 * \f$\mathbf{Px}^* = (0,0)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class matyas: public synthetic {
    public:
        matyas ();
        double evaluate (inst::set x) override;
};

/**
 * The McCormick benchmark function is given as
 * \f[
 * f(\mathbf{x}) = \sin(x_1 + x_2) + (x_1 + x_2)^2 - (3/2)x_1 + (5/2)x_2 + 1,
 * \f]
 * which is subject to \f$-1.5 \le x_1 \le 4\f$ and \f$-3 \le x_2 \le 3\f$ with
 * a global minimum located at \f$\mathbf{x}^* = (-0.5471975602214493,
 * -1.547197559268372)\f$, with value \f$f(\mathbf{x}^*) =
 *  -1.913222954981037\f$.
 */
class mccormick: public synthetic {
    public:
        mccormick ();
        double evaluate (inst::set x) override;
};

/** This is a more compact way of passing the regression problem dimensions. */
typedef struct {
    /** prob is the dimensions of the problem (number of parameters to
     * optimise), as well as being the number of columns in the e_mat and
     * centres matrices. */
    int prob;
    /** coef is the size of the list of coefficients, which must be equal to
     * the number of rows in the e_mat and centres matrices. */
    int coef;
} rdims;

/**
 * This is the base class for implementing a set of functions devised by
 * Michael McCourt working at SigOpt, (a commercial function optimisation
 * service).
 * The derived functions all fit into the framework of a linear combination of
 * functions, many of which are positive definite kernels.
 */
class regression: public synthetic {
    public:
        enum class dist_type: char {
            one,        /** For calculating the l1 norm */
            squared,    /** For calculating the l2 norm */
            inf         /** For claculating the infinity norm */
        };

        /**
         * The constructor for the regression-type problems.
         * @param dims The number of dimensions of the problem
         * @param e_mat TODO document this
         * @param centers TODO document this
         * @param coefs The regression coefficients
         * @param n The benchmark name
         * @param fmin The known global minimum.
         * @param minloc The position of a/the global minimum
         */
        regression (
            rdims dims,
            double *e_mat,
            double *centres,
            double *coefs,
            const std::string &n,
            double fmin,
            double *minloc
            );

        virtual void kernel (double *xs, double *ret) = 0;

        double evaluate (inst::set x) override;

    protected:
        /**
         * The dist_sq(1) function
         * @param xs The input parameters
         * @param ret A double[dims.coef][dims.prob] array to return values.
         */
        void dist_sq_1 (double *xs, double *ret);

        /**
         * The dist_sq(2) function
         * @param xs The input parameters
         * @param ret A double[dims.coef][dims.prob] array to return values.
         */
        void dist_sq_2 (double *xs, double *ret);

        /**
         * The dist_sq(inf) function
         * @param xs The input parameters
         * @param ret A double[dims.coef][dims.prob] array to return values.
         */
        void dist_sq_inf (double *xs, double *ret);

    private:

        double *m_e_mat;
        double *m_centres;
        double *m_coefs;
        rdims m_dims;
};

class court01: public regression {
    public:
        court01 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 7, coef = 6 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.0859426686096;
        static constexpr double minloc[prob] =
        {0.6241, 0.7688, 0.8793, 0.2739, 0.7351, 0.8499, 0.6196};

        static constexpr double centres[coef * prob] = {
            0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
            0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6,
            0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6,
            0.4, 0.7, 0.4, 0.9, 0.4, 0.1, 0.9,
            0.9, 0.3, 0.3, 0.5, 0.2, 0.7, 0.2,
            0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4};

        static constexpr double e_mat[coef * prob] = {
                5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5};

        static constexpr double coefs[coef] = {1, 1, -2, 1, 1, 1};
};

class court02: public regression {
    public:
        court02 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 7, coef = 6 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -2.74162116801;
        static constexpr double minloc[prob] =
        {0.4068, 0.4432, 0.6479, 0.1978, 0.7660, 0.7553, 0.5640};

        static constexpr double centres[coef * prob] = {
            0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
            0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6,
            0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6,
            0.4, 0.7, 0.4, 0.9, 0.4, 0.1, 0.9,
            0.9, 0.3, 0.3, 0.5, 0.2, 0.7, 0.2,
            0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4};

        static constexpr double e_mat[coef * prob] = {
            5. , 5. , 5. , 5. , 5. , 5. , 5. ,
            1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
            1. , 1. , 1. , 1. , 1. , 1. , 1. ,
            5. , 5. , 5. , 5. , 5. , 5. , 5. ,
            5. , 5. , 5. , 5. , 5. , 5. , 5. ,
            5. , 5. , 5. , 5. , 5. , 5. , 5.};

        static constexpr double coefs[coef] = {-1, -1, -2, 1, 1, -1};
};

class court03: public regression {
    public:
        court03 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 9, coef = 9 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -3.02379637466;
        static constexpr double minloc[prob] =
        { 0.9317, 0.1891, 0.2503, 0.3646, 0.1603, 0.9829, 0.0392, 0.3263,
            0.6523 };

        static constexpr double centres[coef * prob] = {
            0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
            0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6, 0.4, 0.2,
            0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6, 0.9, 0.1,
            0.7, 0.2, 0.7, 0.7, 0.3, 0.3, 0.8, 0.6, 0.4,
            0.4, 0.6, 0.4, 0.9, 0.4, 0.1, 0.9, 0.3, 0.3,
            0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4, 0.5, 0.8,
            0.8, 0.3, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6,
            0.8, 0.3, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6,
            0.8, 0.3, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6};

        static constexpr double e_mat[coef * prob] = {
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1,
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                1, 1, 1, 1, 1, 1, 1, 1, 1};

        static constexpr double coefs[coef] = {1, -1, 1, 1, 1, 1, -1, -2, -1};
};

class court04: public regression {
    public:
        court04 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 10, coef = 9 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -4.631135472012;
        static constexpr double minloc[prob] =
        { 0.8286, 0.3562, 0.3487, 0.4623, 0.1549,
          0.7182, 0.2218, 0.3919, 0.5394, 0.441};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6, 0.4, 0.2, 0.9,
                0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6, 0.9, 0.1, 0.2,
                0.7, 0.2, 0.7, 0.7, 0.3, 0.3, 0.8, 0.6, 0.4, 0.1,
                0.4, 0.6, 0.4, 0.9, 0.4, 0.1, 0.9, 0.3, 0.3, 0.2,
                0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4, 0.5, 0.8, 0.6,
                0.8, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6, 0.5,
                0.8, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6, 0.5,
                0.8, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6, 0.5};

        static constexpr double e_mat[coef * prob] = {
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5,
                0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05,
                0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5};

        static constexpr double coefs[coef] = {1, -1, 1, -1, 1, 1, -2, -1, -1};
};

class court05: public regression {
    public:
        court05 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 12, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -11.89842508364;
        static constexpr double minloc[prob] =
        { 0.636, 0.622, 0.39, 0.622, 0.29, 0.047,
          0.97, 0.26, 0.311, 0.247, 0.794, 0.189 };

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6, 0.4, 0.2, 0.9, 0.3, 0.7,
                0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6, 0.9, 0.1, 0.2, 0.5, 0.2,
                0.7, 0.2, 0.7, 0.7, 0.3, 0.3, 0.8, 0.6, 0.4, 0.1, 0.9, 0.9,
                0.4, 0.6, 0.4, 0.5, 0.4, 0.2, 0.8, 0.3, 0.3, 0.2, 0.5, 0.1,
                0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4, 0.5, 0.8, 0.6, 0.9, 0.1,
                0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0, 0.1, 0.2,
                0.8, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4, 0.6, 0.5, 0.3, 0.8,
                0.9, 0.5, 0.3, 0.2, 0.1, 0.9, 0.3, 0.7, 0.7, 0.7, 0.4, 0.4,
                0.2, 0.8, 0.6, 0.4, 0.6, 0.6, 0.5, 0, 0.2, 0.8, 0.2, 0.3};

        static constexpr double e_mat[coef * prob] = {
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,
                0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,
                0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,
                0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,
                0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,0.2 ,
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,
                0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 ,0.4 };


        static constexpr double coefs[coef] =
            {5, -2, 5, -5, -20, -2, 10, 2, -5, 5};
};

class court06: public regression {
    public:
        court06 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 5, coef = 7 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = 2.80720263234;
        static constexpr double minloc[prob] = {1, 1, 0.7636, 0.5268, 1};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.8, 0.8, 0.6, 0.9,
                0.6, 0.1, 0.2, 0.5, 0.2,
                0.7, 0.2, 0.1, 0.8, 0.9,
                0.4, 0.6, 0.5, 0.3, 0.8,
                0.9, 0.5, 0.3, 0.2, 0.4,
                0.2, 0.8, 0.6, 0.4, 0.6};

        static constexpr double e_mat[coef * prob] = {
                0.4 , 0.4 , 0.4 , 0.4 , 0.4 ,
                0.2 , 0.2 , 0.2 , 0.2 , 0.2 ,
                0.4 , 0.4 , 0.4 , 0.4 , 0.4 ,
                0.08, 0.08, 0.08, 0.08, 0.08,
                0.2 , 0.2 , 0.2 , 0.2 , 0.2 ,
                0.4 , 0.4 , 0.4 , 0.4 , 0.4 ,
                0.4 , 0.4 , 0.4 , 0.4 , 0.4};

        static constexpr double coefs[coef] = {-3, 2, -2, 4, -1, 5, -1};
};

class court07: public regression {
    public:
        court07 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 6, coef = 7 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.36321372933;
        static constexpr double minloc[prob] = {0.3811, 1, 0.2312, 0, 1, 0.1403};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.8, 0.8, 0.6, 0.9, 0.4,
                0.6, 1, 0.2, 0, 1, 0.3,
                0.7, 0.2, 0.1, 0.8, 0.9, 0.2,
                0.4, 0.6, 0.5, 0.3, 0.8, 0.3,
                0.9, 0.5, 0.3, 0.2, 0.4, 0.8,
                0.2, 0.8, 0.6, 0.4, 0.6, 0.9};

        static constexpr double e_mat[coef * prob] = {
                0.7 , 0.7 , 0.7 , 0.7 , 0.7 , 0.7 ,
                0.35, 0.35, 0.35, 0.35, 0.35, 0.35,
                0.7 , 0.7 , 0.7 , 0.7 , 0.7 , 0.7 ,
                0.14, 0.14, 0.14, 0.14, 0.14, 0.14,
                0.35, 0.35, 0.35, 0.35, 0.35, 0.35,
                0.7 , 0.7 , 0.7 , 0.7 , 0.7 , 0.7 ,
                0.49, 0.49, 0.49, 0.49, 0.49, 0.49};

        static constexpr double coefs[coef] = {2, 2, -4, 1, -2, 4, -2};
};

class court08: public regression {
    public:
        court08 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 4, coef = 7 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -3.45224058874;
        static constexpr double minloc[prob] = {0.5067, 1, 0.5591, 0.0823};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1,
                0.3, 0.8, 0.9, 0.4,
                0.6, 1, 0.2, 0,
                0.7, 0.2, 0.1, 0.8,
                0.4, 0, 0.8, 1,
                0.9, 0.5, 0.3, 0.2,
                0.2, 0.8, 0.6, 0.4};

        static constexpr double e_mat[coef * prob] = {
                0.7 , 0.7 , 0.7 , 0.7 ,
                0.35, 0.35, 0.35, 0.35,
                0.7 , 2.1 , 0.7 , 2.1 ,
                0.35, 0.35, 0.35, 0.35,
                1.4 , 0.7 , 1.4 , 0.7 ,
                0.7 , 0.7 , 0.7 , 0.7 ,
                0.49, 0.49, 0.49, 0.49};

        static constexpr double coefs[coef] = {2, 1, -8, 1, -5, 3, 2};
};

class court09: public regression {
    public:
        court09 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 8 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -10.17146707797;
        static constexpr double minloc[prob] = {0.594, 1, 0.205};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1,
                0.3, 0.8, 0.9,
                0.6, 1, 0.2,
                0.6, 1, 0.2,
                0.7, 0.2, 0.1,
                0.4, 0, 0.8,
                0.9, 0.5, 1,
                0, 0.8, 0.6};

        static constexpr double e_mat[coef * prob] = {
                0.6 , 0.6 , 0.6 ,
                0.36, 0.36, 0.36,
                0.6 , 0.3 , 0.6 ,
                2.4 , 6.  , 2.4 ,
                0.3 , 0.3 , 0.3 ,
                0.3 , 0.6 , 0.3 ,
                0.6 , 0.6 , 0.6 ,
                0.18, 0.3 , 0.3};

        static constexpr double coefs[coef] = {4, -3, -6, -2, 1, -3, 6, 2};
};

class court10: public regression {
    public:
        court10 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 8, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -2.51939597030;
        static constexpr double minloc[prob] =
            {0.5085, 0.5433, 0.2273, 1, 0.3381, 0.0255, 1, 0.5038};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6, 0.4,
                0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6, 0.9,
                0.7, 0, 0.7, 1, 0.3, 0, 0.8, 0.6,
                0.4, 0.6, 0.4, 1, 0.4, 0.2, 1, 0.3,
                0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4, 0.5,
                0.1, 0.2, 1, 0.4, 0.5, 0.6, 0.7, 0,
                0.9, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4,
                0, 0.5, 0.3, 0.2, 0.1, 0.9, 0.3, 0.7,
                0.2, 0.8, 0.6, 0.4, 0.6, 0.6, 0.5, 0};

        static constexpr double e_mat[coef * prob] = {
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8,
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8,
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8,
                0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4,
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8,
                2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4,
                0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4,
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8,
                1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6,
                0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8};

        static constexpr double coefs[coef] =
            {5, -2, 5, -5, -12, -2, 10, 2, -5, 5};
};

class court11: public regression {
    public:
        court11 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 8, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.39045528652;
        static constexpr double minloc[prob] =
            {0.4, 0.6, 0.4, 1, 0.4, 0.2, 1, 0.3};

        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
                0.3, 0.1, 0.5, 0.1, 0.8, 0.8, 0.6, 0.4,
                0.6, 0.7, 0.8, 0.3, 0.7, 0.8, 0.6, 0.9,
                0.7, 0, 0.7, 1, 0.3, 0, 0.8, 0.6,
                0.4, 0.6, 0.4, 1, 0.4, 0.2, 1, 0.3,
                0.5, 0.5, 0.2, 0.8, 0.5, 0.3, 0.4, 0.5,
                0.1, 0.2, 1, 0.4, 0.5, 0.6, 0.7, 0,
                0.9, 0.4, 0.3, 0.5, 0.2, 0.7, 0.2, 0.4,
                0, 0.5, 0.3, 0.2, 0.1, 0.9, 0.3, 0.7,
                0.2, 0.8, 0.6, 0.4, 0.6, 0.6, 0.5, 0};

        static constexpr double e_mat[coef * prob] = {
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 ,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 ,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 ,
                0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 ,
                1.5 , 1.5 , 1.5 , 1.5 , 1.5 , 1.5 , 1.5 , 1.5 ,
                0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 ,
                1.  , 1.  , 1.  , 1.  , 1.  , 1.  , 1.  , 1.  ,
                0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 , 0.5 };

        static constexpr double coefs[coef] =
            {5, -2, 5, -5, -7, -2, 10, 2, -5, 5};
};

// skip 12 because of bessel function (imported from scipy in evalse). TODO for
// completeness, implement scipy.special.jv).

class court13: public regression {
    public:
        court13 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 15 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = 1.49048296359;
        static constexpr double minloc[prob] = {1,1,1};

        static constexpr double centres[coef * prob] = {
                0.9, 0.9, 0.9,
                0.9, 0.9, 1,
                0.9, 1, 0.9,
                1, 0.9, 0.9,
                1, 1, 1,
                1, 0, 0,
                0.5, 0, 0,
                0, 1, 0,
                0, 0.7, 0,
                0, 0, 0,
                0.4, 0.3, 0.6,
                0.7, 0.7, 0.7,
                0.7, 0.7, 1,
                1, 0.7, 0.7,
                0.7, 1, 0.7};

        static constexpr double e_mat[coef * prob] = {
                7.6, 7.6, 7.6,
                7.6, 7.6, 7.6,
                7.6, 7.6, 7.6,
                7.6, 7.6, 7.6,
                7.6, 7.6, 7.6,
                0.8, 0.4, 0.8,
                1.6, 0.4, 0.8,
                0.4, 0.4, 0.4,
                0.4, 0.8, 0.4,
                0.8, 0.8, 0.8,
                1.6, 1.6, 2.8,
                6.8, 6.8, 6.8,
                6.8, 6.8, 6.8,
                6.8, 6.8, 6.8,
                6.8, 6.8, 6.8};

        static constexpr double coefs[coef] =
            {4, 4, 4, 4, -12, 1, 3, -2, 5, -2, 1, -2, -2, -2, -2};
};

class court14: public regression {
    public:
        court14 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 1 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -5;
        static constexpr double minloc[prob] = {0.1, 0.8, 0.3};
        static constexpr double centres[coef * prob] = {0.1, 0.8, 0.3};
        static constexpr double e_mat[coef * prob] = {5, 5, 5};
        static constexpr double coefs[coef] = {-5};
};

class court15: public regression {
    public:
        court15 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 1 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -5;
        static constexpr double minloc[prob] = {0.1, 0.8, 0.3};
        static constexpr double centres[coef * prob] = {0.1, 0.8, 0.3};
        static constexpr double e_mat[coef * prob] = {7, 7, 7};
        static constexpr double coefs[coef] = {-5};
};

class court16: public regression {
    public:
        court16 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 4, coef = 2 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.84221700966;
        static constexpr double minloc[prob] = {0.1858, 0.6858, 0.1858, 0.4858};
        static constexpr double centres[coef * prob] =
            {0.3, 0.8, 0.3, 0.6, 0.4, 0.9, 0.4, 0.7};
        static constexpr double e_mat[coef * prob] = {5, 5, 5, 5, 5, 5, 5, 5};
        static constexpr double coefs[coef] = {-5, 5};
};

class court17: public regression {
    public:
        court17 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 7, coef = 3 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.47089199032;
        static constexpr double minloc[prob] = {
            0.3125, 0.9166, 0.3125, 0.7062, 0.0397, 0.9270, 0.5979};
        static constexpr double centres[coef * prob] = {
                0.3, 0.8, 0.3, 0.6, 0.2, 0.8, 0.5,
                0.8, 0.3, 0.8, 0.2, 0.5, 0.2, 0.8,
                0.2, 0.7, 0.2, 0.5, 0.4, 0.7, 0.3};

        static constexpr double e_mat[coef * prob] = {
            4, 4, 4, 4, 4, 4, 4,
             4, 4, 4, 4, 4, 4, 4,
             4, 4, 4, 4, 4, 4, 4};

        static constexpr double coefs[coef] = {-5, 5, 5};
};

class court18: public regression {
    public:
        court18 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 8, coef = 5 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -1.42906223657;
        static constexpr double minloc[prob] =
            {0.2677, 0.8696, 0.2677, 0.6594, 0.1322, 0.9543, 0.0577, 0.295};
        static constexpr double centres[coef * prob] = {
                0.3, 0.8, 0.3, 0.6, 0.2, 0.8, 0.2, 0.4,
                0.3, 0.8, 0.3, 0.6, 0.2, 0.8, 0.2, 0.4,
                0.3, 0.8, 0.3, 0.6, 0.2, 0.8, 0.2, 0.4,
                0.8, 0.3, 0.8, 0.2, 0.5, 0.2, 0.5, 0.7,
                0.2, 0.7, 0.2, 0.5, 0.4, 0.3, 0.8, 0.8};

        static constexpr double e_mat[coef * prob] = {
                0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                1, 1, 1, 1, 1, 1, 1, 1,
                4, 4, 4, 4, 4, 4, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4};

        static constexpr double coefs[coef] = {-1, 2, -5, 4, 4};
};

class court19: public regression {
    public:
        court19 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 2, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -8.67263950474;
        static constexpr double minloc[prob] = {0.4, 0.8};
        static constexpr double centres[coef * prob] = {
                0.1, 0.1,
                0.3, 0.8,
                0.6, 0.7,
                0.7, 0.1,
                0.4, 0.3,
                0.2, 0.8,
                0.1, 0.2,
                0.9, 0.4,
                0.5, 0.5,
                0, 0.8};

        static constexpr double e_mat[coef * prob] = {
                3. , 3. ,
                3. , 3. ,
                3. , 3. ,
                1.5, 1.5,
                3. , 3. ,
                9. , 9. ,
                1.5, 1.5,
                3. , 3. ,
                6. , 6. ,
                3. , 3. };

        static constexpr double coefs[coef] =
            { -5, 4,  -5,  5,  4,   2, -10,  -4,   5,   5};
};

class court20: public regression {
    public:
        court20 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 2, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -6.59763663216;
        static constexpr double minloc[prob] = {0.7, 0.1};
        static constexpr double centres[coef * prob] = {
                0.1, 0.1,
                0.3, 0.8,
                0.6, 0.7,
                0.7, 0.1,
                0.4, 0.3,
                0.2, 0.8,
                0.1, 0.2,
                0.9, 0.4,
                0.5, 0.5,
                0, 0.8};

        static constexpr double e_mat[coef * prob] = {
                50.,  50.,
                50.,  50.,
                50.,  50.,
                25.,  25.,
                50.,  50.,
               150., 150.,
                25.,  25.,
                50.,  50.,
               100., 100.,
                50.,  50.};

        static constexpr double coefs[coef] =
            {5, -4, 5, -7, -4, -2, 10, 4, -2, -5};
};

class court21: public regression {
    public:
        court21 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 4, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -7.74993665759;
        static constexpr double minloc[prob] = {0.9, 0.4, 0.9, 0.8};
        static constexpr double centres[coef * prob] = {
                0.1, 0.1, 0.1, 0.1,
                0.3, 0.8, 0.5, 0.2,
                0, 0.7, 0.4, 0.9,
                0.7, 0.1, 0.2, 0.8,
                0.4, 0.3, 0.6, 0.6,
                0.2, 0.8, 0.2, 0.6,
                0.9, 0.2, 0.3, 0.4,
                0.9, 0.4, 0.9, 0.8,
                0.5, 0.5, 0.5, 0.5,
                0, 0.8, 0, 0.2};

        static constexpr double e_mat[coef * prob] = {
                10., 10., 40., 40.,
                10., 10., 40., 40.,
                30., 30., 40., 40.,
                 5.,  5., 20., 20.,
                10., 10.,  5.,  2.,
                30., 30., 10., 10.,
                 5.,  5., 40., 20.,
                10., 10., 20., 30.,
                20., 20., 30., 40.,
                10., 10.,  5.,  5.};

        static constexpr double coefs[coef] =
            {5, -4, 5, -5, 4, -2, 10, -8, -2, -5};
};

class court22: public regression {
    public:
        court22 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 5, coef = 7 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -3.08088199150;
        static constexpr double minloc[prob] =
             {0.2723, 0.4390, 0.8277, 0.3390, 0.3695};
        static constexpr double centres[coef * prob] = {
                1, 0.3, 0.1, 0.4, 0.1,
                0.9, 0.7, 0, 0.5, 0.8,
                0.5, 0.6, 0.6, 0.5, 0.5,
                0.2, 0.2, 0.4, 0, 0.3,
                0, 0.6, 1, 0.1, 0.8,
                0.3, 0.5, 0.8, 0, 0.2,
                0.8, 1, 0.1, 0.1, 0.5};

        static constexpr double e_mat[coef * prob] = {
                 5, 30, 25,  5, 15,
                10, 30, 10,  5,  5,
                 5, 10,  5, 10,  5,
                20,  5, 20,  5,  5,
                25, 30,  5, 15, 10,
                20, 10, 15,  5, 20,
                15, 25,  5, 20, 25};

        static constexpr double coefs[coef] =
            {3, 4, -4, 2, -3, -2, 6};
};

// Skip 23 because of bessel function issue; see above (12)

class court24: public regression {
    public:
        court24 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 7, coef = 9 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -0.17296443752;
        static constexpr double minloc[prob] =
             {0.7, 0.1369, 0.6, 0.2, 0.5, 0.3, 0.2};
        static constexpr double centres[coef * prob] = {
                0, 0.4, 0, 0.3, 0.2, 0.3, 0.6,
                0.6, 0.8, 0.6, 0.7, 0.7, 0.1, 0.4,
                0.7, 0.7, 0, 0.5, 0, 0.6, 0.8,
                0.7, 0.5, 0.6, 0.2, 0.5, 0.3, 0.2,
                0.9, 0.3, 0.9, 0.8, 0.7, 1, 0,
                0.8, 0.1, 0.1, 0.2, 0.6, 0.1, 0.3,
                0.2, 0.7, 0.5, 0.5, 1, 0.7, 0.4,
                0.4, 0.1, 0.4, 0.1, 0.9, 0.2, 0.9,
                0.6, 0.9, 0.1, 0.4, 0.8, 0.7, 0.1};

        static constexpr double e_mat[coef * prob] = {
                0.2, 0.4, 0.4, 0.6, 1. , 0.4, 0.2,
                1. , 0.4, 0.6, 0.6, 0.8, 0.4, 0.8,
                1. , 0.8, 0.4, 0.2, 0.8, 0.2, 0.8,
                0.8, 0.2, 0.4, 1. , 0.2, 0.4, 1. ,
                0.4, 0.8, 0.8, 0.8, 1. , 1. , 0.6,
                0.2, 0.4, 1. , 0.4, 0.2, 0.8, 1.2,
                0.2, 1.2, 0.4, 0.2, 0.8, 1. , 1.2,
                0.2, 0.2, 1. , 0.2, 0.8, 1. , 1. ,
                0.6, 1. , 0.2, 0.6, 0.4, 1. , 0.8};

        static constexpr double coefs[coef] =
            {1, 2, 3, -4, 3, -2, -1, -2, 5};
};

class court25: public regression {
    public:
        court25 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 8, coef = 10 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -4.14042985928;
        static constexpr double minloc[prob] =
             {0.2, 0.6, 0.4, 0.8, 0.4, 0.3, 0.9, 0.8};
        static constexpr double centres[coef * prob] = {
                0.5, 0, 0.3, 0.5, 0.8, 0.3, 0.2, 1,
                0.6, 0.1, 0.6, 0.9, 0.2, 0, 0.5, 0.9,
                0.9, 0.9, 0, 1, 0.5, 1, 0.1, 0,
                0.2, 0.6, 0.4, 0.8, 0.4, 0.3, 0.9, 0.8,
                0.2, 0.8, 0.5, 0.1, 0.7, 0.2, 0.4, 0.8,
                0.2, 0.1, 0.7, 0.6, 0.2, 1, 0.6, 0.2,
                0.5, 0.8, 0.6, 0, 0.6, 0.3, 0.3, 0.2,
                0, 0, 0.2, 0.8, 0.9, 0.1, 0.1, 0.5,
                0.9, 0.9, 0.1, 0.3, 0.9, 0.8, 0.7, 0,
                0.3, 0.2, 0.9, 0.8, 0.9, 0.3, 0, 0.7};

        static constexpr double e_mat[coef * prob] = {
                25, 20, 20, 30, 20, 25, 15,  5,
                30, 30,  5, 25, 10, 25, 15, 10,
                10, 20, 25, 10, 15, 30, 25, 10,
                10,  5, 15, 10,  5,  5, 10, 20,
                20, 15, 30, 20,  5,  5, 25, 20,
                25,  5, 30,  5, 20, 30, 20, 30,
                25, 15, 15, 15,  5, 15, 20, 25,
                25, 20, 10, 25,  5, 25, 15, 25,
                30, 20, 10,  5,  5, 25, 25, 20,
                15, 15, 15, 15, 10, 25, 30,  5};

        static constexpr double coefs[coef] =
            {1, 2, 3, -5, 3, -2, -1, -2, 5, 2};
};

class court26: public regression {
    public:
        court26 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 11 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -1.55349754312;
        static constexpr double minloc[prob] = {0.5, 0.8, 0.3};
        static constexpr double centres[coef * prob] = {
                0.5, 0.2, 0,
                0.6, 0.2, 0.5,
                0.4, 0.6, 0.5,
                0.5, 0.7, 0.3,
                0.4, 0.4, 0.4,
                0.8, 0.5, 0.8,
                0, 0, 0.8,
                0.7, 0.7, 0.2,
                0.9, 0.3, 1,
                0.4, 0.4, 0.8,
                0.2, 0.8, 0.8};

        static constexpr double e_mat[coef * prob] = {
                1. , 1. , 1. ,
                3. , 2.5, 1.5,
                1.5, 1.5, 1.5,
                2.5, 1. , 2.5,
                2. , 3. , 1.5,
                1. , 1. , 1.5,
                1. , 2. , 0.5,
                2. , 3. , 2. ,
                0.5, 1.5, 2. ,
                1.5, 1. , 1. ,
                3. , 1. , 1.5};

        static constexpr double coefs[coef] =
            {1, 2, 3, -5, 3, -2, 1, -2, 5, 2, -2};
};

class court27: public regression {
    public:
        court27 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 3, coef = 11 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -1.76908456233;
        static constexpr double minloc[prob] = {0.6, 0.3, 0.5};
        static constexpr double centres[coef * prob] = {
                0.6, 0.3, 0.5,
                0.5, 0.2, 0,
                0.4, 0.6, 0.5,
                0.5, 0.7, 0.3,
                0.4, 0.4, 0.4,
                0.8, 0.5, 0.8,
                0, 0, 0.8,
                0.7, 0, 0.2,
                0.9, 0.3, 1,
                0.4, 0.4, 0.8,
                0.2, 0.8, 0.8};

        static constexpr double e_mat[coef * prob] = {
                2, 2, 2,
                6, 5, 3,
                3, 3, 3,
                5, 2, 5,
                4, 6, 3,
                2, 2, 3,
                2, 4, 1,
                4, 6, 4,
                1, 3, 4,
                3, 2, 2,
                6, 2, 3};

        static constexpr double coefs[coef] =
            {-10, 2, 3, 5, 3, 2, 1, 2, 5, 2, 2};
};

class court28: public regression {
    public:
        court28 ();
        void kernel (double *xs, double *ret) override;
        enum { prob = 4, coef = 11 };

    private:
        static constexpr rdims dims = { .prob = prob, .coef = coef };
        static constexpr double fmin = -7.69432628909;
        static constexpr double minloc[prob] =
           {0.4493, 0.0667, 0.9083, 0.2710};
        static constexpr double centres[coef * prob] = {
                0.6, 0.2, 0.8, 0.4,
                0.1, 0.1, 0.7, 0.9,
                1, 0.1, 0.8, 0.6,
                0, 0.3, 0.2, 1,
                0.2, 1, 0.8, 0,
                0.6, 0.9, 0.2, 0.9,
                0.1, 0.7, 0.6, 0.8,
                0.8, 0.4, 0.3, 0.2,
                0.1, 1, 0.8, 0.2,
                0.3, 0.9, 0.9, 0,
                0.8, 1, 0.6, 0.9};

        static constexpr double e_mat[coef * prob] = {
                1, 1, 1, 1,
                5, 3, 3, 3,
                4, 6, 2, 4,
                4, 1, 6, 3,
                2, 5, 3, 5,
                5, 4, 6, 1,
                6, 4, 1, 6,
                5, 1, 2, 1,
                1, 5, 4, 2,
                1, 3, 3, 2,
                4, 6, 6, 2};

        static constexpr double coefs[coef] =
            {-10, 2, 3, 5, 3, 2, 1, 2, 5, 2, 2};
};

/**
 * Michalewicz's function (2D) has the following formula
 * \f[
 * f(\mathbf{x}) = - \sum^2_{i=1}\sin(x_i)\left(
 * \sin\left(\frac{ix_i^2}{\pi}\right)
 * \right)^{20m}
 * \f]
 * which is subject to \f$0 \le x_1 \le \pi\f$. In this 2 dimensional version,
 * the global minimum is located at \f$\mathbf{x}^* = ( 2.202905513296628,
 * 1.570796322320509)\f$, with value \f$f(\mathbf{x}^*) =
 * -1.8013034100985499\f$.
 */
class michalewicz02: public synthetic {
    public:
        michalewicz02 ();
        double evaluate (inst::set x) override;
};

/**
 * Michalewicz's function (6D) has the following formula
 * \f[
 * f(\mathbf{x}) = - \sum^6_{i=1}\sin(x_i)\left(
 * \sin\left(\frac{ix_i^2}{\pi}\right)
 * \right)^{20m}
 * \f]
 * which is subject to \f$0 \le x_1 \le \pi\f$. In this 6 dimensional version,
 * the global minimum is located at \f$\mathbf{x}^* = ( 2.202905513296628,
 * 1.570796322320509, 1.284991564577549, 1.923058467505610, 1.720469766517768,
 * 1.570796319218113)\f$, with value \f$f(\mathbf{x}^*) = 5.687658179087978\f$.
 */
class michalewicz06: public synthetic {
    public:
        michalewicz06 ();
        double evaluate (inst::set x) override;
};

/**
 * Michalewicz's function (12D) has the following formula
 * \f[
 * f(\mathbf{x}) = - \sum^{12}_{i=1}\sin(x_i)\left(
 * \sin\left(\frac{ix_i^2}{\pi}\right)
 * \right)^{20m}
 * \f]
 * which is subject to \f$0 \le x_1 \le \pi\f$. In this 12 dimensional version,
 * the global minimum is located at \f$\mathbf{x}^* = ( 2.202905513296628,
 * 1.570796322320509, 1.284991564577549, 1.923058467505610, 1.720469766517768,
 * 1.570796319218113, 1.454413962081172, 1.756086513575824, 1.655717409323190,
 * 1.570796319387859, 1.497728796097675, 1.923739461688219,)\f$, with value
 * \f$f(\mathbf{x}^*) = -11.595826967415329\f$. */
class michalewicz12: public synthetic {
    public:
        michalewicz12 ();
        double evaluate (inst::set x) override;
};

/**
 * The Miele Cantrell function has the following formula
 * \f[
 * f(\mathbf{x}) = (e^{-x1} - x_2)^4 + 100(x_2 - x_3)^6 +
 * \big(\tan(x_3 - x_4)\big)^4 + x_1^8,
 * \f]
 * subject to \f$-1 \le x_i \le 1\f$, with the global minmum located at
 * \f$\mathbf{x}^* = (0,1,1,1)\f$ with value \f$f(\mathbf{x}^*) = 0\f$.
 */
class miele_cantrell: public synthetic {
    public:
        miele_cantrell();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 01 function has the following formula:
 * \f[
 * f(\mathbf{x}) = (1 + g_n)^{g_n},
 * \f]
 * where
 * \f[
 * g_n = n - \sum^{n-1}_{i=1}x_i
 * \f]
 * subject to \f$0 \le x_i \le 1\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (1, \ldots, 1)\f$ with value \f$f(\mathbf{x}^*) = 2\f$.
 */
class mishra01: public synthetic {
    public:
        mishra01(int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 02 function has the following formula:
 * \f[
 * f(\mathbf{x}) = (1 + g_n)^{g_n},
 * \f]
 * where
 * \f[
 * g_n = n - \sum^{n-1}_{i=1} \frac{x_i + x_{i+1}}{2}
 * \f]
 * subject to \f$0 \le x_i \le 1\f$ with a global minimum located at
 * \f$\mathbf{x}^* = (1, \ldots, 1)\f$ with value \f$f(\mathbf{x}^*) = 2\f$.
 */
class mishra02: public synthetic {
    public:
        mishra02(int dims);
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 03 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left\vert \cos\left(\sqrt{\vert x_1^2 +
 * x_2\vert}\right)\right\vert^0.5 + \frac{x_1 + x_2}{100},
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (-8.466613775046579,-9.998521308999999)\f$ with
 * value \f$f(\mathbf{x}^*) = -0.184651333342989\f$.
 */
class mishra03: public synthetic {
    public:
        mishra03();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 04 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left\vert \sin\left(\sqrt{\vert x_1^2 +
 * x_2^2\vert}\right)\right\vert^0.5 + \frac{x_1 + x_2}{100},
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (-9.941127263635860, -9.999571661999983)\f$ with
 * value \f$f(\mathbf{x}^*) = -0.199406970088833\f$.
 */
class mishra04: public synthetic {
    public:
        mishra04();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 05 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left[
 * \sin^2\bigg(\big(\cos(x1) + \cos(x2)\big)^2\bigg) +
 * \cos^2\bigg(\big(\sin(x1) + \sin(x2)\big)^2\bigg) +
 * 0.01x_1 + 0.1x_2
 * \right]^2
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (-1.986820662153768, -10)\f$ with
 * value \f$f(\mathbf{x}^*) = - 1.019829519930943 \f$.
 */
class mishra05: public synthetic {
    public:
        mishra05();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 06 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\log \left[ (
 * \sin^2\bigg(\big(\cos(x_1) + \cos(x_2)\big)^2\bigg) -
 * \cos^2\bigg(\big(\sin(x_1) + \sin(x_2)\big)^2\bigg) +
 * x1 )^2
 * \right] +
 * 0.1\left((x_1 - 1)^2 + (x_2 - 1)^2\right)
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (2.886307215440481, 1.823260331422321)\f$ with
 * value \f$f(\mathbf{x}^*) = -2.283949838474759\f$.
 */
class mishra06: public synthetic {
    public:
        mishra06();
        double evaluate (inst::set x) override;
};

// We exclude the mishra 07 (factorial function) for reasons of computational
// unwieldiness; it is more of a computational hassle than useful or
// challenging function to optimise over.

/**
 * The Mishra 08 (decanomial) function has the following formula:
 * \f[
 * f(\mathbf{x}) = \frac{1}{1000}(\vert g(x_1)\vert + \vert h(x_2)\vert)^2,
 * \f]
 * where
 * \f[
 * \begin{align*}
 * g(x_1) &= x_1^{10} - 20x_1^9 + 180x_2^8 - 960x_1^7 + 3360x_1^6 - 8064x_1^5 +
 * 11340x_1^4 - 15360x_1^3 + 11520x_1^2 - 5120x_1 + 2624 \\ \end{align*}
 * h(x_2) = x_2^4 + 12x_2^3 + 54x_2^2 + 108x_2 + 81,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (2, -3)\f$ with
 * value \f$f(\mathbf{x}^*) = 0\f$.
 */
class mishra08: public synthetic {
    public:
        mishra08();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 09 (decanomial) function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left(ab^2c + abc^2 + b^2 + (x_1 + x_2 - x_3)^2 \right)^2
 * \f]
 * where
 * \f[
 * \begin{align*}
 * a &= 2x_1^3 + 5x_1x_2 + 4x_3 - 2x_1^2x_3 - 18 \\
 * b &= x_1 + x_2^3 + x_1x_3^2 - 22 \\
 * c &= 8x_1^2 + 2x_3x_3 + 2x_2^2 + 3x_2^3 - 52,
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2,3\f$ with a global minimum
 * located at \f$\mathbf{x}^* = (1,2,3)\f$ with
 * value \f$f(\mathbf{x}^*) = 0\f$.
 */
class mishra09: public synthetic {
    public:
        mishra09();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 10 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left( \lfloor x_1 \perp x_2 \rfloor - \lfloor x_1 \rfloor -
 * \lfloor x_2 \rfloor \right)^2
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for \f$i = 1,2\f$ with two global minima
 * located at \f$\mathbf{x}^* = \{(0,0), (2,2)\f}\f$ with value
 * \f$(\mathbf{x}^*) = 0\f$.
 */
class mishra10: public synthetic {
    public:
        mishra10();
        double evaluate (inst::set x) override;
};

/**
 * The Mishra 11 function has the following formula:
 * \f[
 * f(\mathbf{x}) = \left(
 * \frac{1}{n}\sum^n_{i=1}\vert x_i \vert - \left(\prod^n_{i=1}\vert
 * x_i\vert\right^{\frac{1}{N}} \right)^2
 * \f],
 * subject to \f$0 \le x_i \le 10\f$ for \f$i = 1,2, \ldots n\f$. There are an
 * infinite number of global minima located at \f$\mathbf{x} = (x, \ldots,
 * x)\f$ (that is, \f$x_1 = x_2 = \cdots = x_n\f$, with value
 * \f$f(\mathbf{x}^*) = 0\f$.
 */
class mishra11: public synthetic {
    public:
        mishra11(int dims);
        double evaluate (inst::set x) override;
};

/**
 * The ManifoldMin function has the following formula:
 * \f[
 * f(\mathbf{x}) = \sum^{n}_{i=1}\vert x_i \vert * \prod^{n}_{i=1} \vert x_i
 * \vert.
 * \f]
 * subject to \f$-10 \le x_i \le 10\f$ for $\f$i = 1, \ldots, n\f$. The global
 * minimum is found at \f$\mathbf{x} = (0, \ldots, 0)\f$ with value
 * \f$f(\mathbf{x}) = 0\f$.
 */
class manifoldmin: public synthetic {
    public:
        manifoldmin(int dims);
        double evaluate (inst::set x) override;
};

/**
 * The MixtureOfGaussians01 function has the following formula:
 * \f[
 * f(\mathbf{x}) = -\Bigg(
 * \frac{1}{2} \exp \Big(-10 \big(0.8 (x_1 + 0.2)^2 + 0.7 (x_2 + 0.5)^2\big)\Big) +
 * \frac{1}{2} \exp \Big(-8  \big(0.3 (x_1 - 0.8)^2 + 0.6 (x_2 - 0.3)^2\big)\Big)
 * \Bigg)
 * \f]
 */
class mog01: public synthetic {
    public:
        mog01 ();
        double evaluate (inst::set x) override;
};

// pick up from f85. in jamil et al
// from l.3035 in evalset:test_funcs
// https://al-roomi.org/ for reference

} // end namespace syn

#endif // __SYNTHETIC_H_
