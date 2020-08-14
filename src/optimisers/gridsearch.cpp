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

#include <nlohmann/json.hpp>
#include <tuple>

// parameter space class implementation ----------------------------------------

pspace::pspace ()
{
    m_name = "";
    m_paramlist = params();
}

pspace::pspace (std::string name)
{
    m_name = name;
    // instatiate the vector of parameters
    m_paramlist = params();
}

void
pspace::register_param (param *p)
{
    m_paramlist.push_back(*p);
}

void
pspace::register_subspace(pspace *s)
{
    m_subspaces.push_back(s);
}

pspace::params *
pspace::get_paramlist ()
{
    return &m_paramlist;
}

pspace::subspaces *
pspace::get_subspaces ()
{
    return &m_subspaces;
}

std::string
pspace::get_name ()
{
    return m_name;
}

// gridsearch implementation ---------------------------------------------------

gridsearch::gridsearch (): optimiser ("gridsearch")
{
    // initialise a default root space;
    // @todo verify that this is necessary.
    m_root = new pspace("root");
}

static void
free_spaces (pspace *root)
{
    pspace::subspaces *ss = root->get_subspaces();
    pspace::subspaces::iterator it;
    for (it = ss->begin (); it != ss->end (); it++) {
        free_spaces (*it);
    }
    delete root;
}

gridsearch::~gridsearch ()
{
    // recurse through nested search spaces, freeing them
    free_spaces (m_root);
}

void
gridsearch::unpack_param (optk::param_t *param, pspace *space)
{
    // 1. Assert that the parameter is of the correct type for gridsearch
    param::param_type t = param->get_type ();
    assert (
        t == param::randint ||
        t == param::quniform ||
        t == param::categorical ||
        t == param::choice
    );

    switch (t) {
        case param::randint:
        {
            optk::randint *ri = static_cast<optk::randint *>(param);
            std::string name = ri->get_name ();
            std::vector<double> values;

            for (int i = ri->m_lower; i < ri->m_upper; i++) {
                values.push_back ((double) i);
            }

            pspace::param p = std::make_tuple (name, values);
            space->register_param (&p);
            break;
        }
        case param::quniform:
        {
            optk::quniform *qu = static_cast<optk::quniform *>(param);
            std::string name = qu->get_name ();
            std::vector <double> values;

            for (int i = qu->m_lower; i < qu->m_upper; i++) {
                values.push_back ((double) i);
            }

            pspace::param p = std::make_tuple (name, values);
            space->register_param (&p);
            break;
        }
        case param::categorical:
        {
            // NOTE: categorical must be double values
            // anything else will segfault
            optk::categorical<double> *cat =
                static_cast<optk::categorical<double> *>(param);
            std::string name = cat->get_name ();
            std::vector <double> values = *cat->values();
            pspace::param p = std::make_tuple (name, values);
            space->register_param (&p);
            break;
        }
        case param::choice:
        {
            optk::choice *c = static_cast<optk::choice *>(param);

            // creates a new pspace on the heap
            pspace *nspace = new pspace (c->get_name ());
            space->register_subspace (nspace);

            optk::sspace_t *subspace = c->options ();
            optk::sspace_t::iterator it;
            for (it = subspace->begin (); it != subspace->end (); it++) {
                gridsearch::unpack_param (*it, nspace);
            }
            break;
        }

        default:
            // we should never arrive at this case due to the assert
            // hence it is left unhandled
            break;
    }
}

void
gridsearch::update_search_space (optk::sspace_t *space)
{
    // free previous search spaces
    free_spaces (m_root);

    m_root = new pspace ("root");
    optk::sspace_t::iterator it;
    for (it = space->begin (); it != space->end (); it++) {
        gridsearch::unpack_param (*it, m_root);
    }
}

// dummy functions for compilation ---------------------------------------------

param::list gridsearch::generate_parameters (int param_d) {
    return param::list ();
}

void gridsearch::receive_trial_results (int pid, param::list params, double value) {
    return;
}

