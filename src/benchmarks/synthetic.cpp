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
#include <cmath>

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
    validate_param_set (x);

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
    sspace::param_t *x2 = new sspace::uniform ("1", -1, 1);
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
    validate_param_set (x);

    double x1 = x->getdbl(0);
    double x2 = x->getdbl(1);
    return std::cos(x1) * std::sin(x2) - (x1 / (x2*x2 + 1));
}

alpine1::alpine1 (int dims):
    synthetic ("alpine1", dims, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::non_differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("alpine1 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val(std::to_string (i), 0.));
    this->set_opt_param (opt);
}

double
alpine1::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 0.;
    for (u_int i = 0; i < m_dims; i++) {
        double tmp = x->getdbl(i);
        res += std::fabs(tmp * std::sin(tmp) + 0.1 * tmp);
    }
    return res;
}

alpine2::alpine2 (int dims):
    synthetic ("alpine2", dims, 0., 10., 0)
{
    this->update_opt (std::pow(2.808, (double) dims));

    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("alpine2 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val(std::to_string (i), 7.917));
    this->set_opt_param (opt);
}

double
alpine2::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 1.;
    for (u_int i = 0; i < m_dims; i++) {
        double tmp = x->getdbl(i);
        res *= std::sqrt(tmp) * std::sin(tmp);
    }
    return res;
}

brad::brad ():
    synthetic ("brad", 3, 0.00821488)
{
    sspace::sspace_t *ss = this->get_search_space();
    sspace::param_t *x1 = new sspace::uniform ("0", -0.25, 0.25);
    sspace::param_t *x2 = new sspace::uniform ("1", 0.01, 2.5);
    sspace::param_t *x3 = new sspace::uniform ("2", 0.01, 2.5);
    ss->push_back(x1);
    ss->push_back(x2);
    ss->push_back(x3);

    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("brad opt");
    opt->add_item (new inst::dbl_val ("0", 0.08241040));
    opt->add_item (new inst::dbl_val ("1", 1.133033));
    opt->add_item (new inst::dbl_val ("2", 2.343697));
    this->set_opt_param (opt);
}

double
brad::evaluate (inst::set x)
{
    validate_param_set (x);

    double y[15] = {
        0.14, 0.18, 0.22, 0.25, 0.29, 0.32, 0.35, 0.39,
        0.37, 0.58, 0.73, 0.96, 1.34, 2.10, 4.39
    };
    double res = 0.;
    double x1 = x->getdbl(0);
    double x2 = x->getdbl(1);
    double x3 = x->getdbl(2);
    for (int i = 1; i < 16; i++) {
        res += std::pow(
                x1 + ((double)i/(x2*(double)(16-i) + x3*(double)std::min(i, 16-i))) - y[i-1]
                , 2.
                );
    }
    return res;
}

bartels_conn::bartels_conn ():
    synthetic ("bartels conn", 2, -500., 500., 1.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("brad opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bartels_conn::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl("0"), x2 = x->getdbl("1");
    double x12 = std::pow(x1, 2.), x22 = std::pow(x2, 2.);
    return
        std::abs(x12 + x22 + x1*x2) +
        std::abs(std::sin(x1)) +
        std::abs(std::cos(x2));
}

beale::beale ():
    synthetic ("beale", 2, -4.5, 4.5, 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
    inst::node *opt = new inst::node ("beale opt");
    opt->add_item (new inst::dbl_val ("0", 3.));
    opt->add_item (new inst::dbl_val ("1", 0.5));
    this->set_opt_param (opt);
}

double
beale::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x22 = std::pow(x2, 2.), x23 = std::pow(x2, 3.);
    return std::pow((1.5 - x1 + x1 * x2), 2.) +
        std::pow((2.25 - x1 + x1 * x22), 2.) +
        std::pow (2.625 - x1 + x1 * x23, 2.);
}

biggs_exp2::biggs_exp2 ():
    synthetic ("biggs exp2", 2, 0., 20., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("biggs exp2 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    this->set_opt_param (opt);
}

double
biggs_exp2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double res = 0.;
    for (int i = 1; i < 11; i++) {
        res += std::pow(std::exp (-0.1 * i * x1)
            - 5 * std::exp (-0.1 * i * x2)
            - std::exp (-0.1 * i)
            + 5 * std::exp (-i),
            2.);
    }
    return res;
}

biggs_exp3::biggs_exp3 ():
    synthetic ("biggs exp3", 3, 0., 20., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("biggs exp3 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    opt->add_item (new inst::dbl_val ("2", 5.));
    this->set_opt_param (opt);
}

double
biggs_exp3::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double res = 0.;
    for (int i = 1; i < 11; i++) {
        res += std::pow(std::exp (-0.1 * i * x1)
            - x3 * std::exp (-0.1 * i * x2)
            - std::exp (-0.1 * i)
            + 5 * std::exp (-i),
            2.);
    }
    return res;
}

biggs_exp4::biggs_exp4 ():
    synthetic ("biggs exp4", 4, 0., 20., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("biggs exp4 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    opt->add_item (new inst::dbl_val ("2", 1.));
    opt->add_item (new inst::dbl_val ("3", 5.));
    this->set_opt_param (opt);
}

double
biggs_exp4::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double x4 = x->getdbl(3);
    double res = 0.;
    for (int i = 1; i < 11; i++) {
        res += x3 * std::pow(std::exp (-0.1 * i * x1)
            - x4 * std::exp (-0.1 * i * x2)
            - std::exp (-0.1 * i)
            + 5 * std::exp (-i),
            2.);
    }
    return res;
}

biggs_exp5::biggs_exp5 ():
    synthetic ("biggs exp5", 5, 0., 20., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("biggs exp5 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    opt->add_item (new inst::dbl_val ("2", 1.));
    opt->add_item (new inst::dbl_val ("3", 5.));
    opt->add_item (new inst::dbl_val ("4", 4.));
    this->set_opt_param (opt);
}

double
biggs_exp5::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double x4 = x->getdbl(3), x5 = x->getdbl(4);
    double res = 0.;
    for (int i = 1; i < 11; i++) {
        res += x3 * std::pow(std::exp (-0.1 * i * x1)
            - x4 * std::exp (-0.1 * i * x2)
            + 3 * std::exp (-0.1 * i * x5)
            - std::exp (-0.1 * i)
            + 5 * std::exp (-i)
            - 3 * std::exp (-0.4 * i),
            2.);
    }
    return res;
}

biggs_exp6::biggs_exp6 ():
    synthetic ("biggs exp6", 6, -20., 20., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("biggs exp5 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    opt->add_item (new inst::dbl_val ("2", 1.));
    opt->add_item (new inst::dbl_val ("3", 5.));
    opt->add_item (new inst::dbl_val ("4", 4.));
    opt->add_item (new inst::dbl_val ("5", 3.));
    this->set_opt_param (opt);
}

double
biggs_exp6::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double x4 = x->getdbl(3), x5 = x->getdbl(4), x6 = x->getdbl(5);
    double res = 0.;
    for (int i = 1; i < 11; i++) {
        res += x3 * std::pow(std::exp (-0.1 * i * x1)
            - x4 * std::exp (-0.1 * i * x2)
            + x6 * std::exp (-0.1 * i * x5)
            - std::exp (-0.1 * i)
            + 5 * std::exp (-i)
            - 3 * std::exp (-0.4 * i),
            2.);
    }
    return res;
}

} // end namespace syn
