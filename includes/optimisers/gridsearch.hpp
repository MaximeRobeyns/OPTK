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
#include <vector>

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

#include <optk/types.hpp>
#include <optk/optimiser.hpp>

class gridsearch: public optk::optimiser {
    public:

        /**
         * The constructor; simply calls the constructor specified for the
         * \ref optimiser class which this class inherits.
         */
        gridsearch ();

        typedef std::vector<std::vector<std::vector<double>>> pvals;

        /**
         * In this function, gridsearch expands all the parameters so as to
         * enumerate all the possible combinations. These are matched to
         * parameter ids from individual trials, allowing results to be assigned
         * back to parameter combinations.
         * 
         * @param space The search space to unpack. The gridsearch optimiser
         * only accepts parameters of type param::choice, param::categorical,
         * param::randint as well as param::quniform.
         */
        void update_search_space (optk::sspace_t *space);

        /**
         * Selects the next un-evaluated parameter combination and returns that.
         * @param param_id The identifier which will be matched with the
         * selected parameter combination.
         */
        optk::plist generate_parameters (int param_id);

        /**
         * Since gridsearch is not an 'active learning' approach, this function
         * simply does nothing but store the parameter combination if the value
         * is greater than the current maximum.
         * 
         * @param param_id The identifier of the parameter combination which
         * generated the result.
         * @param params The parameters themselves
         * @param params The result from the objective function
         */
        void receive_trial_results (
            int param_id, optk::plist params, double value
        );

    private:

        /**
         * This recursively enumerates the possibilities for each parameter in
         * the search space.
         * 
         * Recursion only occurs for parameters of type param::choice, whose
         * values are themselves parameters; this implements nested or
         * conditional search spaces.
         * 
         * We consider the parameter space as a connected graph with no cycles
         * (i.e. a tree), where param::choice elements represent nodes, and all
         * other compatible types represent leaves. This function begins by
         * enumerating all the choices of the leaves, and storing these results
         * in a vector of vectors of doubles.
         */
        void unpack_param (optk::param_t *param);

        /**
         * This matches the names of the paameters with the cardinality of their
         * search sets. The index into this array corresponding to a given
         * parameter becomes that parameter's numeric id.
         *
         * Note that the optk::plist type is not used in this instance as it is
         * used elsewhere.
         * TODO get rid of this
         */
        optk::plist m_params;

        /**
         * pvals is used to store a list of (parameter, values) pairs, which
         * gives the parameter name, as well as an array of doubles to hold the
         * enumerated values which that parameter can take on. This only applies
         * to non-choice parameters.
         */
        std::vector<std::tuple<std::string, std::vector<double>>> pvals;

        /**
         * This lists out all the values that the parameters can take in
         * the search space in a 2D matrix.
         * 
         * TODO get rid of this
         */
        std::vector<std::vector<double>> param_list;

        /**
         * This is a copy of the original search space. One reason for keeping
         * it is to have a reference to strings which may form a categorical type.
         */
        optk::sspace_t m_space;
};

#endif // __GRIDSEARCH_H_
