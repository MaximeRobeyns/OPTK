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
#include <unordered_map>

// concrete values tyeps ======================================================

inst::param::param
(const std::string &k, inst::inst_t t) : key(k), type(t)
{}

inst::node::node (const std::string &k):
    inst::param (k, inst::inst_t::node)
{
    values = std::unordered_map<std::string, param *>();
}

void
inst::node::add_item (param *p)
{
    values.insert({p->get_key(), p});
}

void
inst::node::add_items (std::vector<param *> items)
{
    std::vector<param *>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
        values.insert({(*it)->get_key(), *it});
}

// O(log(n)) lookup
inst::param *
inst::node::get_item (const std::string &k)
{
    try {
        return values.at(k);
    } catch (const std::out_of_range &e) {
        return NULL;
    }
}

void
inst::node::remove_item (const std::string &k)
{
    values.erase(k);
}

inst::int_val::int_val (const std::string &k, int v):
    inst::param (k, inst::inst_t::int_val),
    val (v)
{}

inst::dbl_val::dbl_val (const std::string &k, double v):
    inst::param (k, inst::inst_t::dbl_val),
    val (v)
{}

inst::str_val::str_val (const std::string &k, const std::string &v):
    inst::param (k, inst::inst_t::str_val),
    val (v)
{}

// search space types =========================================================

sspace::param_t::param_t (std::string n, pt t): m_type(t), m_name(n) {};

// choice ----------------------------------------------------------------------

sspace::choice::choice (std::string n, sspace_t *options) :
    param_t (n, pt::choice)
{
    m_options = options;
}

sspace::sspace_t *
sspace::choice::options()
{
    return m_options;
}

long unsigned int
sspace::choice::count ()
{
    return m_options->size();
}

sspace::param_t *
sspace::choice::get (long unsigned int i)
{
    if (i < 0 || i > m_options->size())
        throw "i out of range";
    return (*m_options)[i];
}

// randint ---------------------------------------------------------------------

sspace::randint::randint (std::string n, int l, int u) :
    param_t (n, pt::randint)
{
    // initialise Mersenne twister prng using the random device.
    m_lower = l;
    m_upper = u;
    generator = std::mt19937 (rd());
    dist = std::uniform_int_distribution<int> (l, u);
}

int
sspace::randint::sample()
{
    return dist(generator);
}

// uniform ---------------------------------------------------------------------

sspace::uniform::uniform (std::string n, double l, double u, pt t)
    : param_t (n, t)
{
    m_lower = l;
    m_upper = u;
    generator = std::mt19937 (rd());
    dist = std::uniform_real_distribution<double> (l, u);
}

sspace::uniform::uniform (std::string n, double l, double u)
    : param_t (n, pt::uniform)
{
    m_lower = l;
    m_upper = u;
    generator = std::mt19937 (rd());
    dist = std::uniform_real_distribution<double> (l, u);
}

double
sspace::uniform::sample()
{
    return dist (generator);
}

// quniform --------------------------------------------------------------------

sspace::quniform::quniform (std::string n, double l, double u, double q):
    uniform (n, l, u, pt::quniform)
{
    m_q = q;
}

double
sspace::quniform::sample ()
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

sspace::loguniform::loguniform (std::string n, double l, double u):
    uniform (n, l, u, pt::loguniform)
{
   if (l <= 0 || u <= 0)
        throw std::invalid_argument ("bounds cannot be negative or zero");
    dist = std::uniform_real_distribution<double> (log (l), log (u));
}

sspace::loguniform::loguniform (std::string n, double l, double u, pt t):
    uniform (n, l, u, t)
{
   if (l <= 0 || u <= 0)
        throw std::invalid_argument ("bounds cannot be negative or zero");
    dist = std::uniform_real_distribution<double> (log (l), log (u));
}

double
sspace::loguniform::sample ()
{
    return exp (dist (generator));
}

// qloguniform ----------------------------------------------------------------

sspace::qloguniform::qloguniform (
    std::string n,
    double lower,
    double upper,
    double q
) :
    loguniform (n, lower, upper, pt::qloguniform)
{
    m_q = q;
}

double
sspace::qloguniform::sample ()
{
    double value = round(loguniform::sample()/m_q) * m_q;

    if (value < m_lower) {
        value = m_lower;
    } else if (value > m_upper) {
        value = m_upper;
    }

    return value;
}

// normal ----------------------------------------------------------------------

sspace::normal::normal (std::string n, double mu, double sigma) :
    param_t (n, pt::normal)
{
    m_mu = mu;
    m_sigma = sigma;
    generator = std::mt19937 (rd());
    dist = std::normal_distribution<double> (mu, sigma);
}

sspace::normal::normal (std::string n, double mu, double sigma, pt t) :
    param_t (n, t)
{
    m_mu = mu;
    m_sigma = sigma;
    generator = std::mt19937 (rd());
    dist = std::normal_distribution<double> (mu, sigma);
}

double
sspace::normal::sample ()
{
    return dist (generator);
}

// qnormal ---------------------------------------------------------------------

sspace::qnormal::qnormal (std::string n, double mu, double sigma, double q) :
    normal (n, mu, sigma, pt::qnormal)
{
    m_q = q;
}

double
sspace::qnormal::sample ()
{
    return round(normal::sample () / m_q) * m_q;
}

// lognormal -------------------------------------------------------------------

sspace::lognormal::lognormal (std::string n, double mu, double sigma) :
    normal (n, mu, sigma, pt::lognormal)
{}

sspace::lognormal::lognormal (std::string n, double mu, double sigma, pt t) :
    normal (n, mu, sigma, t)
{}

double
sspace::lognormal::sample ()
{
    return exp(normal::sample());
}

// qlognormal ------------------------------------------------------------------

sspace::qlognormal::qlognormal (
    std::string n,
    double mu,
    double sigma,
    double q
) :
    lognormal (n, mu, sigma, pt::qlognormal)
{
    m_q = q;
}

double
sspace::qlognormal::sample ()
{
    return round(lognormal::sample() / m_q) * m_q;
}

