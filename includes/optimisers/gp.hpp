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
 * @brief Header file for the Gaussian process-based optimiser
 */

#ifndef __GP_H_
#define __GP_H_

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <optk/optimiser.hpp>
#include <optk/types.hpp>
#include <optk/benchmark.hpp>

class gp_opt: public optk::optimiser {

    public:

        /**
         * Initialises the Python interpreter, and skopt.gp_minimize. Also
         * prepares python arguments.
         */
        gp_opt (const char *aq_func);

        /**
         * The destructor terminates the Python interpreter and frees heap
         * allocations.
         */
        ~gp_opt ();

        /**
         * Clears references to python objects and re-starts interpreter
         * between runs to avoid bugs.
         */
        void clear () override;

        /**
         * This function will covert the C++ search space into a python list of
         * tuples describing as required by skopt.gp_minimize.
         */
        void update_search_space (sspace::sspace_t *space) override;

        /**
         * Generate parameters will call skopt's gaussian process minimisation
         * function (skopt.gp_minimize) to generate new parameter instances.
         * @warning This should not be used while the temporary python-based GP
         * optimiser is being used.
         * @todo implement a c-based GP optimiser
         */
        inst::set generate_parameters (int param_id) override;

        void receive_trial_results (
                int param_id,
                inst::set params,
                double value
            ) override;

    private:
        // the number of dimensions (parameters) for the current problem
        uint m_dims;

        // Python objects for arguments for generate_parameters
        PyObject *gp_aq_func, *X_obs, *Y_obs, *gp_bounds;

        // References to auxiliary python objects
        PyObject *sys, *path, *name, *module, *module_dict;

        // The python version of the generate params method
        PyObject *py_generate_params;
};

#endif // __GP_H_
