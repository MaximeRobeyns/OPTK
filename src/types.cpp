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
 * @brief Implements the main types used in the program.
 */

#include <optk/types.hpp>
#include <random>

// choice ----------------------------------------------------------------------

optk::choice::choice (std::string n, std::vector<optk::param_t *> *options)
{
    m_name = n;
    std::vector<optk::param_t *> m_options(*options);
    m_type = param::choice;
}

std::vector<optk::param_t *> *
optk::choice::options()
{
    return &m_options;
}

int
optk::choice::count ()
{
    return m_options.size();
}

optk::param_t *
optk::choice::get (long unsigned int i)
{
    if (i < 0 || i > m_options.size())
        throw "i out of range";
    return m_options[i];
}

// randint ---------------------------------------------------------------------

optk::randint::randint (std::string n, int l, int u)
{
    m_name = n;
    m_type = param::randint;
    // initialise mersenne twister prng using the random device.
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> dist (l, u);
}

int
optk::randint::sample()
{
    return dist(generator);
}

// uniform ---------------------------------------------------------------------

optk::uniform::uniform (std::string n, double l, double u)
{
    m_name = n;
    m_type = param::uniform;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> dist (l, u);
}

double
optk::uniform::sample()
{
    return dist (generator);
}


