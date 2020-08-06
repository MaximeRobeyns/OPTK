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

static void
test_categorical()
{
    std::vector<int> myopts;
    for (int i = 0; i < 5; i++) {
        myopts.push_back (i);
    }

    optk::categorical<int> test ("int categorical", &myopts);

    assert (test.get_name () == std::string ("int categorical"));
    assert (test.get_type () == param::categorical);
    assert (test.count () == myopts.size ());

    std::vector<int> *ret = test.values ();
    assert (ret->size () == myopts.size ());

    for (int i = 0; i < 5; i++) {
        assert (test.get (i) == i);
    }
}

// TODO test the choice parameter type once others have been implemented
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
test_randint ()
{
    optk::randint testrandint("test", 0, 10);


    assert (testrandint.get_name () == std::string ("test"));
    assert (testrandint.get_type () == param::randint);

    for (int i = 0; i < 100; i++) {
        int tmp = testrandint.sample ();
        assert (0 <= tmp && tmp <= 10);
    }
}

static void
test_uniform ()
{
    optk::uniform testuniform ("testing", 0, 10);

    assert (testuniform.get_name () == std::string ("testing"));
    assert (testuniform.get_type () == param::uniform);

    for (int i = 0; i < 100; i++) {
        double tmp = testuniform.sample ();
        assert (0.0 <= tmp && tmp <= 10.0);
    }
}

static void
test_quniform ()
{
    optk::quniform testquniform ("testing", 0, 10, 2);

    assert (testquniform.get_name () == std::string ("testing"));
    assert (testquniform.get_type () == param::quniform);

    for (int i = 0; i < 100; i++) {
        double tmp = testquniform.sample ();
        assert (0.0 <= tmp && tmp <= 10.0);
        assert ((int) tmp % 2 == 0);
    }
}

static void
test_loguniform ()
{
    optk::loguniform tloguniform ("testing", 1, 10);

    assert (tloguniform.get_name () == std::string ("testing"));
    assert (tloguniform.get_type () == param::loguniform);

    for (int i = 0; i < 100; i++) {
        double tmp = tloguniform.sample ();
        assert (1.0 <= tmp && tmp <= 10.0);
    }
}

static void
test_qloguniform ()
{
    optk::qloguniform testqlu ("test_qlu", 10, 20, 2);

    assert (testqlu.get_name () == std::string ("test_qlu"));
    assert (testqlu.get_type () == param::qloguniform);

    for (int i = 0; i < 100; i++) {
        double tmp = testqlu.sample ();
        assert (10.0 <= tmp && tmp <= 20);
        assert ((int) tmp % 2 == 0);
    }
}

static void
test_normal ()
{
    optk::normal testnorm ("testnormal", 5, 2.5);

    assert (testnorm.get_name () == std::string ("testnormal"));
    assert (testnorm.get_type () == param::normal);

    int N = 500;

    double total = 0;
    double values[N];
    for (int i = 0; i < N; i++) {
        values[i] = testnorm.sample();
        total += values[i];
    }
    double em_mean = total / N;
    assert (4.5 < em_mean && em_mean < 5.5);
    double tmp_em_var = 0;
    for (int i = 0; i < N; i++) {
        tmp_em_var += pow((values[i] - em_mean),2);
    }
    double em_var = sqrt(tmp_em_var / (N-1));
    assert (2.2 < em_var && em_var < 2.8);
}

static void
test_qnormal ()
{
    optk::qnormal testqnorm ("testqnorm", 10, 5, 2);

    assert (testqnorm.get_name () == std::string ("testqnorm"));
    assert (testqnorm.get_type () == param::qnormal);

    for (int i = 0; i < 100; i++) {
        assert ((int)testqnorm.sample() % 2 == 0);
    }

}

static void
test_lognormal ()
{
    optk::lognormal testlognorm ("testlognorm", 10, 5);

    assert (testlognorm.get_name () == std::string ("testlognorm"));
    assert (testlognorm.get_type () == param::lognormal);

    // TODO possibly implement a normality test on the log of sampled values
    // e.g. Kolmogorov–Smirnov test
}

void
run_type_tests()
{
    // test_choice_type();
    test_categorical ();
    test_randint ();
    test_uniform ();
    test_quniform ();
    test_loguniform ();
    test_qloguniform ();
    test_normal ();
    test_qnormal ();
    test_lognormal ();
    std::cout << "All type tests pass" << std::endl;
}

