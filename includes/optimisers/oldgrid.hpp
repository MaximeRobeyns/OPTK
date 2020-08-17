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
 * @brief Header file for the gridsearch built-in optimiser.
 */

#ifndef __GRIDSEARCH_H_
#define __GRIDSEARCH_H_

#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

#include <optk/types.hpp>
#include <optk/optimiser.hpp>

// These are the data structures that unpack_params unpacks the search space
// into. They are designed for gridsearch to easily enumerate and iterate over
// the possible parameter values.
//
// TODO put this in the cpp file; it does not need to be in the header

namespace pspace {

enum class pspace_t: char {
    node,
    value
};

enum class vtype: char {
    int_val,
    dbl_val,
    str_val
};

class param {
    public:
        param (const std::string &k, pspace_t t);

        pspace_t get_type () { return m_type; }
        std::string get_key () { return m_key; }

    private:
        const std::string m_key;
        const pspace_t m_type;
};

class node: public param {
    public:
        node (const std::string &k);

        void add_item (param *p);

        param *get_item(const std::string &k);

        void remove_item (const std::string &k);

    private:
        // TODO separate out into concrete values, and further nodes
        // as an invariant, all param * in here have type pspace_t::node.
        std::unordered_map <std::string, param *> nodes;
        // as an invariant, all param * in here have type pspace_t::value.
        std::unordered_map <std::string, param *> values;
};

template <class T>
class value: public param {
    public:
        value (const std::string &k, vtype t);
        value (const std::string &k, vtype t, std::vector<T> v);

        vtype get_type() { return m_type; }
        std::vector<T> *get_vals() { return &m_values; }
        T at(unsigned int i) { return m_values[i]; }
        void set(std::vector<T> vs) { m_values = vs; }

        // TODO add an iterator method here

    private:
        const vtype m_type;
        std::vector<T> m_values;
};

}; // namespace pspace

class pspace {

    public:

        pspace ();

        // types ---------------------------------------------------------------

        /**
         * Param is a name-values tuple.
         * It should allow for either a vector of doubles, or integers, or strings.
         */
        typedef std::tuple<std::string, std::vector<double>> param;

        /**
         * Somewhat similar to optk::plist, params is a list of (name, values)
         * tuples, which represents all parameters, their names and
         * corresponding values.
         */
        typedef std::vector<param> params;

        /**
         * Used to represent a subspce in a pspace.
         */
        typedef std::vector<pspace *> subspaces;

        // methods -------------------------------------------------------------

        /**
         * The constructor
         */
        pspace (std::string name);

        /**
         * The step function is used to step through a search space.
         * @todo make the requirements for this function more concrete before
         * going further
         */
        void step ();
        // bool step ();

        /**
         * registers the enumerated values for a parameter.
         * @param p pointer to the parameter whose values have been enumerated
         */
        void register_param (param *p);

        /**
         * registers a nested search space to add to the \c subspaces member
         * @param subspace Pointer to the nested subspace
         */
        void register_subspace (pspace *subspace);

        /**
         * Get the parameter list; used during testing
         */
        params *get_paramlist ();

        /**
         * Get the subspaces; used during testing
         */
        subspaces *get_subspaces ();

        /**
         * Access this parameter space's name
         */
        std::string get_name ();

#ifdef __OPTK_TESTING
        std::vector<int> get_ctrs() { return m_ctrs; }
        std::vector<int> get_sizes() { return m_sizes; }
#endif

    private:
        /** This is an array of counters used when generating new permutations */
        std::vector<int> m_ctrs;

        /** Stores the number of candidate values per parameter*/
        std::vector<int> m_sizes;

        /** The list of concrete parameters for this 'level' */
        params m_paramlist;

        /** The list of nested search spaces on this 'level' */
        subspaces m_subspaces;

        /** The name corresponding to the (nested) search space. The top-level
            search space has name "root". */
        std::string m_name;
};

class gridsearch: public optk::optimiser {

    public:

        /**
         * The constructor; simply calls the constructor specified for the
         * \ref optimiser class which this class inherits.
         */
        gridsearch ();

        /**
         * The destructor; used to recursively free nested search spaces stored
         * on the heap.
         */
        ~gridsearch ();

        /**
         * Expands the lists of parameters into a format from whcih it is easier
         * to generate all possible configurations sequentially.
         *
         * @param space The search space to unpack. The gridsearch optimiser
         * only accepts parameters of type param::choice, param::categorical,
         * param::randint as well as param::quniform.
         */
        void update_search_space (sspace::sspace_t *space);

        /**
         * Returns the next unique parameter configuration.
         * @param param_id The identifier which will be matched with the
         * selected parameter combination.
         */
        inst::set generate_parameters (int param_id);

        /**
         * Stores \c params if \c value is greater than previous best.
         *
         * @param param_id The identifier of the parameter combination which
         * generated the result.
         * @param params The parameters themselves
         * @param params The result from the objective function
         */
        void receive_trial_results (
            int param_id, inst::set params, double value
        );

#ifdef __OPTK_TESTING
        pspace * get_root () {
            return m_root;
        }
#endif

    private:

        /**
         * Recursively expands the parameters into a connected acyclic graph
         * which is a representation that gridsearch can more easily iterate
         * over.
         *
         * @param param The parameter to expand.
         * @param space The parameter space (level) in which param belongs
         */
        void unpack_param (sspace::param_t *param, pspace *space);

        /**
         * reference to the 'root' search space; the first node in the graph
         */
        pspace *m_root;

        /** This keeps track of previously generated instances / settings of
         * the parameters, identified by the param id given to
         * generate_parameters. */
        std::unordered_map<int, inst::set> trials;
};

#endif // __GRIDSEARCH_H_
