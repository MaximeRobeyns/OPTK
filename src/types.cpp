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

int
inst::node::getint (const std::string &key)
{
    return static_cast<inst::int_val *>(values.at(key))->get_val();
}

int
inst::node::getint (int i)
{
    std::string key = std::to_string(i);
    return getint(key);
}

double
inst::node::getdbl (const std::string &key)
{
    return static_cast<inst::dbl_val *>(values.at(key))->get_val();
}

double
inst::node::getdbl (int i)
{
    std::string key = std::to_string(i);
    return getdbl(key);
}

std::string
inst::node::getstr (const std::string &key)
{
    return static_cast<inst::str_val *>(values.at(key))->get_val();
}

std::string
inst::node::getstr (int i)
{
    std::string key = std::to_string(i);
    return getstr(key);
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

void
inst::free_node (inst::node *n)
{
    // iterate through the elements of this node, freeing them as we do so
    std::unordered_map<std::string, param *> *vals = n->get_values();
    std::unordered_map<std::string, param *>::iterator it;

    for (it = vals->begin (); it != vals->end (); it++) {
        inst::param *p = std::get<1>(*it);
        switch (p->get_type()) {
            case inst::inst_t::int_val:
            {
                delete static_cast<inst::int_val *>(p);
                break;
            }
            case inst::inst_t::dbl_val:
            {
                delete static_cast<inst::dbl_val *>(p);
                break;
            }
            case inst::inst_t::str_val:
            {
                delete static_cast<inst::str_val *>(p);
                break;
            }
            case inst::inst_t::node:
            {
                inst::node *nn = static_cast<inst::node *>(p);
                free_node (nn);
                break;
            }
        }
    }
    delete n;
}

// search space types =========================================================

sspace::param_t::param_t (std::string n, pt t) : m_name(n), m_type(t) {};

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

// validation =================================================================

static bool
dbleq (double a, double b)
{
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

/**
 * Searches a vector of sspace::param_t values for an element matching the
 * provided key. Throws an exception upon error.
 * @param k The key to search for
 * @param sspace The vector of sspace::param_t values to search through.
 * @exception std::invalid_argument When the key cannot be found.
 */
static sspace::param_t *
find_key (const std::string &k, sspace::sspace_t *sspace)
{
    sspace::sspace_t::iterator it;
    for (it = sspace->begin (); it != sspace->end (); it++) {
        if ((*it)->get_name() == k) {
            return *it;
        }
    }
    throw std::invalid_argument(
            "No key match for parameter " + (*it)->get_name()
            );
}

/** Validates an integer-valued parameter.
 * @param ival An integer
 * @param iparam The corresponding specification for the parameter
 * @exception std::invalid_argument if ival is invalid under iparam.
 */
static void
validate_int_value (int ival, sspace::param_t *iparam)
{
    pt param_type = iparam->get_type();
    switch (param_type) {
        case pt::categorical_int:
        {
            sspace::categorical<int> *cint =
                static_cast<sspace::categorical<int> *>(iparam);
            std::vector<int> *vals = cint->values();
            for (u_int i = 0; i < vals->size (); i++)
                if (ival == vals->at (i))
                    return;
            throw std::invalid_argument (
                    "Categorical integer value for " +
                    iparam->get_name() +
                    "not in allowed values"
                    );
            break;
        }
        case pt::randint:
        {
            sspace::randint *rint = static_cast<sspace::randint *>(iparam);
            if (ival < rint->m_lower || ival > rint->m_upper)
                throw std::invalid_argument (
                        "Value for " +
                        iparam->get_name() +
                        "out of range."
                        );
            break;
        }
        default:
        {
            throw std::invalid_argument (
                    "Integer value was incorrectly provided for parameter: " +
                    iparam->get_name()
                    );
            break;
        }
    }
}

/** Validates a double-valued parameter.
 * @param dval A double-precision floating point value
 * @param dparam The corresponding specification for the parameter
 * @exception std::invalid_argument if dval is invalid under dparam.
 */
static void
validate_dbl_value (double dval, sspace::param_t *dparam)
{
    pt param_type = dparam->get_type();
    switch (param_type) {
        case pt::categorical_dbl:
        {
            sspace::categorical<double> *cdouble =
                static_cast<sspace::categorical<double> *>(dparam);
            std::vector<double> *vals = cdouble->values();
            for (u_int i = 0; i < vals->size (); i++)
                if (dbleq (dval, vals->at (i)))
                    return;
            throw std::invalid_argument (
                    "Categorical double value for " +
                    dparam->get_name() +
                    "not in allowed values."
                    );
            break;
        }
        case pt::normal:
        case pt::lognormal:
            break;
        case pt::qnormal:
        {
            sspace::qnormal *qnormal = static_cast<sspace::qnormal *>(dparam);
            double mult = (double) std::round (dval / qnormal->m_q);
            if (!dbleq (dval - mult * qnormal->m_q, 0))
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() +
                        " is not properly quantised."
                        );
            break;
        }
        case pt::qlognormal:
        {
            sspace::qlognormal *qlnormal =
                static_cast<sspace::qlognormal *>(dparam);
            double mult = (double) std::round (dval / qlnormal->m_q);
            if (!dbleq (dval - mult * qlnormal->m_q, 0))
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() +
                        " is not properly quantised."
                        );
            break;
        }
        case pt::uniform:
        {
            sspace::uniform *uni = static_cast<sspace::uniform *>(dparam);
            if (dval < uni->m_lower || dval > uni->m_upper)
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() + "out of range."
                        );
            break;
        }
        case pt::loguniform:
        {
            sspace::loguniform *luni = static_cast<sspace::loguniform *>(dparam);
            if (dval < luni->m_lower || dval > luni->m_upper)
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() + "out of range."
                        );
            break;
        }
        case pt::quniform:
        {
            sspace::quniform *quni = static_cast<sspace::quniform *>(dparam);
            if (dval < quni->m_lower || dval > quni->m_upper)
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() + "out of range."
                        );
            double mult = (double) std::round (dval / quni->m_q);
            if (!dbleq (dval - mult * quni->m_q, 0))
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() +
                        " is not properly quantised."
                        );
            break;
        }
        case pt::qloguniform:
        {
            sspace::qloguniform *qluni =
                static_cast<sspace::qloguniform *>(dparam);
            if (dval < qluni->m_lower || dval > qluni->m_upper)
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() + "out of range."
                        );
            double mult = (double) std::round (dval / qluni->m_q);
            if (!dbleq (dval - mult * qluni->m_q, 0))
                throw std::invalid_argument (
                        "Value for " + dparam->get_name() +
                        " is not properly quantised."
                        );
            break;
        }
        default:
        {
            throw std::invalid_argument (
                    "Double value was incorrectly provided for parameter: " +
                    dparam->get_name()
                    );
            break;
        }
    }
}

