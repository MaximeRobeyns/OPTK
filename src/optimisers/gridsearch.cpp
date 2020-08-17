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

enum class vtype: char {
    int_val,
    dbl_val,
    str_val
};

param::param (const std::string &k, pspace_t t) :
    m_key(k), m_type(t)
{}

class node: public param {
    public:
        node (const std::string &k) :
            param (k, pspace_t::node)
        {
            nodes = std::unordered_map<std::string, param *>();
            values = std::unordered_map<std::string, param *>();
        }

        void
        add_item (param *p)
        {
            pspace_t t = p->get_type ();
            switch (t) {
                case pspace_t::node:
                    {
                        nodes.insert({p->get_key(), p});
                    }
                default:
                    {
                        values.insert({p->get_key(), p});
                    }
            }
        }

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

        std::unordered_map <std::string, param *> *
        get_subspaces ()
        { return &nodes; }

        std::unordered_map <std::string, param *> *
        get_values ()
        { return &values; }

        // TODO add an iteration method here

    private:
        // as an invariant, all param * in here have type pspace_t::node.
        std::unordered_map <std::string, param *> nodes;
        // as an invariant, all param * in here have type pspace_t::value.
        std::unordered_map <std::string, param *> values;
};

template <class T>
class value: public param {
    public:
        value (const std::string &k, vtype t) :
            param (k, pspace_t::value), m_type(t)
        {
            m_values = std::vector<T>();
        }

        value (const std::string &k, std::vector<int> v) :
            param (k, pspace_t::value), m_type(vtype::int_val)
        {
            m_values = std::vector<int>(v);
        }

        value (const std::string &k, std::vector<double> v) :
            param (k, pspace_t::value), m_type (vtype::dbl_val)
        {
            m_values = std::vector<double>(v);
        }

        value (const std::string &k, std::vector<std::string> v) :
            param (k, pspace_t::value), m_type (vtype::str_val)
        {
            m_values = std::vector<std::string>(v);
        }

        vtype get_type() { return m_type; }
        std::vector<T> *get_vals() { return &m_values; }
        T at(unsigned int i) { return m_values[i]; }
        void set(std::vector<T> vs) { m_values = vs; }

        // TODO add an iterator method here
        // Sends a signal when the last value is returned (so that the calling
        // process can know that the iteration is complete)
        // count the lenght of the values array

    private:
        const vtype m_type;
        std::vector<T> m_values;
};

typedef std::unordered_map<std::string, param *> subspaces;

typedef std::unordered_map<std::string, param *> params;

} // namespace __gs


// Actual gridsearch class methods ============================================

// free, static functions -----------------------------------------------------

static void
free_spaces (__gs::node *root)
{
    __gs::subspaces *ss = root->get_subspaces ();
    __gs::subspaces::iterator it;
    for (it = ss->begin(); it != ss->end(); it++) {
        __gs::node *tmp = static_cast<__gs::node *>((*it).second);
        free_spaces (tmp);
    }

    // free all the concrete values at this level
    __gs::params *ps = root->get_values ();
    __gs::params::iterator val_it;
    for (val_it = ps->begin (); val_it != ps->end (); val_it++) {
        __gs::value<int> *tmp = static_cast<__gs::value<int> *>((*val_it).second);
        delete tmp;
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
unpack_pararm (sspace::param_t *param, __gs::node *parent)
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
            for (int i = qu->m_lower; i < qu->m_upper; i++) {
                values.push_back ((double) i);
            }
            __gs::param *tmp_val = new __gs::value<double>(name, values);
            parent->add_item (tmp_val);
            break;
        }
        case pt::categorical_int:
            unpack_categorical<int>(param, parent);
        case pt::categorical_dbl:
            unpack_categorical<double>(param, parent);
        case pt::categorical_str:
            unpack_categorical<std::string>(param, parent);
        case pt::choice:
        {
            sspace::choice *c = static_cast <sspace::choice *>(param);

            // instantiate a new node on the heap
            __gs::node *nspace = new __gs::node (c->get_name ());
            parent->add_item (nspace);

            sspace::sspace_t *subspace = c->options ();
            sspace::sspace_t::iterator it;
            for (it = subspace->begin (); it != subspace->end (); it++)
                unpack_pararm(*it, nspace);
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

    // TODO come back to this once we have finished the update_search_space method
}


void
gridsearch::update_search_space (sspace::sspace_t *space)
{
    // free any existing / old search spaces
    if (m_root)
        free (m_root);

    // initialise a new parameter node on the heap
    __gs::node *new_root = new __gs::node("root");
    sspace::sspace_t::iterator it;
    for (it = space->begin (); it != space->end (); it++)
        unpack_param (*it, new_root);
}











