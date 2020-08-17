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

namespace __gs {

typedef std::vector<std::tuple<std::string, param *>> subspaces;

typedef std::vector<std::tuple<std::string, param *>> params;

param::param (const std::string &k, pspace_t t) :
    m_key(k), m_type(t)
{}

class node: public param {
    public:
        node (const std::string &k) :
            param (k, pspace_t::node)
        {
            nodes = subspaces();
            values = params();
        }

        void
        add_item (param *p)
        {
            pspace_t t = p->get_type ();
            switch (t) {
                case pspace_t::node:
                    {
                        nodes.push_back({p->get_key(), p});
                    }
                default:
                    {
                        values.push_back({p->get_key(), p});
                    }
            }
        }

        // get_item is completely unnecessary; when will we ever directly
        // access an element?
        // TODO remove when safe
        /*
        param *
        get_item (const std::string &k)
        {
            // not very pretty; refactor?
            try {
                return nodes.at(k);
            } catch (const std::out_of_range &e) { }

            try {
                return values.at(k);
            } catch (const std::out_of_range &e) { }

            return NULL;
        }
        */

        subspaces *
        get_subspaces ()
        { return &nodes; }

        params *
        get_values ()
        { return &values; }

        // TODO add an iteration method here

    private:
        // as an invariant, all param * in here have type pspace_t::node.
        subspaces nodes;
        // as an invariant, all param * in here have type pspace_t::value.
        params values;
};

template <class T>
class value: public param {
    public:
        value (const std::string &k, pspace_t t) :
            param (k, t)
        {
            m_values = std::vector<T>();
        }

        value (const std::string &k, std::vector<int> v) :
            param (k, pspace_t::int_val)
        {
            m_values = std::vector<int>(v);
        }

        value (const std::string &k, std::vector<double> v) :
            param (k, pspace_t::dbl_val)
        {
            m_values = std::vector<double>(v);
        }

        value (const std::string &k, std::vector<std::string> v) :
            param (k, pspace_t::str_val)
        {
            m_values = std::vector<std::string>(v);
        }

        std::vector<T> *get_vals() { return &m_values; }
        T at(unsigned int i) { return m_values[i]; }
        void set(std::vector<T> vs) { m_values = vs; }

        // TODO add an iterator method here
        // Sends a signal when the last value is returned (so that the calling
        // process can know that the iteration is complete)
        // count the lenght of the values array

    private:
        std::vector<T> m_values;
};

} // namespace __gs


// Actual gridsearch class methods ============================================

// free, static functions -----------------------------------------------------

static void
free_spaces (__gs::node *root)
{
    __gs::subspaces *ss = root->get_subspaces ();
    __gs::subspaces::iterator it;
    for (it = ss->begin(); it != ss->end(); it++) {
        __gs::node *tmp = static_cast<__gs::node *>(std::get<1>(*it));
        free_spaces (tmp);
    }

    // free all the concrete values at this level
    __gs::params *ps = root->get_values ();
    __gs::params::iterator val_it;
    for (val_it = ps->begin (); val_it != ps->end (); val_it++) {
        __gs::param *tmpv = std::get<1>(*val_it);
        __gs::pspace_t t = tmpv->get_type();
        switch (t) {
            case __gs::pspace_t::int_val:
            {
                __gs::value<int> *iv = static_cast <__gs::value<int> *>(tmpv);
                delete iv;
                break;
            }
            case __gs::pspace_t::dbl_val:
            {
                __gs::value<double> *dv = static_cast <__gs::value<double> *>(tmpv);
                delete dv;
                break;
            }
            case __gs::pspace_t::str_val:
            {
                __gs::value<std::string> *sv =
                    static_cast <__gs::value<std::string> *>(tmpv);
                delete sv;
                break;
            }
            default:
                break;
        }
    }

    // now actually free the 'root' node.
    delete root;
}

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

            for (int i = ri->m_lower; i < ri->m_upper; i++) {
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
            // this is the appoximation to the values of quniform used for
            // gridsarch.
            for (int i = qu->m_lower; i < qu->m_upper; i+=qu->m_q) {
                values.push_back ((double) i);
            }
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
            // unpack_categorical<std::string>(param, parent);
            sspace::categorical<std::string> *cat =
                static_cast<sspace::categorical<std::string> *>(param);
            std::string name = cat->get_name ();
            std::vector<std::string> values = *cat->values ();
            __gs::param *tmp_val = new __gs::value<std::string>(name, values);
            parent->add_item (tmp_val);
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
    free_spaces (tmp_root);
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

// dummy functions TODO get rid of these ----------------------------------------

inst::set gridsearch::generate_parameters (int param_id) {
    return inst::set();
}

void gridsearch::receive_trial_results (int pid, inst::set params, double value)
{
    return;
}

// Gridsearch tests ===========================================================

// #ifdef __OPTK_TESTING

// compare two double-precision floating point values.
/*
static bool
dbleq (double a, double b)
{
    return std::fabs (a - b) < std::numeric_limits<double>::epsilon();
}
*/

void
test_update_search_space ()
{
    gridsearch test = gridsearch ();

    sspace::randint tri ("testrandint", 0, 10);
    sspace::quniform tqu ("testquniform", 0, 10, 2.5);
    std::vector<int> int_opts = {0,1,2,3,4,5};
    sspace::categorical<int> cat1("testcatint", &int_opts);
    std::vector<double> dbl_opts = {0.0, 2.5, 5.0, 7.5, 10.0};
    sspace::categorical<double> cat2("testcatdbl", &dbl_opts);
    std::vector<std::string> str_opts = {
        std::string("first"),
        std::string("second"),
        std::string("third"),
    };
    sspace::categorical<std::string> cat3("testcatstr", &str_opts);

    sspace::randint c1("choice1", 0, 5);
    sspace::quniform c2("choice2", 0, 5, 1);
    sspace::categorical<int> c3("choice3", &int_opts);
    sspace::categorical<double> c4("choice4", &dbl_opts);
    sspace::categorical<std::string> c5("choice5", &str_opts);
    sspace::sspace_t copts = {&c1, &c2, &c3, &c4, &c5};
    sspace::choice choice("testchoice", &copts);

    sspace::sspace_t testspace ({&tri, &tqu, &cat1, &cat2, &cat3, &choice});

    test.update_search_space (&testspace);

    // get_root is only defined when __OPTK_TESTING is defined.
    __gs::param *root = test.get_root();

    __gs::node *nroot = static_cast<__gs::node *>(root);

    __gs::params *ps = nroot->get_values();

    // testrandint ------------------------------------------------------------

    std::tuple<std::string, __gs::param *> p_tri = ps->at(0);
    assert (std::get<0>(p_tri) == std::string("testrandint"));
    __gs::value<int> *pv_tri =
        static_cast<__gs::value<int> *>(std::get<1>(p_tri));
    assert (pv_tri->get_type() == __gs::pspace_t::int_val);
    for (int i = 0; i < 10; i++)
        assert (pv_tri->at(i) == i);

    // __gs::subspaces *ss = nroot->get_subspaces();
}

void
run_static_gridsearch_tests ()
{
    test_update_search_space ();
    std::cout << "gridsearch tests pass" << std::endl;
}

// #endif // __OPTK_TESTING

