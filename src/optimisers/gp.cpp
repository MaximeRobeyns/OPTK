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
 * @brief Implementes a gp-based minimiser, using skopt.gp_minimize
 */

#include <optimisers/gp.hpp>

// TODO get rid of this
#include <iostream>

gp_opt::gp_opt (const char *aq_func):
    optk::optimiser ("gp_optimiser")
{
    Py_Initialize ();

    // TODO deal with reference counts to avoid memory leaks!

    // initialise the python path:
    sys = PyImport_ImportModule ("sys");
    path = PyObject_GetAttrString (sys, "path");
    PyList_Append (path, PyUnicode_FromString ("src/python"));

    // load the gp module
    name = PyUnicode_FromString ("gp");
    module = PyImport_Import (name);
    module_dict = PyModule_GetDict (module);

    // assume EI acquisition function for now:
    // TODO load different acquisition functions depending on the arguments to
    // this constructor.
    gp_aq_func = PyDict_GetItemString(module_dict, "expected_improvement");
    assert(PyCallable_Check(gp_aq_func));
    py_generate_params = PyDict_GetItemString(module_dict, "generate_parameters");
    assert(PyCallable_Check(py_generate_params));
}

gp_opt::~gp_opt ()
{
    // decrement reference counts; sole reference so should free on heap too.
    Py_DECREF (py_generate_params);
    Py_DECREF (gp_aq_func);
    Py_DECREF (module);
    Py_DECREF (name);
    Py_DECREF (path);
    Py_DECREF (sys);
    Py_Finalize();
}

void
gp_opt::clear ()
{
    Py_DECREF(gp_bounds);
    Py_DECREF(X_obs);
    Py_DECREF(Y_obs);

    optk::optimiser::clear();
}

/**
 * This function validates whether the search space provided is supported by
 * the gp optimiser. Specifically only pt::uniform, and pt::loguniform are
 * compatible and hence allowed.
 *
 * Also note that since the gaussian process optimiser doesn't support nested
 * search spaces, we disallow the pt::choice parameter type.
 */
static bool validate_space (sspace::sspace_t *space)
{
    sspace::sspace_t::iterator it;
    bool valid = true;
    for (it = space->begin (); it != space->end (); it++) {
        pt t = (*it)->get_type ();
        valid &= (t == pt::uniform || t == pt::loguniform);
    }
    return valid;
}


void
gp_opt::update_search_space (sspace::sspace_t *space)
{
    X_obs = PyList_New(0);
    Y_obs = PyList_New(0);

    m_dims = space->size();

    // only pt::uniform and pt::loguniform are compatible
    if (!validate_space (space))
        throw std::invalid_argument (
                "provided search space is not compatible with gp optimiser"
                );

    gp_bounds = PyList_New(space->size());

    for (uint i = 0; i < space->size(); i++) {
        sspace::uniform *uparam = static_cast<sspace::uniform *>(space->at(i));
        PyObject *tmp_bounds = PyList_New(2);
        PyList_SetItem (tmp_bounds, 0, PyFloat_FromDouble(uparam->m_lower));
        PyList_SetItem (tmp_bounds, 1, PyFloat_FromDouble(uparam->m_upper));

        // add to global bounds
        PyList_SetItem (gp_bounds, i, tmp_bounds);
    }
}

inst::set
gp_opt::generate_parameters(int param_id)
{
    // initialise the gp arguments
    PyObject *gp_args = PyTuple_New(4);
    PyTuple_SetItem(gp_args, 0, gp_aq_func);
    PyTuple_SetItem(gp_args, 1, X_obs);
    PyTuple_SetItem(gp_args, 2, Y_obs);
    PyTuple_SetItem(gp_args, 3, gp_bounds);

    Py_INCREF(gp_aq_func);
    Py_INCREF(X_obs);
    Py_INCREF(Y_obs);
    Py_INCREF(gp_bounds);

    PyObject *params = PyObject_CallObject(py_generate_params, gp_args);
    Py_DECREF(gp_args);
    Py_INCREF(params);

    // append the generated parameters to X_obs
    PyList_Append (X_obs, params);

    // convert the python parameters to a parameter set
    inst::node *root = new inst::node ("GP parameters");
    for (uint i = 0; i < m_dims; i++) {
        PyObject *tmppyval = PyList_GetItem (params, i);
        double tmpval = PyFloat_AsDouble (tmppyval);
        root->add_item (
                new inst::dbl_val (std::to_string(i), tmpval)
                );
    }
    add_to_trials (param_id, root);

    return root;
}

void
gp_opt::receive_trial_results(int param_id, inst::set params, double value)
{
    // add the coresponding response value to Y_obs (the x values were added
    // during generate_parmeters)
    PyList_Append(Y_obs, PyFloat_FromDouble(value));
}

