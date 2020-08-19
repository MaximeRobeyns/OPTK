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
 * @brief This file implements a simple gridsearch algorithm.
 */

#include <optimisers/gridsearch.hpp>

/** This namespace contains types which are specific to the gridsearch
 * algorithm */
namespace __gs {

class node;

/** Subspaces is a list of key, node pairs */
typedef std::vector<std::tuple<std::string, node *>> subspaces;

/** params is a list of key, value pairs; here represented as the base class
 * __gs::param, values must be cast to their concrete type before use upon
 * access. */
typedef std::vector<std::tuple<std::string, param *>> params;

param::param (const std::string &k, pspace_t t) :
    m_key(k), m_type(t)
{}

/** Value contains lists of integers, doubles or strings. */
template <class T>
class value: public param {
    public:
        value (const std::string &k, pspace_t t) :
            param (k, t)
        {
            m_values = std::vector<T>();
            m_i = 0; m_max = m_values.size();
        }

        value (const std::string &k, std::vector<int> v) :
            param (k, pspace_t::int_val)
        {
            m_values = std::vector<int>(v);
            m_i = 0; m_max = m_values.size();
        }

        value (const std::string &k, std::vector<double> v) :
            param (k, pspace_t::dbl_val)
        {
            m_values = std::vector<double>(v);
            m_i = 0; m_max = m_values.size();
        }

        value (const std::string &k, std::vector<std::string> v) :
            param (k, pspace_t::str_val)
        {
            m_values = std::vector<std::string>(v);
            m_i = 0; m_max = m_values.size();
        }

        std::vector<T> *get_vals() { return &m_values; }
        T at(unsigned int i) { return m_values.at(i); }
        void set(std::vector<T> vs) { m_values = vs; }

        /** Returns the cardinality of the set of values for this parmeter. */
        unsigned int
        get_count ()
        {
            return m_values.size();
        }

        /**
         * Next will iterate through the enumerated values for this parameter,
         * and update the val parameter to be the next one along. If this is
         * the last value in the set, then this function will return true,
         * otherwise it will return false.
         * @param val The value to be updated with the next parameter.
         * @returns A boolean indicating whether the returned value was the
         * first in the set. This occurs upon first invokation, as well as when
         * 'wrapping around'.
         */
        bool next (T *val) {
            *val = m_values.at(m_i);
            m_i = (m_i + 1)%m_max;
            return m_i == 1;
        }

    private:
        std::vector<T> m_values;
        unsigned int m_i, m_max;
};

/** A node contains the parameters for this 'level' of the search space,
 * including concrete parameters, and nested search spaces. */
class node: public param {
    public:
        node (const std::string &k) :
            param (k, pspace_t::node)
        {
            nodes = subspaces();
            values = params();
            node_idx = val_idx = node_max = val_max = 0;
        }

