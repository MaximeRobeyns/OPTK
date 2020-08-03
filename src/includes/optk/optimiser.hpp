/*
 * Copyright (C) 2020 Maxime Robeyns <maximerobeyns@gmail.com>
 *
 * Written for the ACRC, University of Bristol
 *
 * Licensed under the Educational Community License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.osedu.org/licenses/ECL-2.0
 *_
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the LIcense For The Specific Language Governing permissions and
 * limitations under the License.
 */

#ifndef __OPTIMISER_H_
#define __OPTIMISER_H_

#include <cstdio>
#include <optk/types.hpp>

// abstract base class that every optimisation algorithm inherits
// TODO document me!
class optimiser {
    public:
        optimiser();
        std::string get_name () { return m_name; }
        search_space get_search_space () { return m_space; }

        virtual void update_search_space (search_space space);
        // TODO potentially return another type allowing for categorical variables...
        virtual vecd_t generate_parameters(int param_id);
        virtual void receive_trial_results(int param_id, vecd_t parameters, double value);

    private:
        std::string m_name;
        search_space m_space;
};

// class to hold the list of optimisation algorithms
// TODO document me!
class optimisers {
    public:
        optimisers();
        void register_opt(optimiser *o);
        optimiser *get_next();
    private:
        int iterator;
        std::vector<optimiser *>m_arr;
};

#endif // __OPTIMISER_H_
