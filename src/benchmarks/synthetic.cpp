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

synthetic::synthetic (const std::string &n, u_int dims, double opt):
    benchmark(n), m_dims(dims), m_opt(opt)
{ }

#define deltype(type, src) \
    { \
    type *tmp_type = static_cast<type *>(src); \
    delete tmp_type; \
    break; \
    }

synthetic::~synthetic()
{
    // free the search space description
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
    // free the optimal parameters
    inst::free_node (opt_params);
}

void
synthetic::validate_param_set(inst::set x)
{
    inst::value_map *vals = x->get_values();
    sspace::validate_param_values (vals, &m_sspace);
}

void
synthetic::set_opt_param(inst::set op)
{
    opt_params = op;
}

ackley1::ackley1 (int d) :
    synthetic ("ackley1", d, -35, 35, 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node("ackley1 opt");
    for (int i = 0; i < d; i++)
        opt->add_item (new inst::dbl_val(std::to_string (i), 0.));
    this->set_opt_param (opt);
}

double
ackley1::evaluate (inst::set x)
{
    validate_param_set (x);

    double e1 = 0., e2 = 0.;
    double rD = 1. / (double) m_dims;
    for (u_int i = 0u; i < m_dims; i++) {
        e1 += x->getdbl(i) * x->getdbl(i);
        e2 += std::cos(2. * M_PI * x->getdbl(i));
    }
    return -20. * std::exp(-0.2 * std::sqrt(rD * e1))
        - std::exp(rD * e2) + 20. + M_E;
};

ackley2::ackley2 ():
    synthetic ("ackley2", 2, -32, 32, -200)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node("ackley2 opt");
    opt->add_item (new inst::dbl_val("0", 0.));
    opt->add_item (new inst::dbl_val("1", 0.));
    this->set_opt_param (opt);
}

double
ackley2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x12 = x->getdbl(0) * x->getdbl(0);
    double x22 = x->getdbl(1) * x->getdbl(1);
    return -200 * std::exp(-0.02 * std::sqrt (x12 + x22));
}

ackley3::ackley3 ():
    synthetic ("ackley3", 2, -32, 32, 5.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node("ackley3 opt");
    opt->add_item (new inst::dbl_val("0", 0.));
    opt->add_item (new inst::dbl_val("1", 0.));
    this->set_opt_param (opt);
}

double
ackley3::evaluate (inst::set x)
{
    double x12 = std::pow(x->getdbl("0"), 2.);
    double x22 = std::pow(x->getdbl("1"), 2.);
    double e1 = -0.2 * std::sqrt(x12 + x22);
    double e2 = std::cos(3 * x->getdbl("0")) + std::sin(3 * x->getdbl("1"));
    return -200 * e1 + 5 * e2;
}

adjiman::adjiman ():
    synthetic ("adjiman", 2, -2.02181)
{
    sspace::sspace_t *ss = this->get_search_space();
    sspace::param_t *x1 = new sspace::uniform ("0", -1, 2);
    sspace::param_t *x2 = new sspace::uniform ("0", -1, 1);
    ss->push_back(x1);
    ss->push_back(x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node("ackley3 opt");
    opt->add_item (new inst::dbl_val("0", 2.));
    opt->add_item (new inst::dbl_val("1", 0.10578));
    this->set_opt_param (opt);
}

double
adjiman::evaluate (inst::set x)
{
    double x1 = x->getdbl(0);
    double x2 = x->getdbl(1);
    return std::cos(x1) * std::sin(x2) - (x1 / (x2*x2 + 1));
}

} // end namespace syn
