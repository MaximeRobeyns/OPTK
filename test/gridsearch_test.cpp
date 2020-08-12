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
 * @brief Implements tests for the gridsearch optimisation algorithm.
 */

#include <iostream>
#include <vector>

#include <optk/types.hpp>

#include <optimisers/gridsearch.hpp>

#include <tests/optimiser_test.hpp>

static void
test_update_search_space ()
{
    gridsearch test = gridsearch();

    optk::sspace_t testspace;

    optk::randint tri("testrandint", 0, 10);
    optk::quniform tqu("testquniform", 0, 10, 1);
    std::vector<int> opts = {0,1,2,3,4};
    optk::categorical<int> cat("testcat", &opts);

    testspace.push_back(&tri);
    testspace.push_back(&tqu);
    testspace.push_back(&cat);

    test.update_search_space(&testspace);
}

void
run_gridsearch_tests()
{
    test_update_search_space ();
    std::cout << "All gridsearch tests pass" << std::endl;
}

