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
 * @brief This file defines the base class for optimisers.
 */

#ifndef __OPTIMISER_H_
#define __OPTIMISER_H_

#include <optk/types.hpp>

namespace optk {

// forward declaration of optimisers class
class optimisers;

/**
 * This is the base class that every optimisation algorithm will inherit.
 */
class optimiser {

    public:
        /**
         * The constructor
         * @param name The identifiying name of this optimisation algorithm.
         */
        optimiser (std::string name);

        /**
         * Uses the visitor pattern to register an optimiser with the optimisers
         * class.
         * @param o A pointer to the optimisers class with which to register
         * this optimiser.
         */
        void accept (optimisers *o);

        /**
         * This updates the search space  the search space defined by the optimiser
         * @param space The new search space to use
         */
        virtual void update_search_space (sspace::sspace_t *space) {}

        /**
         * This is the method invoked to generate the next trial's parameters
         * for the benchmark or program being optimised.
         * @param param_id An identifier for this parameter trial; this may or
         * may not be used by the optimistaion algorithm.
         * @returns A pointer to an inst::node, or a NULL pointer if there are
         * no more parameters to return.
         */
        virtual inst::set generate_parameters (int param_id) {
            return inst::set();
        }

        /**
         * Allows the benchmark or program being optimised to communicate back
         * the value of the cost function for a parameter set.
         *
         * @param param_id This parameter id is used to identify the parameters
         * used to generate the result.
         * @param parameters For increased flexibility, the actual parameters
         * used to generate the result are also copied back.
         * @param value The actual value of the objective function.
         */
        virtual void receive_trial_results (
            int param_id,
            inst::set params,
            double value
        ) {}

    private:
        std::string m_name;
};

/**
 * This is a class to hold the list of optimisation algorithms known to OPTK.
 */
class optimisers {
    public:
        /**
         * The constructor
         */
        optimisers();

        /**
         * The register function allows you to tell the main runner about the
         * existance of a benchmark.
         */
        void register_optimiser (optimiser *o);

        /**
         * @returns The collection of optimisation algorithms held by this class
         */
        std::vector <optimiser *> *collection();

    private:
        std::vector <optimiser *> m_arr;
};


} // namespace optk

#endif // __OPTIMISER_H_