/** Validates a string-valued parameter.
 * @param sval A string value
 * @param sparam The corresponding specification for the parameter
 * @exception std::invalid_argument if sval is invalid under sparam.
 */
static void
validate_str_value (std::string sval, sspace::param_t *sparam)
{
    pt param_type = sparam->get_type();
    switch (param_type) {
        case pt::categorical_str:
        {
            sspace::categorical<std::string> *cstring =
                static_cast<sspace::categorical<std::string> *>(sparam);
            std::vector<std::string> *vals = cstring->values();
            for (u_int i = 0; i < vals->size (); i++)
                if (sval == vals->at(i))
                    return;
            throw std::invalid_argument (
                    "Categorical string value for " +
                    sparam->get_name() +
                    "not in allowed values list."
                    );
            break;
        }
        default:
        {
            throw std::invalid_argument (
                    "String value was incorrectly provided for parameter: " +
                    sparam->get_name()
                    );
            break;
        }
    }
}

void
sspace::validate_param_values (inst::value_map *vals, sspace::sspace_t *sspace)
{
    inst::value_map::iterator it;
    for (it = vals->begin(); it != vals->end(); it++) {
        inst::param *p = std::get<1>(*it);
        inst::inst_t t = p->get_type();
        switch (t) {
            case inst::inst_t::int_val:
            {
                inst::int_val *ival = static_cast<inst::int_val *>(p);
                sspace::param_t *iparam = find_key (std::get<0>(*it), sspace);
                validate_int_value (ival->get_val (), iparam);
                break;
            }
            case inst::inst_t::dbl_val:
            {
                inst::dbl_val *dval = static_cast<inst::dbl_val *>(p);
                sspace::param_t *dparam = find_key (std::get<0>(*it), sspace);
                validate_dbl_value (dval->get_val(), dparam);
                break;
            }
            case inst::inst_t::str_val:
            {
                inst::str_val *sval = static_cast<inst::str_val *>(p);
                sspace::param_t *sparam = find_key (std::get<0>(*it), sspace);
                validate_str_value (sval->get_val(), sparam);
                break;
            }
            case inst::inst_t::node:
            {
                inst::node *n = static_cast<inst::node *>(p);
                sspace::param_t *node = find_key (std::get<0>(*it), sspace);
                if (node->get_type () != pt::choice)
                    throw std::invalid_argument("Invalid type for subspace");
                sspace::choice *cs = static_cast<sspace::choice *>(node);
                validate_param_values (n->get_values(), cs->options());
                break;
            }
        }
    }
}

#define deltype(type, src) \
    { \
    type *tmp_type = static_cast<type *>(src); \
    delete tmp_type; \
    break; \
    }

void
sspace::free_ss (sspace::sspace_t *ss)
{
    sspace::sspace_t::iterator it;
    for (it = ss->begin (); it != ss->end (); it++) {
        sspace::param_t *tmp = (*it);
        pt t = tmp->get_type();
        switch (t) {
            case pt::categorical_int:
                deltype(sspace::categorical<int>, tmp);
            case pt::categorical_dbl:
                deltype(sspace::categorical<double>, tmp);
            case pt::categorical_str:
                deltype(sspace::categorical<std::string>, tmp);
            case pt::choice:
                deltype(sspace::choice, tmp);
            case pt::normal:
                deltype(sspace::normal, tmp);
            case pt::qnormal:
                deltype(sspace::qnormal, tmp);
            case pt::lognormal:
                deltype(sspace::lognormal, tmp);
            case pt::qlognormal:
                deltype(sspace::qlognormal, tmp);
            case pt::uniform:
                deltype(sspace::uniform, tmp);
            case pt::quniform:
                deltype(sspace::quniform, tmp);
            case pt::loguniform:
                deltype(sspace::loguniform, tmp);
            case pt::qloguniform:
                deltype(sspace::qloguniform, tmp);
            case pt::randint:
                deltype(sspace::randint, tmp);
        }
    }
    // delete ss;
}

