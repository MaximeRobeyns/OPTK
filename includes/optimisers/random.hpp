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
 * @brief Header file for the random search built-in optimiser.
 */

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

// class random_search: public optk::optimiser {
//
//     void update_search_space (sspace::sspace_t *space) override;
//
//     /**
//      * The random search optimiser will simply generate the relevant parameters
//      * by calling the \c sample method of parameters in the search space.
//      * @param param_id A largely disregarded parameter; save for internal
//      * memory management.
//      * @returns A randomly generated set of valid parameters, with no
//      * guarantees of uniqueness.
//      */
//     inst::set generate_parameters (int param_id) override;
//
//     void receive_trial_results (
//             int param_id,
//             inst::set params,
//             double value
//         ) override;
//
// };
