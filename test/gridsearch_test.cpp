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

/**
 * test the values returned from a search space
 */
static void
tst_val (std::string n, int l, int u, pspace::params::iterator val)
{
    std::vector<double> tmpvals = std::get<1>(*val);
    assert (std::get<0>(*val) == n);
    for (int i = l; i < u; i++)
        assert (dbleq (tmpvals[i], (double) i));
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
    optk::quniform c2("choice2", 0, 10, 1);
    std::vector<double> c_cat = {0,1,2,3,4,5,6,7,8,9,10};
    optk::categorical<double> c3("choice3", &c_cat);
    optk::sspace_t copts = {&c1, &c2, &c3};
    optk::choice choice("testchoice", &copts);

    testspace.push_back(&tri);
    testspace.push_back(&tqu);
    testspace.push_back(&choice);
    testspace.push_back(&cat);

    test.update_search_space(&testspace);

    // this is defined only when __OPTK_TESTING is set
    pspace *root = test.get_root();

    // iterate through the concrete parameters at this level
    pspace::params *fst_params = root->get_paramlist ();
    pspace::params::iterator it;
    int ctr = 0;
    for (it = fst_params->begin (); it != fst_params->end (); it++) {
        std::vector<double> tmpvals = std::get<1>(*it);

        // these tests are entirely specific to the values used at the beginning
        // of this function.
        if (ctr == 0)
            tst_val (std::string ("testrandint"), 0, 10, it);
        else if (ctr == 1)
            tst_val (std::string ("testquniform"), 0, 10, it);
        else if (ctr == 2)
            tst_val (std::string ("testcat"), 0, 5, it);
        ctr++;
    }

    assert (ctr == 3);

    // iterate through the nested search space(s)
    pspace::subspaces *fst_subsp = root->get_subspaces ();
    assert (fst_subsp->size() == (size_t) 1);

    pspace *nested = (*fst_subsp)[0];
    assert (nested->get_name() == std::string("testchoice"));
    pspace::params *snd_params = nested->get_paramlist ();

    ctr = 0;
    for (it = snd_params->begin (); it != snd_params->end (); it++) {
        if (ctr == 0)
            tst_val (std::string ("choice1"), 0, 5, it);
        else if (ctr == 1)
            tst_val (std::string ("choice2"), 0, 10, it);
        else if (ctr == 2)
            tst_val (std::string ("choice3"), 0, 10, it);
        ctr++;
    }
}

void
run_gridsearch_tests()
{
    test_update_search_space ();
    std::cout << "All gridsearch tests pass" << std::endl;
}

