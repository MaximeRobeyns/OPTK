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

#ifndef __BENCHMARK_H_
#define __BENCHMARK_H_

#include <string>
#include <sys/types.h>
#include <vector>
#include <math.h>
#include <iostream>

#include <optk/types.hpp>

// This is the class that individual benchmarks will inherit
// TODO document me!
class benchmark {
    public:
        std::string m_name;
        u_int       m_dim = 1;
        vecd_t      m_fmin = vecd_t(m_dim);
        std::vector<param_t> param_space;

        // function prototypes
        std::string get_name();
        search_space get_search_space();
        vecd_t evaluate(vecd_t x);

};

// This is the class that wraps the list of benchmarks
// TODO document me!
class benchmarks {
    public:
        benchmarks();
        void register_bench(benchmark *b);
        benchmark *get_next();
    private:
        int iterator;
        std::vector<benchmark *> m_arr;
};

// example test benchmark -----------------------------------------------------
// TODO move to its own file when implementing properly

class ackley: public benchmark {
    public:
        ackley();
        search_space get_search_space();
        vecd_t evaluate(vecd_t x);
};

#endif // __BENCHMARK_H_
