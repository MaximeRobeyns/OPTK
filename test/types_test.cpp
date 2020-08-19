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

static bool
dbleq (double a, double b)
{
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

// test types for concrete values ---------------------------------------------

/*
 * creates the test values:
 * [
 *  "val1": 42,
 *  "val2": 21.0,
 *  "val3": "string test",
 *  "val4": [
 *      "val4.1": 42,
 *      "val4.2": 21.0,
 *      "val4.3": "string test"
 *  ]
 * ]
 */

static void
test_concrete_types ()
{
    inst::int_val v41("val4.1", 42);
    inst::dbl_val v42("val4.2", 21.0);
    inst::str_val v43("val4.3", "string test");
    inst::node val4 ("val4");
    val4.add_items (std::vector<inst::param *>({ &v41, &v42, &v43, }));

    inst::int_val val1("val1", 42);
    inst::dbl_val val2("val2", 21.0);
    inst::str_val val3("val3", "string test");

    inst::node root ("root");

    root.add_item(&val1);
    root.add_item(&val2);
    root.add_item(&val3);
    root.add_item(&val4);

    assert (root.get_key() == std::string("root"));
    assert (root.get_type() == inst::inst_t::node );

    // should silently return a NULL pointer if the key is non-existant.
    inst::int_val *test_null = inst::get<inst::int_val *>(&root, "nonexistant");
    assert (test_null == NULL);

    // integer access
    // direct access:
    inst::int_val *fst_int = static_cast<inst::int_val *>(root.get_item ("val1"));
    assert (fst_int->get_val () == 42);
    // using convenience function:
    inst::int_val *snd_int = inst::get<inst::int_val *>(&root, "val1");
    assert (snd_int->get_val () == 42);
    // using macro:
    GETINT(trd_int, &root, "val1");
    assert (trd_int->get_val() == 42);

    // double access
    inst::dbl_val *fst_dbl = static_cast<inst::dbl_val *>(root.get_item ("val2"));
    assert (dbleq (fst_dbl->get_val(), 21.0));
    inst::dbl_val *snd_dbl = inst::get<inst::dbl_val *>(&root, "val2");
    assert (dbleq (snd_dbl->get_val(), 21.0));
    GETDBL(trd_dbl, &root, "val2");
    assert (dbleq (trd_dbl->get_val(), 21.0));

    // string access
    inst::str_val *fst_str = static_cast<inst::str_val *>(root.get_item ("val3"));
    assert (fst_str->get_val() == std::string ("string test"));
    inst::str_val *snd_str = inst::get<inst::str_val *>(&root, "val3");
    assert (snd_str->get_val() == std::string ("string test"));
    GETSTR(trd_str, &root, "val3");
    assert (trd_str->get_val() == std::string ("string test"));

    // node access
    inst::node *fst_node = static_cast<inst::node *>(root.get_item ("val4"));
    assert (fst_node->get_key() == std::string("val4"));
    inst::node *snd_node = inst::get<inst::node *>(&root, "val4");
    assert (snd_node->get_key() == std::string("val4"));
    GETNODE(tmpnode, &root, "val4");
    assert (tmpnode->get_key() == std::string("val4"));

    // nested item access
    GETINT(nint, tmpnode, "val4.1");
    assert (nint->get_val() == 42);
    assert (nint->get_type () == inst::inst_t::int_val);
    assert (nint->get_key() == std::string ("val4.1"));
    nint->update_val(100);
    GETINT(nint_2, tmpnode, "val4.1");
    assert (nint_2->get_val() == 100);

    GETDBL(ndbl, tmpnode, "val4.2");
    assert (dbleq (ndbl->get_val(), 21.0));

    GETSTR(nstr, tmpnode, "val4.3");
    assert (nstr->get_val() == std::string("string test"));
}

static void
test_heap_concrete_types ()
{
    inst::int_val *v41 = new inst::int_val("val4.1", 42);
    inst::dbl_val *v42 = new inst::dbl_val("val4.2", 21.0);
    inst::str_val *v43 = new inst::str_val("val4.3", "string test");
    inst::node *val4 = new inst::node("val4");
    val4->add_items (std::vector<inst::param *>({ v41, v42, v43, }));

    inst::int_val *val1 = new inst::int_val("val1", 42);
    inst::dbl_val *val2 = new inst::dbl_val("val2", 21.0);
    inst::str_val *val3 = new inst::str_val("val3", "string test");

    inst::node *root = new inst::node("root");
    root->add_item(val1);
    root->add_item(val2);
    root->add_item(val3);
    root->add_item(val4);

    inst::free_node (root);

    // There are no asserts in this function, but the address sanitiser will
    // warn of any memory leaks if the free_node function does not work
    // correctly.
}

// test search space types ----------------------------------------------------

static void
test_categorical()
{
    // integers

    std::vector<int> myopts;
    for (int i = 0; i < 5; i++) {
        myopts.push_back (i);
    }

    sspace::categorical<int> test ("int categorical", &myopts);

    assert (test.get_name () == std::string ("int categorical"));
    assert (test.get_type () == pt::categorical_int);
    assert (test.count () == myopts.size ());

    std::vector<int> *ret = test.values ();
    assert (ret->size () == myopts.size ());

    for (int i = 0; i < 5; i++) {
        assert (test.get (i) == i);
    }

    // doubles

    std::vector<double> myopts_dbl;
    for (double i = 0.0; i <= 10.0; i+=2.5) {
        myopts.push_back (i);
    }

    sspace::categorical<double> test_dbl ("double categorical", &myopts_dbl);

    assert (test_dbl.get_name () == std::string ("double categorical"));
    assert (test_dbl.get_type () == pt::categorical_dbl);
    assert (test_dbl.count () == myopts_dbl.size ());

    std::vector<double> *ret_dbl = test_dbl.values ();
    assert (ret_dbl->size () == myopts_dbl.size ());

    int ctr = 0;
    for (double i = 0.0; dbleq (i, 10.0); i+=2.5)
        dbleq (test_dbl.get (ctr++), i);

    // strings

    std::vector<std::string> myopts_str;
    myopts_str.push_back (std::string("one"));
    myopts_str.push_back (std::string("two"));
    myopts_str.push_back (std::string("three"));
    myopts_str.push_back (std::string("four"));
    myopts_str.push_back (std::string("five"));

    sspace::categorical<std::string> test_str ("string categorical", &myopts_str);

    assert (test_str.get_name () == std::string ("string categorical"));
    assert (test_str.get_type () == pt::categorical_str);
    assert (test_str.count () == myopts_str.size ());

    std::vector<std::string> *ret_str = test_str.values ();
    assert (ret_str->size () == myopts_str.size ());

    assert (test_str.get(0) == std::string ("one"));
    assert (test_str.get(1) == std::string ("two"));
    assert (test_str.get(2) == std::string ("three"));
    assert (test_str.get(3) == std::string ("four"));
    assert (test_str.get(4) == std::string ("five"));
}

static void test_choice_type() {
    sspace::sspace_t options;

    sspace::randint ri("randint", 0, 10);
    std::vector<int> int_opts = {0,1,2,3,4};
    sspace::categorical<int> cat_int("categorical_int", &int_opts);
    std::vector<double> dbl_opts = {0.0, 0.1, 0.2, 0.3, 0.4};
    sspace::categorical<double> cat_dbl("categorical_dbl", &dbl_opts);
    std::vector<std::string> str_opts = {
        std::string("first"), std::string("second"), std::string("third")
    };
    sspace::categorical<std::string> cat_str("categorical_str", &str_opts);
    sspace::normal norm("normal", 0, 1);
    sspace::qloguniform qlogu ("qloguniform", 1, 10, 2);
    sspace::uniform uni ("uniform", 10, 20);

    options.push_back(&ri);
    options.push_back(&cat_int);
    options.push_back(&cat_dbl);
    options.push_back(&cat_str);
    options.push_back(&norm);
    options.push_back(&qlogu);
    options.push_back(&uni);

    sspace::choice test("testchoice", &options);

    assert (test.get_name() == "testchoice");
    assert (test.get_type() == pt::choice);
    assert (test.count() == options.size());

    for (int i = 0; i < 5; i++) {
        sspace::param_t *param = test.get(i);
        pt t = param->get_type();
        switch (t) {
            case pt::randint:
            {
                sspace::randint *rparam = static_cast<sspace::randint *>(param);
                assert (rparam->get_type() == pt::randint);
                assert (rparam->get_name() == std::string ("randint"));
                assert (rparam->m_lower == 0);
                assert (rparam->m_upper == 10);
                for (int j = 0; j < 100; j++) {
                    int smp = rparam->sample ();
                    assert (0 <= smp && smp <= 10);
                }
                break;
            }
            case pt::categorical_int:
            {
                sspace::categorical<int> *cparam =
                    static_cast<sspace::categorical<int> *>(param);
                assert (cparam->get_name () == std::string ("categorical_int"));
                assert (cparam->get_type () == pt::categorical_int);
                for (int j = 0; j < 5; j++)
                    assert (cparam->get(i) == i);
                break;
            }
            case pt::categorical_dbl:
            {
                sspace::categorical<double> *cparam =
                    static_cast<sspace::categorical<double> *>(param);
                assert (cparam->get_name () == std::string ("categorical_dbl"));
                assert (cparam->get_type () == pt::categorical_dbl);
                for (int j = 0; j < 5; j++) {
                    assert (dbleq (cparam->get(j), ((double)j)/10.0));
                }
                break;
            }
            case pt::categorical_str:
            {
                sspace::categorical<std::string> *cparam =
                    static_cast<sspace::categorical<std::string> *>(param);
                assert (cparam->get_name () == std::string ("categorical_str"));
                assert (cparam->get_type () == pt::categorical_str);
                assert (cparam->get(0) == std::string("first"));
                assert (cparam->get(1) == std::string("second"));
                assert (cparam->get(2) == std::string("third"));
                break;
            }
            case pt::normal:
            {
                sspace::normal *nparam = static_cast<sspace::normal *>(param);
                assert (nparam->get_name () == std::string("normal"));
                assert (nparam->get_type () == pt::normal);
                assert (nparam->m_mu == 0);
                assert (nparam->m_sigma == 1);
                break;
            }
            case pt::qloguniform:
            {
                sspace::qloguniform *qparam =
                    static_cast<sspace::qloguniform *>(param);
                assert (qparam->get_name () == std::string("qloguniform"));
                assert (qparam->get_type () == pt::qloguniform);
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
            case pt::uniform:
            {
                sspace::uniform *uparam = static_cast<sspace::uniform *>(param);
                assert (uparam->get_name () == std::string ("uniform"));
                assert (uparam->get_type () == pt::uniform);
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
    sspace::randint testrandint("test", 0, 10);


    assert (testrandint.get_name () == std::string ("test"));
    assert (testrandint.get_type () == pt::randint);
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
    sspace::uniform testuniform ("testing", 0, 10);

    assert (testuniform.get_name () == std::string ("testing"));
    assert (testuniform.get_type () == pt::uniform);
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
    sspace::quniform testquniform ("testing", 0, 10, 2);

    assert (testquniform.get_name () == std::string ("testing"));
    assert (testquniform.get_type () == pt::quniform);
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
    sspace::loguniform tloguniform ("testing", 1, 10);

    assert (tloguniform.get_name () == std::string ("testing"));
    assert (tloguniform.get_type () == pt::loguniform);

    for (int i = 0; i < 100; i++) {
        double tmp = tloguniform.sample ();
        assert (1.0 <= tmp && tmp <= 10.0);
    }
}

static void
test_qloguniform ()
{
    sspace::qloguniform testqlu ("test_qlu", 10, 20, 2);

    assert (testqlu.get_name () == std::string ("test_qlu"));
    assert (testqlu.get_type () == pt::qloguniform);
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
    sspace::normal testnorm ("testnormal", 5, 2.5);

    assert (testnorm.get_name () == std::string ("testnormal"));
    assert (testnorm.get_type () == pt::normal);
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
    sspace::qnormal testqnorm ("testqnorm", 10, 5, 2);

    assert (testqnorm.get_name () == std::string ("testqnorm"));
    assert (testqnorm.get_type () == pt::qnormal);
    assert (testqnorm.m_q == 2);

    for (int i = 0; i < 100; i++) {
        assert ((int)testqnorm.sample() % 2 == 0);
    }
}

static void
test_lognormal ()
{
    sspace::lognormal testlognorm ("testlognorm", 10, 5);

    assert (testlognorm.get_name () == std::string ("testlognorm"));
    assert (testlognorm.get_type () == pt::lognormal);

    // TODO possibly implement a normality test on the log of sampled values
    // e.g. Kolmogorov–Smirnov test
}

static void
test_qlognormal ()
{
    sspace::qlognormal testqln ("testqln", 10, 1.5, 3);

    assert (testqln.get_name () == std::string ("testqln"));
    assert (testqln.get_type () == pt::qlognormal);
    assert (testqln.m_q == 3);

    for (int i = 0; i < 100; i++) {
        assert ((int) testqln.sample() % 3 == 0);
    }
}

void
run_type_tests()
{

    test_concrete_types ();
    test_heap_concrete_types ();

    test_choice_type();
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

