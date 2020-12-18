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
 * @brief Header file for the Gaussian process built-in optimiser.
 */

#ifndef __GP_H_
#define __GP_H_

#include <stdexcept>

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

/**
 * gp_opt is a generic Gaussian process-based optimiser which is only
 * implemented to provide a baseline against other methods.
 */
class gp_opt: public optk::optimiser {

    public:

        gp_opt();

        /**
         * The GP optimiser is only compatible with continuous valued inputs
         * (for the moment); conceretely an error will be raised if the input
         * is not pt::uniform, pt::loguniform, pt::normal, pt::lognormal.
         * Currently all parameter types are treated as pt::uniform, although
         * in the future information about parameters' distributions may be
         * taken into account.
         * @param space The new search space.
         */
        void update_search_space (sspace::sspace_t *space) override;

        inst::set generate_parameters (int param_id) override;

        void receive_trial_results (
                int param_id,
                inst::set params,
                double value
                ) override;

    private:

        /** Counts the number of iterations performed */
        uint n_iters;

        /** A copy of the problem search space */
        sspace::sspace_t *m_space;

};

#endif // _GP_H__
