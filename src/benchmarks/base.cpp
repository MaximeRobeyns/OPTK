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

#include <benchmarks/base.hpp>

std::string benchmark::get_name() {
    return m_name;
}

vecd_t benchmark::get_search_space() {
    vecd_t base(0);
    return base;
}

vecd_t benchmark::evaluate (vecd_t x) {
    return x;
}

// Test benchmark function: ackley ----------------------------------------------

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

vecd_t ackley::get_search_space () {
    vecd_t result;
    return result;
}


/*

class Benchmark {

    // fields
    u_int dim = 1;
    std::string m_name = "";
    vecd_t fmin = std::vector<double>(0.0);

    // virtual methods
    virtual vecd_t evaluate (const vecd_t &a) = 0;
    virtual vecd_t get_search_space () = 0;
};

class ackley : protected Benchmark {

    std::string name = "";
    int dim = 0;
    vecd_t min_loc = std::vector<double>(0.0);

    public:
        vecd_t evaluate (const vecd_t &x) const;
        std::string get_name () const;

};

*/
