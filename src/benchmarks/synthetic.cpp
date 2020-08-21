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
 * @brief In this file, we implement the ~175 synthetic test functions given in
 * Jamil et al. 2013 <https://arxiv.org/abs/1308.4008>
 */

#include <benchmarks/synthetic.hpp>

/** This namespace contains all free functions and types relating to the
 * synthetic test functions. */
namespace syn {

synthetic::synthetic (
        const std::string &n,
        u_int dims,
        double lb,
        double ub,
        double opt) :
    benchmark(n), m_dims(dims), m_lb(lb), m_ub(ub), m_opt(opt)
{
    // this is not gonna work is it??
    for (u_int i = 0; i < dims; i++) {
        sspace::uniform temp(std::to_string (i), m_lb, m_ub);
        m_sspace.push_back(&temp);
    }
}

ackley1::ackley1 (int d) :
    synthetic ("ackley", 10, -35, 35, 0)
{ }

double
ackley1::evaluate (inst::set x)
{
    // TODO actually implement
    return 0;
}

/*

sspace::sspace_t *
ackley1::get_search_space ()
{
    // for every dimension, create a uniform parameter within the bounds
}

vecd_t
ackley::evaluate(inst::set x)
{
    vecd_t result;

    for (vecd_t::iterator i = x.begin(); i != x.end(); i++) {
        double tmp = *i + 5;
        result.push_back(tmp);
    }

    return result;
}
*/

} // end namespace syn
