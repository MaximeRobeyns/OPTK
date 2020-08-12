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
 * @brief This file implements a simple gridsearch algorithm.
 */

#include <optimisers/gridsearch.hpp>
#include <nlohmann/json.hpp>

gridsearch::gridsearch (): optimiser ("gridsearch")
{
    m_params = optk::plist();
    // store this vector on the stack
    pvals = std::vector<std::tuple<std::string, std::vector<double>>>();
}

void
gridsearch::unpack_param (optk::param_t *param)
{
    // 1. Assert that the parameter is of the correct type for gridsearch
    param::param_type t = param->get_type();
    assert (
        t == param::randint ||
        t == param::quniform ||
        t == param::categorical ||
        t == param::choice
    );

    // 2. If it's a choice type, recursively call unpack_param on each choice
    if (t == param::choice) {
        optk::choice *cparam = static_cast<optk::choice *>(param);
        optk::sspace_t *opts = cparam->options();
        optk::sspace_t::iterator it;
        for (it = opts->begin(); it != opts->end(); it++) {
            gridsearch::unpack_param(*it);
        }
        return;
    }

    // for values in param1:
    //     for values in param2:
    //         for values in param3:
    //             evaluate(param1, param2, param3);
    //             
    // if param 2 is not relevant at this point, then many useless evaluations
    // will be calculated. Therefore it should take on a single
    // (null/irrelevant) value when it is not applicable.
    //
    // We need a method of determining when a parameter is and is not relevant.
    // 
    // Observation1: The values specified in a choice may only become relevant
    // when the appropriate parent choice is selected (takes on a single value).
    //
    // At the very least, we must enumerate all the possible values that an
    // individual parameter may take on, irrespective of its dependencies to
    // others.
        
    // 3. Count the cardinality of the sets of values that each parameter may take on.
    // enumerate the possible values of the parameter in all other cases
    // append the name of the parameter and the cardinality of the space to the
    // m_params vector.

    switch (t) {
        case param::randint:
        {
            optk::randint *ri = static_cast<optk::randint *>(param);
            for (int i = ri->m_lower; i < ri->m_upper; ri++) {
                values.push_back((double) i);
            }
            // in this case, simply arange integers between the two bounds.
            // TODO modify the types to retain the bound information that they
            // are passed.
            break;
        }
        case param::quniform:
        {
            // just sample uniformly from low to high; not too dissimilar to the
            // above
            break;
        }
        case param::categorical:
        {
            // the work has already been done for you for the most part;
            // If the type cannot be directly cast to a double value; then we
            // have to enumerate it (e.g. a string).
            break;
        }
        default:
            // I can't see how it is possible to get to this point, so it is not
            // handled.
            break;
    };
}

void
gridsearch::update_search_space (optk::sspace_t *space)
{

    // first validate the search space:

    optk::sspace_t::iterator it;
    for (it = space->begin(); it != space->end(); it++) {
        gridsearch::unpack_param(*it);
    }


    optk::sspace_t::iterator it;
    for (it = space->begin(); it != space->end(); it++) {
        // gridsearch::unpack_param(*it);
    }
    // initially create a list of lists containing the enumerated values of

    // save a copy of the search space (sspace_t is just a list of pointers).
    m_space = *space;
}

optk::plist
gridsearch::generate_parameters (int param_id)
{
    return optk::plist();
}


void
gridsearch::receive_trial_results (
    int param_id, optk::plist params, double value
){
    return;
};
