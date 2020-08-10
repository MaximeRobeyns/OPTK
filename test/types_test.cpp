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

#include <iostream>
#include <memory>
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

static void test_choice_type() {
    optk::sspace_t options;

    optk::randint ri("randint", 0, 10);
    std::vector<int> opts = {0,1,2,3,4};
    optk::categorical<int> cat("categorical", &opts);
    optk::normal norm("normal", 0, 1);
    optk::qloguniform qlogu ("qloguniform", 1, 10, 2);
    optk::uniform uni ("uniform", 10, 20);

    options.push_back(&ri);
    options.push_back(&cat);
    options.push_back(&norm);
    options.push_back(&qlogu);
    options.push_back(&uni);

    optk::choice test("testchoice", &options);

    assert (test.get_name() == "testchoice");
    assert (test.get_type() == param::choice);
    assert (test.count() == options.size());

    for (int i = 0; i < 5; i++) {
        optk::param_t *param = test.get(i);
        param::param_type t = param->get_type();
        switch (t) {
            case param::randint:
            {
                optk::randint *rparam = static_cast<optk::randint *>(param);
                assert (rparam->get_type() == param::randint);
                assert (rparam->get_name() == std::string ("randint"));
                assert (rparam->m_lower == 0);
                assert (rparam->m_upper == 10);
                for (int j = 0; j < 100; j++) {
                    int smp = rparam->sample ();
                    assert (0 <= smp && smp <= 10);
                }
                break;
            }
            case param::categorical:
            {
                optk::categorical<int> *cparam =
                    static_cast<optk::categorical<int> *>(param);
                assert (cparam->get_name () == std::string ("categorical"));
                assert (cparam->get_type () == param::categorical);
                for (int j = 0; j < 5; j++)
                    assert (cparam->get(i) == i);
                break;
            }
            case param::normal:
            {
                optk::normal *nparam = static_cast<optk::normal *>(param);
                assert (nparam->get_name () == std::string("normal"));
                assert (nparam->get_type () == param::normal);
                assert (nparam->m_mu == 0);
                assert (nparam->m_sigma == 1);
                break;
            }
            case param::qloguniform:
            {
                optk::qloguniform *qparam =
                    static_cast<optk::qloguniform *>(param);
                assert (qparam->get_name () == std::string("qloguniform"));
                assert (qparam->get_type () == param::qloguniform);
                assert (qparam->m_lower == 1);
                assert (qparam->m_upper == 10);
                assert (qparam->m_q == 2);
                for (int j = 0; j < 100; j++) {
                    double tmp = qparam->sample ();
                    assert (1.0 <= tmp && tmp <= 10.0);
                    assert ((int) tmp % 2 == 0);
                }
                break;
            }
            case param::uniform:
            {
                optk::uniform *uparam = static_cast<optk::uniform *>(param);
                assert (uparam->get_name () == std::string ("uniform"));
                assert (uparam->get_type () == param::uniform);
                assert (uparam->m_lower == 10);
                assert (uparam->m_upper == 20);
                for (int j = 0; j < 100; j++) {
                    double tmp = uparam->sample ();
                    assert (10.0 <= tmp);
                    assert (tmp <= 20.0);
                }
                break;
            }
            default:
                // uh oh! we shuoldn't end up here.
                assert (0 == 1); // hacky stop
                break;
        }
    }
}

static void
test_randint ()
{
    optk::randint testrandint("test", 0, 10);


    assert (testrandint.get_name () == std::string ("test"));
    assert (testrandint.get_type () == param::randint);
    assert (testrandint.m_lower == 0);
    assert (testrandint.m_upper == 10);

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
    assert (testuniform.m_lower == 0);
    assert (testuniform.m_upper == 10);

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
    assert (testquniform.m_q == 2);

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
    assert (testqlu.m_q == 2);

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
    assert (testnorm.m_mu == 5);
    assert (testnorm.m_sigma == 2.5);

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
    double em_stddev = sqrt(tmp_em_var / (N-1));
    assert (2.2 < em_stddev && em_stddev < 2.8);
}

static void
test_qnormal ()
{
    optk::qnormal testqnorm ("testqnorm", 10, 5, 2);

    assert (testqnorm.get_name () == std::string ("testqnorm"));
    assert (testqnorm.get_type () == param::qnormal);
    assert (testqnorm.m_q == 2);

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

static void
test_qlognormal ()
{
    optk::qlognormal testqln ("testqln", 10, 1.5, 3);

    assert (testqln.get_name () == std::string ("testqln"));
    assert (testqln.get_type () == param::qlognormal);
    assert (testqln.m_q == 3);

    for (int i = 0; i < 100; i++) {
        assert ((int) testqln.sample() % 3 == 0);
    }
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
    test_qlognormal ();
    test_choice_type ();
    std::cout << "All type tests pass" << std::endl;
}

