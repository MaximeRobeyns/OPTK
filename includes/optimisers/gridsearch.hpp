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

#include <assert.h>
#include <iostream>
// TODO get rid of this
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
         * Expands the lists of parameters into a format from whcih it is easier
         * to generate all possible configurations sequentially.
         *
         * @param space The search space to unpack. The gridsearch optimiser
         * only accepts parameters of type param::choice, param::categorical,
         * param::randint as well as param::quniform.
         */
        void update_search_space (sspace::sspace_t *space);

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

        /**
         * This will add a parameter configuration allocated on the heap to the
         * gridsearch::trials map, so that it may be freed later by gridsearch's
         * destructor. If a trial configuration already exists with the same param_id,
         * those values will be freed, and the value in the map will be replaced.
         * @param param_id The identifier for this trial.
         * @param n The root node of the concrete parameter settings.
         */
        void add_to_trials (int param_id, inst::node *n);

        /**
         * This is used to keep track of previously generated instances or
         * settings of the parameters, identified by the parameter id passed to
         * the generate_parameters method.
         * Contains elements stored on the heap.
         */
        std::unordered_map<int, inst::set> trials;

};

#ifdef __OPTK_TESTING
#include <assert.h>

void test_update_search_space();
#endif // __OPTK_TESTING

#endif // __GRIDSEARCH_H_
