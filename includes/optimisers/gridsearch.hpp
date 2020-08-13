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

class pspace;

namespace gs {
/**
 * Param is a name-values tuple.
 */
typedef std::tuple<std::string, std::vector<double>> param;

/**
 * Somewhat similar to optk::plist, params is a list of (name, values)
 * tuples, which represents all parameters, their names and
 * corresponding values.
 */
typedef std::vector<param> params;

/**
 * Used to represent a subspce in a pspace.
 */
typedef std::vector<pspace *> subspaces;

} // end namespace gs

class pspace {
    public:

        pspace ();

        /**
         * The constructor
         */
        pspace (std::string name);

        /**
         * The step function is used to step through a search space.
         * @todo make the requirements for this function more concrete before
         * going further
         */
        // bool step ();

        /**
         * registers the enumerated values for a parameter.
         * @param p pointer to the parameter whose values have been enumerated
         */
        void register_param (gs::param *p);

        /**
         * registers a nested search space to add to the \c subspaces member
         * @param subspace Pointer to the nested subspace
         */
        void register_subspace (pspace *subspace);

        /**
         * Get the parameter list; used during testing
         */
        gs::params *get_paramlist ();

        /**
         * Get the subspaces; used during testing
         */
        gs::subspaces *get_subspaces ();

        /**
         * Access this parameter space's name
         */
        std::string get_name ();

    private:
        
        /**
         * The list of concrete parameters for this 'level' of the search space
         */
        gs::params paramlist;

        /**
         * The list of nested search spaces on this 'level'
         */
        gs::subspaces subspaces;

        /**
         * The name corresponding to the (nested) search space. The top-level
         * search space has name "root".
         */
        std::string m_name;
};

class gridsearch: public optk::optimiser {

    public:

        /**
         * The constructor; simply calls the constructor specified for the
         * \ref optimiser class which this class inherits.
         */
        gridsearch ();

        /**
         * Expands the lists of parameters into a format from whcih it is easier
         * to generate all possible configurations sequentially.
         * 
         * @param space The search space to unpack. The gridsearch optimiser
         * only accepts parameters of type param::choice, param::categorical,
         * param::randint as well as param::quniform.
         */
        void update_search_space (optk::sspace_t *space);

        /**
         * Returns the next unique parameter configuration.
         * @param param_id The identifier which will be matched with the
         * selected parameter combination.
         */
        param::list generate_parameters (int param_id);

        /**
         * Stores \c params if \c value is greater than previous best.
         * 
         * @param param_id The identifier of the parameter combination which
         * generated the result.
         * @param params The parameters themselves
         * @param params The result from the objective function
         */
        void receive_trial_results (
            int param_id, param::list params, double value
        );

# ifdef __OPTK_TESTING
        pspace * get_root () {
            return &m_root;
        }
#endif

    private:
        /**
         * Recursively expands the parameters into a connected acyclic graph
         * which is a representation that gridsearch can more easily iterate
         * over.
         * 
         * @param param The parameter to expand.
         * @param space The parameter space (level) in which param belongs
         */
        void unpack_param (optk::param_t *param, pspace *space);

        /**
         * reference to the 'root' search space; the first node in the graph
         */
        pspace m_root;
};

#endif // __GRIDSEARCH_H_
