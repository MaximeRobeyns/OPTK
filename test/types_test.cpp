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
 * @brief Implements tests for optk types.
 */

#include <tests/types_test.hpp>
#include <optk/types.hpp>

/*
static void test_choice_type() {
    optk::choice_opts options(5);

    for (int i = 0; i < 5; i++) {
        optk::value<int> tmp(i);
        // implicitly upcasts to optk::param_t
        options.push_back(&tmp);
    }

    optk::choice test("testchoice", options);

    assert (test.get_type() == param::choice);
    assert (test.get_name() == std::string("testchoice"));

    // for (int i = 0; i < 5; i++) {
    //     // make the assumption that the value is an integer
    //     std::unique_ptr<optk::param_t> base = test.get(i);
    //     // optk::param_t *base = test.get(i);
    //     // optk::value<int> *val = dynamic_cast<optk::value<int> *> (base);
    //     // assert (val->val == i);
    // }
}
*/

static void
test_categorical()
{
    std::vector<int> myopts;
    for (int i = 0; i < 5; i++) {
        myopts.push_back(i);
    }

    optk::categorical<int> test("int categorical", &myopts);

    assert (test.get_name() == std::string("int categorical"));
    assert (test.get_type() == param::categorical);
    assert (test.count() == myopts.size());

    std::vector<int> *ret = test.values();
    assert(ret->size() == myopts.size());

    for (int i = 0; i < 5; i++) {
        assert (test.get(i) == i);
    }
}

void run_type_tests() {
    // test_choice_type();
    test_categorical();
    std::cout << "All type tests pass" << std::endl;
}

