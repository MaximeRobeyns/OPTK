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
 * @brief This file contains definitions for 'core' logic
 */

#ifndef __CORE_H_
#define __CORE_H_

#include <cstring>

#include <optk/types.hpp>
#include <optk/benchmark.hpp>
#include <optk/optimiser.hpp>

namespace optk {

/**
 * This performs the core optimisation loop. Given a benchmark and an
 * optimiser, this function will first commuicate the benchmark's search space,
 * and then iterate for up to max_iter iterations, generating new
 * configurations, evaluating, and storing the results.
 * @param bench A pointer to the benchmark to be run
 * @param opt A pointer to the optimiser to run on the benchmark
 * @param max_iter The maximum number of iterations; default 20
 */
void
core_loop (
        optk::benchmark *bench,
        optk::optimiser *opt,
        double *trace,
        uint max_iter=20
        );
}

#endif // __CORE_H_
