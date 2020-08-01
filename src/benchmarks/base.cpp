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

#include <optk/benchmark.hpp>

std::string benchmark::get_name() {
    return m_name;
}

search_space benchmark::get_search_space() {
    std::vector<param_t> base(0);
    return base;
}

vecd_t benchmark::evaluate (vecd_t x) {
    return x;
}

// Test benchmark function: ackley ----------------------------------------------
// TODO move this to its own file when implementing properly.

ackley::ackley() {
    m_name = "ackley";
}

vecd_t ackley::evaluate(vecd_t x) {
    vecd_t result;

    for (vecd_t::iterator i = x.begin(); i != x.end(); i++) {
        double tmp = *i + 5;
        result.push_back(tmp);
    }

    return result;
}

search_space ackley::get_search_space () {
    std::vector<param_t> tmp(0);
    return tmp;
}

