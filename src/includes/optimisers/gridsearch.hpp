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
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the LIcense For The Specific Language Governing permissions and
 * limitations under the License.
 */

#ifndef __GRIDSEARCH_
#define __GRIDSEARCH_

#include <optk/optimiser.hpp>
#include <optk/types.hpp>

class gridsearch: public optimiser {

    public:
        gridsearch();
        void update_search_space (search_space space);
        vecd_t generate_parameters(int param_id);
        void receive_trial_results(int param_id, vecd_t parameters, double value);
        search_space m_space;
};

#endif // __GRIDSEARCH_
