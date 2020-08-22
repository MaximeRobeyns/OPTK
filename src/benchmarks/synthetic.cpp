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
    for (u_int i = 0; i < dims; i++) {
        sspace::uniform *temp = new sspace::uniform(std::to_string (i), m_lb, m_ub);
        m_sspace.push_back(temp);
    }
}

#define deltype(type, src) \
    { \
    type *tmp_type = static_cast<type *>(src); \
    delete tmp_type; \
    break; \
    }

synthetic::~synthetic()
{
    sspace::sspace_t::iterator it;
    for (it = m_sspace.begin (); it != m_sspace.end (); it++) {
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
                if (dbleq (dval,vals->at (i)))
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

// things to validate:
// For every concrete instance of the parameters, verify that the value
// selected is of the correct type and that it is within applicable bounds.

static void
validate_param_values (inst::value_map *vals, sspace::sspace_t *sspace)
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

void
synthetic::validate_param_set(inst::set x)
{
    // verify values at this level
    inst::value_map *vals = x->get_values();
    validate_param_values (vals, &m_sspace);
}

ackley1::ackley1 (int d) :
    synthetic ("ackley1", 10, -35, 35, 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));
}

double
ackley1::evaluate(inst::set x)
{

    // TODO create a validation function which ensures that a set is
    // compatible with the optimisation problem's search space
    // specification.

    for (u_int i = 0; i < m_dims; i++) {
        // x->get_values()->at(std::to_string(i))
    }
    // std::sqrt(1/(double) m_dims )
    // double exp1 =
    return 0;
};

} // end namespace syn
