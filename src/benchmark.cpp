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

optk::benchmark::benchmark (std::string name, u_int dim)
{
    m_name = name;
    m_dim = dim;
}

std::string
optk::benchmark::get_name ()
{
    return m_name;
}

sspace::sspace_t *
optk::benchmark::get_search_space ()
{
    return &m_params;
}

inst::set
optk::benchmark::evaluate (inst::set x)
{
    return x;
}

optk::benchmarks::benchmarks ()
{
    m_arr = std::vector<benchmark *>();
}

void
optk::benchmarks::register_benchmark (optk::benchmark *b)
{
    m_arr.push_back(b);
}

std::vector <optk::benchmark *> *
optk::benchmarks::collection ()
{
    return &m_arr;
}
