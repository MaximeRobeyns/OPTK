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

#include <optimisers/gridsearch.hpp>

#include <tests/optimiser_test.hpp>
#include <tests/testutils.hpp>
#include <benchmarks/synthetic.hpp>

void
test_gridsearch_functionality ()
{
    // instantiate an example benchmark (alpine1)
    syn::alpine1 a1(3);

    // instantiate the gridsearch algorithm
    gridsearch test = gridsearch ();

    // setup the search space
    test.update_search_space_s(a1.get_search_space (), 0.5);

    inst::set params = NULL;
    uint idx = 0;
    double best = std::numeric_limits<double>::max ();

    do {
        params = test.generate_parameters (idx++);
        if (!params) break;
        double tmp = a1.evaluate (params);
        if (best > tmp) best = tmp;
        test.receive_trial_results (idx-1, params, tmp);
    } while (params != NULL);

    assert (tutils::dbleq (best, 0.));
}

void
run_gridsearch_tests()
{
    run_static_gridsearch_tests ();
    test_update_search_space ();

    test_gridsearch_functionality  ();
    std::cout << "All gridsearch tests pass" << std::endl;
}

