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

static bool
dbleq (double a, double b)
{
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

static void
test_update_search_space ()
{
    gridsearch test = gridsearch();

    optk::sspace_t testspace;

    optk::randint tri("testrandint", 0, 10);
    optk::quniform tqu("testquniform", 0, 10, 1);
    std::vector<double> opts = {0,1,2,3,4};
    optk::categorical<double> cat("testcat", &opts);

    optk::randint c1("choice1", 0,5);
    optk::randint c2("choice2", 5,10);
    optk::quniform c3("choice3", 5, 15, 3);
    optk::sspace_t copts = {&c1, &c2, &c3};
    optk::choice choice("testchoice", &copts);

    testspace.push_back(&tri);
    testspace.push_back(&tqu);
    testspace.push_back(&choice);
    testspace.push_back(&cat);

    test.update_search_space(&testspace);
    pspace *root = test.get_root();

    // iterate through the concrete parameters at this level
    gs::params *fst_params = root->get_paramlist ();
    gs::params::iterator it;
    int ctr = 0;
    for (it = fst_params->begin (); it != fst_params->end (); it++) {

        std::vector<double> tmpvals = std::get<1>(*it);

        // these tests are entirely specific to the values used at the beginning
        // of this function.
        if (ctr == 0) {
            assert (std::get<0>(*it) == std::string ("testrandint"));
            for (int i = 0; i < 10; i++)
                assert (dbleq (tmpvals[i], (double) i));
        } else if (ctr == 1) {
            assert (std::get<0>(*it) == std::string ("testquniform"));
            for (int i = 0; i < 10; i++)
                assert (dbleq (tmpvals[i], (double) i));
        } else if (ctr == 2) {
            assert (std::get<0>(*it) == std::string ("testcat"));
            for (int i = 0; i < 5; i++) {
                assert (dbleq (tmpvals[i], (double) i));
            }
        }
        ctr++;
    }

    // iterate through the nested search space(s)
    gs::subspaces *fst_subsp = root->get_subspaces ();
    gs::subspaces::iterator i;
    for (i = fst_subsp->begin (); i != fst_subsp->end (); i++) {
        std::cout << (*i)->get_name() << std::endl;
    }
}

void
run_gridsearch_tests()
{
    test_update_search_space ();
    std::cout << "All gridsearch tests pass" << std::endl;
}

