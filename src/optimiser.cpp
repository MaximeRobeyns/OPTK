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
 * @brief This file implements the base class for optimisers.
 */

#include "optk/types.hpp"
#include <optk/optimiser.hpp>

optk::optimiser::optimiser (std::string name)
{
    m_name = name;
    stepidx = 0;
}

optk::optimiser::~optimiser ()
{
    // delete any parameter instances which were not deleted by calls to
    // receive_trials_results.
    std::unordered_map<int, inst::set>::iterator it;
    for (it = trials.begin (); it != trials.end (); it++)
        inst::free_node (std::get<1>(*it));
}

optk::optimisers::optimisers ()
{
    m_arr = std::vector<optimiser *>();
}

void
optk::optimisers::register_optimiser (optimiser *o)
{
    m_arr.push_back (o);
}

std::vector <optk::optimiser *> *
optk::optimisers::collection ()
{
    return &m_arr;
}

void
optk::optimiser::accept (optimisers *o)
{
    o->register_optimiser (this);
}

bool
optk::optimiser::step (optk::benchmark *b)
{
    inst::set params = this->generate_parameters (stepidx++);
    if (params == NULL)
        return false;
    double result = b->evaluate (params);
    this->receive_trial_results (stepidx-1, params, result);
    return true;
}

void
optk::optimiser::add_to_trials (int param_id, inst::set n)
{
    if (trials.count (param_id))
        inst::free_node (trials.at (param_id));
    trials.emplace (param_id, n);
}
