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

optk::param_t::param_t (std::string n)
{
    m_name = n;
}

// choice ----------------------------------------------------------------------

optk::choice::choice (std::string n, optk::sspace_t *options) : param_t (n)
{
    m_type = pt::choice;
    m_options = options;
    m_type = pt::choice;
}

optk::sspace_t *
optk::choice::options()
{
    return m_options;
}

long unsigned int
optk::choice::count ()
{
    return m_options->size();
}

optk::param_t *
optk::choice::get (long unsigned int i)
{
    if (i < 0 || i > m_options->size())
        throw "i out of range";
    return (*m_options)[i];
}

// randint ---------------------------------------------------------------------

optk::randint::randint (std::string n, int l, int u): param_t (n)
{
    m_type = pt::randint;
    // initialise Mersenne twister prng using the random device.
    m_lower = l;
    m_upper = u;
    generator = std::mt19937 (rd());
    dist = std::uniform_int_distribution<int> (l, u);
}

int
optk::randint::sample()
{
    return dist(generator);
}

// uniform ---------------------------------------------------------------------

optk::uniform::uniform (std::string n, double l, double u): param_t (n)
{
    m_lower = l;
    m_upper = u;
    m_type = pt::uniform;
    generator = std::mt19937 (rd());
    dist = std::uniform_real_distribution<double> (l, u);
}

double
optk::uniform::sample()
{
    return dist (generator);
}

// quniform --------------------------------------------------------------------

optk::quniform::quniform (std::string n, double l, double u, double q):
    uniform (n, l, u)
{
    m_q = q;
    m_type = pt::quniform;
}

double
optk::quniform::sample ()
{
    double value = round(dist (generator) / m_q) * m_q;
    if (value < m_lower) {
        value = m_lower;
    } else if (value > m_upper) {
        value = m_upper;
    }

    return value;
}

// loguniform ------------------------------------------------------------------

optk::loguniform::loguniform (std::string n, double l, double u):
    uniform (n, l, u)
{
    m_type = pt::loguniform;

   if (l <= 0 || u <= 0) {
        throw std::invalid_argument ("bounds cannot be negative or zero");
    }

    dist = std::uniform_real_distribution<double> (log (l), log (u));
}

double
optk::loguniform::sample ()
{
    return exp (dist (generator));
}

// qloguniform ----------------------------------------------------------------

optk::qloguniform::qloguniform (
    std::string n,
    double lower,
    double upper,
    double q
) :
    loguniform (n, lower, upper)
{
    m_type = pt::qloguniform;
    m_q = q;
}

double
optk::qloguniform::sample ()
{
    double value = round(optk::loguniform::sample()/m_q) * m_q;

    if (value < m_lower) {
        value = m_lower;
    } else if (value > m_upper) {
        value = m_upper;
    }

    return value;
}

// normal ----------------------------------------------------------------------

optk::normal::normal (std::string n, double mu, double sigma): param_t (n)
{
    m_mu = mu;
    m_sigma = sigma;
    m_type = pt::normal;
    m_name = n;
    generator = std::mt19937 (rd());
    dist = std::normal_distribution<double> (mu, sigma);
}

double
optk::normal::sample ()
{
    return dist (generator);
}

// qnormal ---------------------------------------------------------------------

optk::qnormal::qnormal (std::string n, double mu, double sigma, double q) :
    normal (n, mu, sigma)
{
    m_type = pt::qnormal;
    m_q = q;
}

double
optk::qnormal::sample ()
{
    return round(normal::sample () / m_q) * m_q;
}

// lognormal -------------------------------------------------------------------

optk::lognormal::lognormal (std::string n, double mu, double sigma) :
    normal (n, mu, sigma)
{
    m_type = pt::lognormal;
}

double
optk::lognormal::sample ()
{
    return exp(normal::sample());
}

// qlognormal ------------------------------------------------------------------

optk::qlognormal::qlognormal (
    std::string n,
    double mu,
    double sigma,
    double q
) :
    lognormal (n, mu, sigma)
{
    m_type = pt::qlognormal;
    m_q = q;
}

double
optk::qlognormal::sample ()
{
    return round(lognormal::sample() / m_q) * m_q;
}

