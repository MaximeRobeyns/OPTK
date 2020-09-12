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
 * @brief This file implements a Gaussian process optimiser.
 */

#include <optimisers/gp.hpp>

gp_opt::gp_opt ():
    optk::optimiser ("gp optimiser")
{ }

void
gp_opt::update_search_space (sspace::sspace_t *space)
{
    // validate the search space
    m_space = space;
}

inst::set
gp_opt::generate_parameters (int param_id)
{
    inst::node *root = new inst::node ("gp parameters");

    // TODO implement me

    add_to_trials (param_id, root);

    return root;
}

void
gp_opt::receive_trial_results (int pid, inst::set params, double value)
{
    free_node (params);
    trials.erase(pid);
    return;
}

