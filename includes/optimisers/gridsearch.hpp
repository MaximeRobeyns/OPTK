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
 * @brief Header file for the gridsearch built-in optimiser.
 */

#ifndef __GRIDSEARCH_H_
#define __GRIDSEARCH_H_

#include <unordered_map>
#include <vector>
#include <tuple>

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

namespace __gs {

enum class pspace_t: char {
    node,
    int_val,
    dbl_val,
    str_val
};

class param {
    public:
        param (const std::string &k, pspace_t t);
        pspace_t get_type () { return m_type; }
        std::string get_key () { return m_key; }

    private:
        const std::string m_key;
        const pspace_t m_type;
};

// classes inheriting __gs::param are declared / defined in the gridsearch.cpp

} // end namespace gs

class gridsearch: public optk::optimiser {

    public:

        /**
         * The constructor; simply calls the constructor specified for the
         * \ref optimiser class which this class inherits.
         */
        gridsearch ();

        /**
         * The destructor; used to recursively free elements stored on the heap.
         */
        ~gridsearch ();

        /**
         * Expands the lists of parameters into a format from which it is easier
         * to generate all possible configurations sequentially.
         *
         * @param space The search space to unpack. The gridsearch optimiser
         * only accepts parameters of type param::choice, param::categorical,
         * param::randint as well as param::quniform.
         */
        void update_search_space (sspace::sspace_t *space);

        /**
         * Converts the search space for synthetic benchmarks (defined as
         * uniformly distributed variables) into sspace::quniform parameters.
         * @param ss The original search space.
         * @param q The quantisation for uniform parameters.
         * @returns A search space made up of quniform parameters.
         */
        sspace::sspace_t *convert_synthetic_ss (sspace::sspace_t *ss, double q);

        /**
         * A convenience method for accepting a search space from a synthetic
         * benchmark. This is equivalent to calling
         * @code{.cpp}
         * sspace::sspace_t *ss =
         *      gridsearch.convert_synthetic_ss (
         *          benchmark.get_search_space(), q
         *      );
         * @endcode
         * @param space The search space returned from synthetic::get_search_space.
         * @param q The quantisation for uniform parameters.
         */
        void update_search_space_s (sspace::sspace_t *space, double q);

        /**
         * Returns the next unique parameter configuration.
         * @param param_id The identifier which will be matched with the
         * selected parameter combination.
         */
        inst::set generate_parameters (int param_id);

        /**
         * Stores \c params if \c value is greater than previous best.
         *
         * @param param_id The identifier of the parameter combination which
         * generated the result.
         * @param params The parameters themselves
         * @param params The result from the objective function
         */
        void receive_trial_results (
            int param_id, inst::set params, double value
        );

#ifdef __OPTK_TESTING
        __gs::param *get_root () {
            return m_root;
        }
#endif

    private:
        /** This is a reference to the 'root' of the 'unpacked' (internal)
         * representation of the search space. */
        __gs::param *m_root;

        // TODO begin delete
        /**
         * This will add a parameter configuration allocated on the heap to the
         * gridsearch::trials map, so that it may be freed later by gridsearch's
         * destructor. If a trial configuration already exists with the same param_id,
         * those values will be freed, and the value in the map will be replaced.
         * @param param_id The identifier for this trial.
         * @param n The root node of the concrete parameter settings.
         */
        // void add_to_trials (int param_id, inst::node *n);

        /**
         * This is used to keep track of previously generated instances or
         * settings of the parameters, identified by the parameter id passed to
         * the generate_parameters method.
         * Contains elements stored on the heap.
         */
        // std::unordered_map<int, inst::set> trials;
        // TODO end delete

        /** Indicates whether this is the first call to generate_parameters */
        bool fst_gen;

        /**
         * A list of pointers to converted synthetic benchmark search spaces
         * allocated through convert_synthetic_ss; used in destructor to avoid
         * memory leaks.
         */
        std::vector<sspace::sspace_t *> m_syn_spaces;
};

#ifdef __OPTK_TESTING
#include <assert.h>
#include <tests/testutils.hpp>

void test_update_search_space();
#endif // __OPTK_TESTING

#endif // __GRIDSEARCH_H_
