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
 * @brief This file implements the base class for benchmarks.
 */

#include <optk/benchmark.hpp>

// benchmark ------------------------------------------------------------------

optk::benchmark::benchmark (const std::string &name)
{
    m_name = name;
}

std::string
optk::benchmark::get_name ()
{
    return m_name;
}

// benchmark set --------------------------------------------------------------

optk::benchmark_set::~benchmark_set()
{ }

// benchmarks -----------------------------------------------------------------

optk::benchmarks::benchmarks ()
{
    m_arr = optk::bench_list();
}

optk::benchmarks::~benchmarks ()
{
    optk::bench_list::iterator it;
    for (it = m_arr.begin (); it != m_arr.end(); it++) {
        delete *it;
    }
}

void
optk::benchmarks::register_benchmark (optk::benchmark_set *b)
{
    m_arr.push_back(b);
}

optk::bench_list *
optk::benchmarks::collection ()
{
    return &m_arr;
}
