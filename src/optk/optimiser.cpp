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

// this file creates an inheritable class for optimisation algorithms; the idea
// being to promote a compatability with NNI.

#include <optk/optimiser.hpp>

optimisers::optimisers() {
    iterator = 0;
}

void optimisers::register_opt(optimiser *o) {
    m_arr.push_back(o);
}

optimiser *optimisers::get_next() {
    if (iterator < m_arr.size())
        return m_arr[iterator++];
    else
        return NULL;
}
