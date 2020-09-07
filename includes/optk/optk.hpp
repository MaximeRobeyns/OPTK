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
 * @brief This file contains the main entrypoint to the program.
 */

#ifndef __OPTK_H_
#define __OPTK_H_

#include <argp.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include <optk/types.hpp>
#include <optk/benchmark.hpp>
#include <optk/optimiser.hpp>

// optimisers
#include <optimisers/gridsearch.hpp>
#include <optimisers/random.hpp>

// benchmarks
#include <benchmarks/synthetic.hpp>

#ifdef __OPTK_TESTING
#include <tests/tests.hpp>
#endif

/**
 * @namespace
 * optk is the main namespace for this program.
 */
namespace optk {

// program arguments
typedef struct {
    /** The number of threads to run the program with                        */
    int threads;
    /** The number of iterations to run per benchmark                        */
    int max_iters;
    /** The directory into which the output file(s) should go                */
    const char *output;
    /** The benchmarks to run                                                */
    const char *benchmark;
    /** The optimisation algorithm to evaluate                               */
    const char *algorithm;
} arguments;

}

#endif // __OPTK_H_
