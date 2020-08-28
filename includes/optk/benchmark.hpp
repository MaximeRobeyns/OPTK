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
 * @brief This file defines the base class for benchmarks.
 */

#ifndef __BENCHMARK_H_
#define __BENCHMARK_H_

#include <string>

#include <optk/types.hpp>

namespace optk {

/**
 * This is base class which every benchmark inherits.
 */
class benchmark {
    public:
        /**
         * A minimal constructor for a benchmark
         * @param name The name of this particular benchmark.
         */
        benchmark (const std::string &name);

        /**
         * @returns The name of the benchmark
         */
        std::string get_name();

        /**
         * Get the search (parameter) space specified by the benchmark.
         *
         * @returns A pointer to the parameter space.
         */
        virtual sspace::sspace_t *get_search_space () = 0;

        /**
         * Evaluate the benchmark (i.e. calculate the value of the objective
         * fuinction) on the provided values.
         *
         * @param x A vector of double-precision values to evaluate the
         * benchmark at.
         *
         * @todo Verify that a vector of doubles does not constrict the range of
         * benchmark functions which may be evaluated (i.e. as opposed to
         * categorical variables such as strings).
         */
        virtual double evaluate (inst::set x) = 0;

    protected:
        std::string m_name; /** The benchmark's name */
};

/**
 * This represents a collection of benchmarks; either all those known to OPTK,
 * or a subset of those such as those selected by a user from command line
 * arguments.
 */
class benchmarks {
    public:
        /**
         * The constructor
         */
        benchmarks ();

        /**
         * The reigster function allows you to tell the main runner about the
         * existence of a benchmark.
         *
         * @param b The benchmark to register
         */
        void register_benchmark (benchmark *b);

        /**
         * @returns The collection of benchmarks held by this class
         */
        std::vector <benchmark *> * collection();

        /**
         * @todo include an iterator method such as 'map' or a wrapper around a
         * for loop to allow the runner to 'apply' an optimisation algorithm to
         * each benchmark in the collection.
         */

    private:
        std::vector <benchmark *> m_arr; /** The array of benchmarks in this collection */
};

} // namespace optk

#endif // __BENCHMARK_H_
