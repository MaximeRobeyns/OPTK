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
 * @brief Implements tests for the random search baseline algorithm.
 */

#include <optimisers/random.hpp>

#include <tests/optimiser_test.hpp>
#include <tests/testutils.hpp>
#include <benchmarks/synthetic.hpp>

void
test_random_search_functionality ()
{
    // instantiate an example benchmark (alpine1)
    syn::alpine1 a1(10);

    random_search test = random_search ();

    test.update_search_space (a1.get_search_space());

    int i = 0;
    while (i++ < 100) {
        inst::set ss = test.generate_parameters (i);
        a1.validate_param_set(ss);
        double res = a1.evaluate(ss);
        test.receive_trial_results(i, ss, res);
    }
}

void
run_random_search_tests()
{
    test_random_search_functionality ();
    std::cout << "All random search tests pass" << std::endl;
}

