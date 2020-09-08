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
 * @brief Contains core logic of the program
 */

#include <optk/core.hpp>

#include <iostream>

namespace optk {

void
core_loop(
        optk::benchmark *bench,
        optk::optimiser *opt,
        double *trace,
        uint max_iter
) {
    // clear the trace memory region
    memset(trace, 0, sizeof (double) * max_iter);

    sspace::sspace_t *ss = bench->get_search_space();

    opt->update_search_space(ss);

    inst::set params = NULL;
    uint idx = 0;

    do {
        params = opt->generate_parameters (idx);
        // std::cout << "after genp" << std::endl;
        if (params == NULL)
            break;
        double res = bench->evaluate (params);
        opt->receive_trial_results (idx, params, res);
        trace[idx++] = res;
    } while (params != NULL && idx < max_iter);

    opt->clear();

    return;
}

} // namespace optk
