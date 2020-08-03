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

#include <optimisers/gridsearch.hpp>

gridsearch::gridsearch() {
    m_name = "gridsearch";
}

/**
 * Validates that the parameter types are compatible for use with the
 * gridsearch algorithm.
 *
 * Valid parameter types include: choice, quniform and randint.
 */
void gridsearch::update_search_space(search_space space) {

}

void gridsearch::update_search_space(search_space space) {
    // validate that the parameter types are permissible for use with the
    // gridsearch algorithm:

    // Accepted parameter types: choice, quniform, and randint

    // 1. iterate over all the search space parameters:
    for (search_space::iterator i = space.begin(); i != space.end(); i++) {
        if (i->m_name == "gridsearch" || i->m_name == "") {

        }
    }

    m_space = space;
}

vecd_t gridsearch::generate_parameters(int param) {

    // for each parameter; iterate over the range of possible values:
    for (search_space::iterator i = m_space.begin(); i != m_space.end(); i++) {

    }

    vecd_t tmp = vecd_t(0);

    return tmp;
}

void gridsearch::receive_trial_results(
    int param_id,
    vecd_t parameters,
    double value
) {
    return;
}

