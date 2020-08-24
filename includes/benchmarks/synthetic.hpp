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

// TODO get rid of this
#include <iostream>

#include <optk/benchmark.hpp>
#include <optk/types.hpp>
#include <sys/types.h>

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
 * \frac{3}{4} \exp \left(-\frac{(9x_1 + 1)^2}{49} - \frac{(9x_2 + 1)^2}{10}
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


} // end namespace syn

#endif // __SYNTHETIC_H_