        ~node ()
        {
            subspaces::iterator it;
            for (it = nodes.begin(); it != nodes.end(); it++) {
                delete std::get<1>(*it);
            }

            for (unsigned int i = 0; i < values.size(); i++) {
                param *tmpv = std::get<1>(values.at(i));
                inst::param *tmp_inst = local_params.at(i);
                pspace_t t = tmpv->get_type();
                switch (t) {
                    case pspace_t::int_val:
                    {
                        value<int> *iv = static_cast<value<int> *>(tmpv);
                        inst::int_val *iiv =
                            static_cast<inst::int_val *>(tmp_inst);
                        delete iv; delete iiv;
                        break;
                    }
                    case pspace_t::dbl_val:
                    {
                        value<double> *dv = static_cast<value<double> *>(tmpv);
                        inst::dbl_val *idv =
                            static_cast<inst::dbl_val *>(tmp_inst);
                        delete dv; delete idv;
                        break;
                    }
                    case pspace_t::str_val:
                    {
                        value<std::string> *sv =
                            static_cast<value<std::string> *>(tmpv);
                        inst::str_val *isv =
                            static_cast<inst::str_val *>(tmp_inst);
                        delete sv; delete isv;
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        void
        add_item (param *p)
        {
            pspace_t t = p->get_type ();
            switch (t) {
                case pspace_t::node:
                    {
                        node *tmp_node = static_cast<node *>(p);
                        nodes.push_back({p->get_key(), tmp_node});
                        node_max = nodes.size ();
                        break;
                    }
                default:
                    {
                        values.push_back({p->get_key(), p});
                        val_max = values.size ();
                        inst::param *tmp;
                        if (t == pspace_t::int_val) {
                            tmp = new inst::int_val(p->get_key(), 0);
                        } else if (t == pspace_t::dbl_val) {
                            tmp = new inst::dbl_val(p->get_key(), 0);
                        } else {
                            tmp = new inst::str_val(p->get_key(), "");
                        }
                        local_params.push_back (tmp);
                        break;
                    }
            }
        }

        subspaces *
        get_subspaces ()
        { return &nodes; }

        params *
        get_values ()
        { return &values; }

        /**
         * This will iterate through all the concrete parameters once, before
         * calling step on each of the the elemnts in the nodes array.
         * @returns A pointer to a root node.
         */
        inst::set
        step(inst::node *parent)
        {
            bool cont = true;
            unsigned int i, max = values.size();
            for (i = 0; i < max; i++) {
                param *p = std::get<1>(values.at(i));
                pspace_t t = p->get_type();
                switch (t) {
                    case pspace_t::int_val:
                    {
                        value<int> *tmp_int = static_cast<value<int> *>(p);
                        inst::int_val *iinst =
                            static_cast<inst::int_val *>(local_params.at(i));
                        if (cont)
                            cont = tmp_int->next (iinst->get_addr());
                        parent->add_item (
                                new inst::int_val (p->get_key(), iinst->get_val())
                                );
                        break;
                    }
                    case pspace_t::dbl_val:
                    {
                        value<double> *tmp_dbl =
                            static_cast<value<double> *>(p);
                        inst::dbl_val *dinst =
                            static_cast<inst::dbl_val *>(local_params.at(i));
                        if (cont)
                            cont = tmp_dbl->next (dinst->get_addr ());
                        parent->add_item (
                                new inst::dbl_val (p->get_key(), dinst->get_val())
                                );
                        break;
                    }
                    case pspace_t::str_val:
                    {
                        value<std::string> *tmp_str =
                            static_cast<value<std::string> *>(p);
                        inst::str_val *sinst =
                            static_cast<inst::str_val *>(local_params.at(i));
                        if (cont)
                            cont = tmp_str->next (sinst->get_addr ());
                        parent->add_item (
                                new inst::str_val (p->get_key(), sinst->get_val())
                                );
                        break;
                    }
                    default: // this never happens, even if it did, we don't care
                        break;
                }
            }

            subspaces::iterator s_it;
            for (s_it = nodes.begin (); s_it != nodes.end (); s_it++) {
                node *tmp_node = std::get<1>(*s_it);
                inst::node *new_node = new inst::node (tmp_node->get_key());
                parent->add_item (tmp_node->step(new_node));
            }

            return parent;
        }

    private:
        // all param pointers in this array have type pspace_t::node.
        subspaces nodes;
        // all param pointers in this array have type pspace_t::value.
        params values;

        // This is a vector of all the non-node parameter instances.
        std::vector<inst::param *> local_params;

        // TODO verify that x_max are necessary.
        unsigned int node_idx, val_idx, node_max, val_max;
};

} // namespace __gs


// Actual gridsearch class methods ============================================

// free, static functions -----------------------------------------------------

template <typename T>
static void
unpack_categorical (sspace::param_t *param, __gs::node *parent)
{
    sspace::categorical<T> *cat =
        static_cast<sspace::categorical<T> *>(param);
    std::string name = cat->get_name ();
    std::vector<T> values = *cat->values ();
    __gs::param *tmp_val = new __gs::value<T>(name, values);
    parent->add_item (tmp_val);
}

/**
 * This function will 'unpack' a parameter in the search space definition,
 * enumerating all its values if it is a concerete parameter, or recursively
 * unpacking nested / conditional parameters.
 * @param param The parameter description to 'unpack'
 * @param parent The parameter space node in which to store the resulting
 * unpacked value.
 */
static void
unpack_param (sspace::param_t *param, __gs::node *parent)
{
    // assert that the param type is comparible with gridsearch.
    pt t = param->get_type ();
    assert (
        t == pt::randint ||
        t == pt::quniform ||
        t == pt::categorical_int ||
        t == pt::categorical_dbl ||
        t == pt::categorical_str ||
        t == pt::choice
    );

    switch (t) {
        case pt::randint:
        {
            sspace::randint *ri = static_cast<sspace::randint *>(param);
            std::string name = ri->get_name ();
            std::vector<int> values;

            for (int i = ri->m_lower; i <= ri->m_upper; i++) {
                values.push_back (i);
            }

            __gs::param *tmp_val = new __gs::value<int>(name, values);
            parent->add_item (tmp_val);
            break;
        }
        case pt::quniform:
        {
            sspace::quniform *qu = static_cast<sspace::quniform *>(param);
            std::string name = qu->get_name ();
            std::vector<double> values;
            for (double i = qu->m_lower; i <= qu->m_upper; i+=qu->m_q)
                values.push_back (i);
            __gs::param *tmp_val = new __gs::value<double>(name, values);
            parent->add_item (tmp_val);
            break;
        }
        case pt::categorical_int:
        {
            // unpack_categorical<int>(param, parent);
            sspace::categorical<int> *cat =
                static_cast<sspace::categorical<int> *>(param);
            std::string name = cat->get_name ();
            std::vector<int> values = *cat->values ();
            __gs::param *tmp_val = new __gs::value<int>(name, values);
            parent->add_item (tmp_val);
            break;
        }
        case pt::categorical_dbl:
        {
            // unpack_categorical<double>(param, parent);
            sspace::categorical<double> *cat =
                static_cast<sspace::categorical<double> *>(param);
            std::string name = cat->get_name ();
            std::vector<double> values = *cat->values ();
            __gs::param *tmp_val = new __gs::value<double>(name, values);
            parent->add_item (tmp_val);
            break;
        }
        case pt::categorical_str:
        {
            unpack_categorical<std::string>(param, parent);
            /*
            sspace::categorical<std::string> *cat =
                static_cast<sspace::categorical<std::string> *>(param);
            std::string name = cat->get_name ();
            std::vector<std::string> values = *cat->values ();
            __gs::param *tmp_val = new __gs::value<std::string>(name, values);
            parent->add_item (tmp_val);
            */
            break;
        }
        case pt::choice:
        {
            sspace::choice *c = static_cast <sspace::choice *>(param);

            // instantiate a new node on the heap
            __gs::node *nspace = new __gs::node (c->get_name ());
            parent->add_item (nspace);

            sspace::sspace_t *subspace = c->options ();
            sspace::sspace_t::iterator it;
            for (it = subspace->begin (); it != subspace->end (); it++)
                unpack_param(*it, nspace);
            break;
        }
        default:
            break;
    }
}

// gridsearch member functions ------------------------------------------------

gridsearch::gridsearch () :
    optimiser ("gridsearch")
{
    m_root = NULL;
}

gridsearch::~gridsearch ()
{
    __gs::node *tmp_root = static_cast<__gs::node *>(m_root);
    delete tmp_root;

    std::unordered_map<int, inst::set>::iterator it;
    for (it = trials.begin (); it != trials.end (); it++)
        inst::free_node(std::get<1>(*it));
}


void
gridsearch::update_search_space (sspace::sspace_t *space)
{
    // free any existing / old search spaces
    if (m_root)
        delete (m_root);

    // initialise a new parameter node on the heap
    __gs::node *new_root = new __gs::node("root");

    sspace::sspace_t::iterator it;
    for (it = space->begin (); it != space->end (); it++)
        unpack_param (*it, new_root);

    m_root = new_root;
}

void
gridsearch::add_to_trials (int param_id, inst::set n)
{
    if (trials.count(param_id))
        inst::free_node(trials.at(param_id));
    trials.emplace(param_id, n);
}

inst::set
gridsearch::generate_parameters (int param_id)
{
    inst::node *root = new inst::node("gridsearch parameters");

    __gs::node *r_node = static_cast<__gs::node *>(m_root);

    r_node->step(root);

    add_to_trials (param_id, root);
    return root;
}

// dummy functions TODO get rid of these ----------------------------------------


void gridsearch::receive_trial_results (int pid, inst::set params, double value)
{
    // in here, free the parameter set which was previously allocated to the
    // heap.
    return;
}

// Gridsearch tests ===========================================================

/// TODO uncomment this
// #ifdef __OPTK_TESTING

// compare two double-precision floating point values.
static bool
dbleq (double a, double b)
{
    return std::fabs (a - b) < std::numeric_limits<double>::epsilon();
}

void
test_update_search_space ()
{
    gridsearch test = gridsearch ();

    sspace::randint tri ("testrandint", 0, 10);
    sspace::quniform tqu ("testquniform", 0, 10, 2.5);
    std::vector<int> int_opts = {0,1,2,3,4,5};
    sspace::categorical<int> cat1 ("testcatint", &int_opts);
    std::vector<double> dbl_opts = {0.0, 2.5, 5.0, 7.5, 10.0};
    sspace::categorical<double> cat2 ("testcatdbl", &dbl_opts);
    std::vector<std::string> str_opts = {
        std::string ("first"),
        std::string ("second"),
        std::string ("third"),
    };
    sspace::categorical<std::string> cat3 ("testcatstr", &str_opts);

    sspace::randint c1 ("choice1", 0, 5);
    sspace::quniform c2 ("choice2", 0, 5, 1);
    sspace::categorical<int> c3 ("choice3", &int_opts);
    sspace::categorical<double> c4 ("choice4", &dbl_opts);
    sspace::categorical<std::string> c5 ("choice5", &str_opts);
    sspace::sspace_t copts = {&c1, &c2, &c3, &c4, &c5};
    sspace::choice choice ("testchoice", &copts);

    sspace::sspace_t testspace ({&tri, &tqu, &cat1, &cat2, &cat3, &choice});

    test.update_search_space (&testspace);

    // get_root is only defined when __OPTK_TESTING is defined.
    __gs::param *root = test.get_root ();

    __gs::node *nroot = static_cast<__gs::node *> (root);

    __gs::params *ps = nroot->get_values ();

    // testrandint ------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_tri = ps->at (0);
    assert (std::get<0> (p_tri) == std::string ("testrandint"));
    __gs::value<int> *pv_tri =
        static_cast<__gs::value<int> *>(std::get<1> (p_tri));
    assert (pv_tri->get_type () == __gs::pspace_t::int_val);
    for (int i = 0; i < 10; i++)
        assert (pv_tri->at (i) == i);

    // testquiform ------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_tqu = ps->at (1);
    assert (std::get<0> (p_tqu) == std::string("testquniform"));
    __gs::value<double> *pv_tqu =
        static_cast<__gs::value<double> *>(std::get<1> (p_tqu));
    assert (pv_tqu->get_type () == __gs::pspace_t::dbl_val);
    assert (pv_tqu->get_key () == "testquniform");
    for (int i = 0; i < 5; i++)
        assert (dbleq (pv_tqu->at (i), i * 2.5));

    // testcatint -------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_catint = ps->at (2);
    assert (std::get<0> (p_catint) == std::string("testcatint"));
    __gs::value<int> *pv_tci =
        static_cast<__gs::value<int> *>(std::get<1>(p_catint));
    assert (pv_tci->get_type () == __gs::pspace_t::int_val);
    assert (pv_tci->get_key () == "testcatint");
    for (int i = 0; i < 6; i++)
        assert (pv_tci->at (i) == i);

    // testcatdbl -------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_catdbl = ps->at (3);
    assert (std::get<0> (p_catdbl) == std::string ("testcatdbl"));
    __gs::value<double> *pv_tcd =
        static_cast<__gs::value<double> *>(std::get<1>(p_catdbl));
    assert (pv_tcd->get_type () == __gs::pspace_t::dbl_val);
    assert (pv_tcd->get_key () == "testcatdbl");
    for (int i = 0; i < 5; i++)
        assert (dbleq (pv_tcd->at (i), i * 2.5));

    // testcatstr ------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_catstr = ps->at (4);
    assert (std::get<0> (p_catstr) == std::string("testcatstr"));
    __gs::value<std::string> *pv_tcs =
        static_cast<__gs::value<std::string> *>(std::get<1>(p_catstr));
    assert (pv_tcs->get_type () == __gs::pspace_t::str_val);
    assert (pv_tcs->get_key () == "testcatstr");
    assert (pv_tcs->at(0) == std::string("first"));
    assert (pv_tcs->at(1) == std::string("second"));
    assert (pv_tcs->at(2) == std::string("third"));


    // node testing -----------------------------------------------------------

    __gs::subspaces *ss = nroot->get_subspaces();
    std::tuple<std::string, __gs::node *> node_tpl = ss->at(0);
    assert (std::get<0> (node_tpl) == std::string ("testchoice"));
    __gs::node *fst_node = std::get<1>(node_tpl);
    assert (fst_node->get_subspaces()->size() == (unsigned int) 0);

    __gs::params *vals = fst_node->get_values();
    assert (vals->size() == copts.size());
    std::vector<std::string> ns =
        {"choice1", "choice2", "choice3", "choice4", "choice5"};
    std::vector<__gs::pspace_t> ts = {__gs::pspace_t::int_val,
        __gs::pspace_t::dbl_val, __gs::pspace_t::int_val,
        __gs::pspace_t::dbl_val, __gs::pspace_t::str_val};
    for (int i = 0; i < 5; i++) {
        assert (std::get<1>(vals->at(i))->get_key() == ns[i]);
        assert (std::get<1>(vals->at(i))->get_type() == ts[i]);
    }
}

static void
test_generate_parameters ()
{
    std::cout << "gridsearch generate parameters tests pass" << std::endl;
}

void
run_static_gridsearch_tests ()
{
    test_update_search_space ();
    test_generate_parameters ();
    std::cout << "gridsearch tests pass" << std::endl;
}

// #endif // __OPTK_TESTING

