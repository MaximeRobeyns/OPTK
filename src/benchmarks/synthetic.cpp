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
#include <sys/types.h>

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

bird::bird ():
    synthetic ("bird", 2, -2 * M_PI, 2 * M_PI, -106.764537)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    // multimodal; this is not unique
    inst::node *opt = new inst::node ("bird opt");
    opt->add_item (new inst::dbl_val ("0", 4.70104));
    opt->add_item (new inst::dbl_val ("1", 3.15294));
    this->set_opt_param (opt);
}

double
bird::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double e1 = std::exp (std::pow (1 - std::cos (x2), 2.));
    double e2 = std::exp (std::pow (1 - std::sin (x1), 2.));
    return std::sin (x1) * e1 + std::cos (x2) * e2 + std::pow (x1 - x2, 2.);
}

bohachevsky1::bohachevsky1 ():
    synthetic ("bohachevsky1", 2, -100., 100., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bohachevsky1 opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bohachevsky1::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1, 2.) + 2 * std::pow (x2, 2.)
        - 0.3 * std::cos (3 * M_PI * x1)
        - 0.4 * std::cos (4 * M_PI * x2)
        + 0.7;
}

bohachevsky2::bohachevsky2 ():
    synthetic ("bohachevsky2", 2, -100., 100., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bohachevsky2 opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bohachevsky2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1, 2.) + 2 * std::pow (x2, 2.)
        - 0.3 * std::cos (3 * M_PI * x1) * std::cos (4 * M_PI * x2)
        + 0.3;
}

bohachevsky3::bohachevsky3 ():
    synthetic ("bohachevsky3", 2, -100., 100., 0.)
{
    this->set_properties (std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bohachevsky3 opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bohachevsky3::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1, 2.) + 2 * std::pow (x2, 2.)
        - 0.3 * std::cos (3 * M_PI * x1 + 4 * M_PI * x2)
        + 0.3;
}

booth::booth ():
    synthetic ("booth", 2, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
    inst::node *opt = new inst::node ("booth opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 3.));
    this->set_opt_param (opt);
}

double
booth::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1 + 2 * x2 - 7, 2.) +
        std::pow (2 * x1 + x2 - 5, 2.);
}

box_betts::box_betts ():
    synthetic ("box betts", 3, 0.)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x1 = new sspace::uniform ("0", 0.9, 1.2);
    sspace::param_t *x2 = new sspace::uniform ("1", 9, 11.2);
    sspace::param_t *x3 = new sspace::uniform ("2", 0.9, 1.2);
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

    inst::node *opt = new inst::node ("box betts opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    opt->add_item (new inst::dbl_val ("2", 1.));
    this->set_opt_param (opt);
}

static double
box_betts_g(int i, double x1, double x2, double x3)
{
    double e1 = std::exp (-0.1 * (i) * x1);
    double e2 = -std::exp (-0.1 * (i) * x2);
    double e3 = - (std::exp (-0.1 * i) - std::exp (-i)) * x3;
    return std::pow (e1 + e2 + e3, 2.);
}

double
box_betts::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double ret = 0.;
    for (int i = 2; i < 13; i++) {
        ret += box_betts_g (i, x1, x2, x3);
    }
    return ret;
}

branin1::branin1 ():
    synthetic("branin1", 2, 0.39788735772973816)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x1 = new sspace::uniform ("0", -5., 10.);
    sspace::param_t *x2 = new sspace::uniform ("1", 0., 15.);
    ss->push_back (x1);
    ss->push_back (x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("branin1 opt");
    opt->add_item (new inst::dbl_val ("0", -M_PI));
    opt->add_item (new inst::dbl_val ("1", 12.275));
    this->set_opt_param (opt);
}

double
branin1::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x2 -
                (5.1 / (4. * std::pow (M_PI, 2.))) * std::pow (x1, 2.) +
                5. * x1 / M_PI - 6
                , 2.) +
                10. * (1. - 1./(8*M_PI)) * std::cos (x1)
                + 10;
}

branin2::branin2 ():
    synthetic("branin2", 2, -5., 15., 5.559037)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("branin2 opt");
    opt->add_item (new inst::dbl_val ("0", -3.2));
    opt->add_item (new inst::dbl_val ("1", 12.53));
    this->set_opt_param (opt);
}

double
branin2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x12 = std::pow (x1, 2.), x22 = std::pow (x2, 2.);
    return std::pow (x2 -
                     (5.1 / (4 * std::pow (M_PI, 2.))) *
                     x12 + 5 * x1 / M_PI -
                     6, 2.) +
            10 * (1. - 1. / (8 * M_PI)) *
            std::cos (x1) * std::cos (x2) +
            std::log (x12 + x22 + 1) +
            10;
}

brent::brent ():
    synthetic ("brent", 2, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("brent opt");
    opt->add_item (new inst::dbl_val ("0", -10.));
    opt->add_item (new inst::dbl_val ("1", -10.));
    this->set_opt_param (opt);
}

double
brent::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double e1 = std::exp (-std::pow (x1, 2.) - std::pow (x2, 2.));
    return std::pow(x1 + 10, 2.) + std::pow (x2 + 10, 2.) + e1;
}

brown::brown (int dims):
    synthetic ("brown", dims, -1., 4., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("brent opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.));
    this->set_opt_param (opt);
}

double
brown::evaluate (inst::set x)
{
    validate_param_set (x);

    double ret = 0.;
    for (u_int i = 0; i < m_dims-1; i++) {
        double xi2 = std::pow (x->getdbl(i), 2.);
        double xii2 = std::pow (x->getdbl(i+1), 2.);
        ret += std::pow (xi2, xii2 + 1) + std::pow (xii2, xi2 + 1);
    }
    return ret;
}

bukin2::bukin2 ():
    synthetic ("bukin2", 2, 0.)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x1 = new sspace::uniform ("0", -15., -5.);
    sspace::param_t *x2 = new sspace::uniform ("1", -3., 3.);
    ss->push_back (x1);
    ss->push_back (x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bukin2 opt");
    opt->add_item (new inst::dbl_val ("0", -10.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bukin2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return 100 * (x2 - 0.01 * std::pow (x1, 2.) + 1.) +
        0.01 * std::pow(x1 + 10, 2.);
}

bukin4::bukin4 ():
    synthetic ("bukin4", 2, 0.)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x1 = new sspace::uniform ("0", -15., -5.);
    sspace::param_t *x2 = new sspace::uniform ("1", -3., 3.);
    ss->push_back (x1);
    ss->push_back (x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::non_differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bukin4 opt");
    opt->add_item (new inst::dbl_val ("0", -10.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
bukin4::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x22 = std::pow (x->getdbl(1), 2.);
    return 100 * x22 + 0.01 * std::fabs (x1 + 10);
}

bukin6::bukin6 ():
    synthetic ("bukin6", 2, 0.)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x1 = new sspace::uniform ("0", -15., -5.);
    sspace::param_t *x2 = new sspace::uniform ("1", -3., 3.);
    ss->push_back (x1);
    ss->push_back (x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("bukin6 opt");
    opt->add_item (new inst::dbl_val ("0", -10.));
    opt->add_item (new inst::dbl_val ("1", 1.));
    this->set_opt_param (opt);
}

double
bukin6::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return 100 * std::sqrt (std::fabs (x2 - 0.01 * std::pow (x1, 2.))) +
            0.01 * std::fabs (x1 + 10);
}

camel3::camel3 ():
    synthetic ("camel3", 2, -5., 5., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("camel3 opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
camel3::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return 2 * std::pow (x1, 2.) -
           1.05 * std::pow (x1, 4.) +
           std::pow (x1, 6.)/6. +
           x1 * x2 +
           std::pow (x2, 2.);
}

camel6::camel6 ():
    synthetic ("camel6", 2, -5., 5., -1.031628)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("camel6 opt");
    opt->add_item (new inst::dbl_val ("0", 0.08984201368301331));
    opt->add_item (new inst::dbl_val ("1", -0.7126564032704135));
    this->set_opt_param (opt);
}

double
camel6::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return (4 - 2.1 * std::pow (x1, 2.) +
        std::pow (x1, 4.) / 3.) * std::pow (x1, 2.) +
        x1 * x2 +
        (4 * std::pow (x2, 2.) - 4) * std::pow (x2, 2.);
}

chichinadze::chichinadze ():
    synthetic ("chichinadze", 2, -30., 30., -42.94438701899098)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("chichinadze opt");
    opt->add_item (new inst::dbl_val ("0", 6.189866586965680));
    opt->add_item (new inst::dbl_val ("1", 0.5));
    this->set_opt_param (opt);
}

double
chichinadze::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1, 2.) -
        12 * x1 +
        11 +
        10 * std::cos ((M_PI * x1)/2.) +
        8 * std::sin ((5 * M_PI * x1)/2.) -
        std::pow((1./5.), 0.5) * std::exp (-0.5 * std::pow(x2 - 0.5, 2.));
}

chung_reynolds::chung_reynolds (int dims):
    synthetic ("chung reynolds", dims, -100., 100., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::partially_separable,
                properties::scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("chung reynolds opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.));
    this->set_opt_param (opt);
}

double
chung_reynolds::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 0.;
    for (u_int i = 0; i < m_dims; i++) {
        res += std::pow (x->getdbl(i), 2.);
    }
    return std::pow (res, 2.);
}

cola::cola ():
    synthetic ("cola", 17, 11.7464)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x0 = new sspace::uniform ("0", 0., 4.);
    ss->push_back (x0);
    for (int i = 1; i < 17; i++) {
        sspace::param_t *tmp = new sspace::uniform (std::to_string(i), -4., 4.);
        ss->push_back (tmp);
    }

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("cola opt");
    double values[17] = {
        0.651906, 1.30194, 0.099242, -0.883791, -0.8796, 0.204651, -3.28414,
        0.851188, -3.46245, 2.53245, -0.895246, 1.40992, -3.07367, 1.96257,
        -2.97872, -0.807849, -1.68978 };
    for (int i = 0; i < 17; i++)
        opt->add_item (new inst::dbl_val (std::to_string (i), values[i]));
    this->set_opt_param (opt);
}

double
cola::evaluate (inst::set x)
{
    validate_param_set (x);

    double d[10][9] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1.27, 0, 0, 0, 0, 0, 0, 0, 0},
        {1.69, 1.43, 0, 0, 0, 0, 0, 0, 0},
        {2.04, 2.35, 2.43, 0, 0, 0, 0, 0, 0},
        {3.09, 3.18, 3.26, 2.85, 0, 0, 0, 0, 0},
        {3.20, 3.22, 3.27, 2.88, 1.55, 0, 0, 0, 0},
        {2.86, 2.56, 2.58, 2.59, 3.12, 3.06, 0, 0, 0},
        {3.17, 3.18, 3.18, 3.12, 1.31, 1.64, 3., 0, 0},
        {3.21, 3.18, 3.18, 3.17, 1.7, 1.36, 2.95, 1.32, 0},
        {2.38, 2.31, 2.42, 1.94, 2.85, 2.81, 2.56, 2.91, 2.97}
    };

    int idx = 2;
    double x1[10] = {0, x->getdbl(0)};
    for (int i = 1; i < 17; i+=2)
        x1[idx++] = x->getdbl(i);

    idx = 2;
    double x2[10] = {0, 0};
    for (int i = 2; i < 17; i+=2)
        x2[idx++] = x->getdbl(i);

    double res = 0.;
    for (int i = 1; i < 10; i++) {
        for (int j = 0; j < i; j++) {
            res += std::pow (
                std::sqrt(
                        std::pow (x1[i] - x1[j], 2.) +
                        std::pow (x2[i] - x2[j], 2.)
                        ) -
                    d[i][j],
                    2.);
        }
    }
    return res;
}

colville::colville ():
    synthetic ("colville", 4, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("colville opt");
    for (int i = 0; i < 4; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 1.));
    this->set_opt_param (opt);
}

double
colville::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x3 = x->getdbl(2), x4 = x->getdbl(3);

    return 100 * std::pow (x1 - std::pow (x2, 2.), 2.) +
           std::pow (1 - x1, 2.) +
           90 * std::pow (x4 - std::pow (x3, 2.), 2.) +
           std::pow (1 - x3, 2.) +
           10.1 * (std::pow (x2 - 1, 2.) + std::pow (x4 - 1, 2.)) +
           19.8 * (x2 - 1) * (x4 - 1);
}

corana::corana ():
    synthetic ("corana", 4, -100., 100., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("corana opt");
    for (int i = 0; i < 4; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.));
    this->set_opt_param (opt);
}

double
corana::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[4] = {x->getdbl(0), x->getdbl(1), x->getdbl(2), x->getdbl(3)};
    double ds[4] = {1., 1000., 10., 100.};

    double res = 0.;
    for (int i = 0; i < 4; i++) {
        double sgnx = xs[i] < 0. ? -1. : 1.;
        double z = 0.2 * std::floor (std::fabs (xs[i] / 0.2) + 0.49999) * sgnx;
        double sgnz = z < 0. ? -1. : 1.;
        if (std::fabs (xs[i] - z) < 0.05) {
            res += 0.15 * ds[i] * std::pow(z - 0.05 * sgnz, 2.);
        } else {
            res += ds[i] * std::pow (xs[i], 2.);
        }
    }
    return res;
}

cosine_mixture::cosine_mixture (int dims):
    synthetic ("cosine mixture", dims, -1., 1., 0.)
{
    this->update_opt ((double)dims / 10.);

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("cosine mixture opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.));
    this->set_opt_param (opt);
}

double
cosine_mixture::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[m_dims];
    for (u_int i = 0; i < m_dims; i++)
        xs[i] = x->getdbl(i);

    double s1 = 0., s2 = 0.;
    for (u_int i = 0; i < m_dims; i++) {
        s1 += std::cos (5. * M_PI * xs[i]);
        s2 += std::pow (xs[i], 2.);
    }
    return 0.1 * s1 - s2;
}

cross_in_tray::cross_in_tray ():
    synthetic ("cross in tray", 2, -10., 10., -2.062611870822739)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("cross in tray opt");
    opt->add_item (new inst::dbl_val ("0", 1.349406685353340));
    opt->add_item (new inst::dbl_val ("1", 1.349406685353340));
    this->set_opt_param (opt);
}

double
cross_in_tray::evaluate (inst::set x)
{
    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double e1 = std::exp (std::fabs (
                100 -
                std::sqrt (std::pow (x1, 2.) + std::pow (x2, 2.)) / M_PI
                ));
    return -0.0001 * std::pow ((
            std::fabs (
                std::sin (x1) * std::sin(x2) * e1
                )
            + 1
            ), 0.1);
}

csendes::csendes (int dims):
    synthetic ("csendes", dims, -1., 1., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("csendes opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.));
    this->set_opt_param (opt);
}

double
csendes::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 0.;
    for (u_int i = 0; i < m_dims; i++) {
        double x_tmp = x->getdbl(i);
        double x_tmp6 = std::pow (x_tmp, 6.);
        res += x_tmp6 * (2 +
                std::sin (1. /
                        (x_tmp + std::numeric_limits<float>::epsilon())
                    )
                );
    }
    return res;
}

cube::cube ():
    synthetic ("cube", 2, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("cube opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 1.));
    this->set_opt_param (opt);
}

double
cube::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return 100. * std::pow (x2 - std::pow (x1, 3.), 2.) + std::pow (1 - x1, 2.);
}

damavandi::damavandi ():
    synthetic ("damavandi", 2, 0., 14., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("damavandi opt");
    opt->add_item (new inst::dbl_val ("0", 2.));
    opt->add_item (new inst::dbl_val ("1", 2.));
    this->set_opt_param (opt);
}

double
damavandi::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double t1 = M_PI * (x1 - 2), t2 = M_PI * (x2 - 2);
    double q1;
    if (std::fabs (x1-2) > 1e-3 && std::fabs(x2-2) > 1e-3) {
        double n1 = std::sin (t1) * std::sin (t2);
        double d1 = t1 * t2;
        q1 = n1 / d1;
    } else {
        double x_t = std::fabs (x1 - 2) <= 1e-3 ?
            1. - std::pow (t1, 2.) / 6. :
            std::sin (t1) / t1;
        double y_t = std::fabs (x2 - 2) <= 1e-3 ?
            1. - std::pow (t2, 2.) / 6. :
            std::sin (t2) / t2;
        q1 = x_t * y_t;
    }
    double f1 = 1. - std::pow (std::fabs (q1), 5.);
    double f2 = 2. + std::pow (x1 - 7, 2.) + 2 * std::pow (x2 - 7, 2.);
    return f1 * f2;
}

deb1::deb1 (int dims):
    synthetic ("deb 1", dims, -1., 1., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deb 1 opt");
    for (u_int i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.3));
    this->set_opt_param (opt);
}

double
deb1::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 0.;
    double q = -1. / (double) m_dims;
    for (u_int i = 0; i < m_dims; i++)
        res += std::pow (std::sin (5. * M_PI * x->getdbl(i)), 6.);
    return q * res;
}

deb2::deb2 (int dims):
    synthetic ("deb 2", dims, -1., 1., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deb 1 opt");
    for (u_int i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.0796993926887));
    this->set_opt_param (opt);
}

double
deb2::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = 0.;
    double q = -1. / (double) m_dims;
    for (u_int i = 0; i < m_dims; i++)
        res += std::pow (std::sin (
                    5 * M_PI * (std::pow (x->getdbl(i), 0.75) - 0.05)
                    ),
                6.);
    return q * res;
}

deckkers_aarts::deckkers_aarts ():
    synthetic ("deckkers aarts", 2, -20., 20., -24777.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deckkers aarts opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 15.));
    this->set_opt_param (opt);
}

double
deckkers_aarts::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x12 = std::pow (x1, 2.), x22 = std::pow (x2, 2.);

    return std::pow (10., 5.) * x12 +
        x22 -
        std::pow (x12 + x22, 2.) +
        std::pow (10, -5) * std::pow (x12 + x22, 4.);
}

devillers_glasser1::devillers_glasser1 ():
    synthetic ("deVillers Glasser 1", 4, -500., 500., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deVillers glasser opt");
    opt->add_item (new inst::dbl_val ("0", 60.173));
    opt->add_item (new inst::dbl_val ("1", 1.371));
    opt->add_item (new inst::dbl_val ("2", 3.112));
    opt->add_item (new inst::dbl_val ("3", 1.761));
    this->set_opt_param (opt);
}

double
devillers_glasser1::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x3 = x->getdbl(2), x4 = x->getdbl(3);

    double res = 0.;
    for (int i = 1; i <= 24; i++) {
        double ti = 0.1 * ((double)i - 1.);
        double yi = 60.137 *
            std::pow (1.371, ti) *
            std::sin (3.112 * ti + 1.761);
        res += std::pow (
                x1 *
                std::pow (x2, ti) *
                std::sin (x3 * ti + x4) -
                yi,
                2.);
    }
    return res;
}

devillers_glasser2::devillers_glasser2 ():
    synthetic ("deVillers Glasser 2", 5, -500., 500., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deVillers glasser opt");
    opt->add_item (new inst::dbl_val ("0", 53.81));
    opt->add_item (new inst::dbl_val ("1", 1.27));
    opt->add_item (new inst::dbl_val ("2", 3.012));
    opt->add_item (new inst::dbl_val ("3", 2.13));
    opt->add_item (new inst::dbl_val ("4", 0.507));
    this->set_opt_param (opt);
}

double
devillers_glasser2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double x4 = x->getdbl(3), x5 = x->getdbl(4);
    double res = 0.;
    for (int i = 1; i <= 16; i++) {
        double ti = 0.1 *((double)i - 1.);
        double yi = 53.81 *
            std::pow (1.27, ti) *
            std::tanh (3.012 * ti + std::sin(2.31 * ti)) *
            std::cos (std::exp (0.507) * ti);
        res += std::pow (
                x1 *
                std::pow (x2, ti) *
                std::tanh (x3 * ti + std::sin (x4 * ti)) *
                std::cos (std::exp (x5) * ti) -
                yi,
                2.);
    }
    return res;
}

dixon_price::dixon_price (int dims):
    synthetic ("dixon price", dims, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("dixon & price opt");
    for (uint i = 1; i <= m_dims; i++) {
        double v = std::pow (2., -(std::pow (2., i) - 2.)/(std::pow (2., i)));
        opt->add_item (new inst::dbl_val (std::to_string (i-1), v));
    }
    this->set_opt_param (opt);
}

double
dixon_price::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = std::pow (x->getdbl(0) - 1, 2.);
    for (uint i = 2; i <= m_dims; i++) {
        res += (double) i *
            std::pow (
                    2. * std::pow (x->getdbl(i-1), 2.) -
                    x->getdbl(i-2),
                    2.);
    }
    return res;
}

dolan::dolan ():
    synthetic ("dolan", 5, -100., 100., -529.8714387324576)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("dolan opt");
    opt->add_item (new inst::dbl_val ("0", 98.964258312237106));
    opt->add_item (new inst::dbl_val ("1", 100.));
    opt->add_item (new inst::dbl_val ("2", 100.));
    opt->add_item (new inst::dbl_val ("3", 99.224323672554704));
    opt->add_item (new inst::dbl_val ("4", -0.249987527588471));
    this->set_opt_param (opt);
}

double
dolan::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);
    double x4 = x->getdbl(3), x5 = x->getdbl(4);

    return (x1 + 1.7 * x2) * std::sin (x1) -
        1.5 * x3 -
        0.1 * x4 * std::cos (x5 + x4 - x1) +
        0.2 * std::pow (x5, 2.) - x2 - 1;
}

deceptive::deceptive (int dims):
    synthetic ("deceptive", dims, 0., 1., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deceptive opt");
    double delta = 0.8 / (double) m_dims;
    for (uint i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0.1 + delta * (double)i));
    this->set_opt_param (opt);
}

double
deceptive::evaluate (inst::set x)
{
    validate_param_set (x);

    inst::set opt = this->get_opt_param ();

    double g = 0.;
    for (uint i = 0; i < m_dims; i++) {
        double ai = opt->getdbl(i);
        double xi = x->getdbl(i);

        if (xi <= 0) {
            g += xi;
        } else if (xi < 0.8 * ai) {
            g += -xi / ai + 0.8;
        } else if (xi < ai) {
            g += 5 * xi / ai - 4;
        } else if (xi < (1 + 4 * ai) / 5.) {
            g += 5 * (xi - ai) / (ai - 1.) + 1.;
        } else if (xi <= 1.) {
            g += (xi - 1.) / (1. - ai) + 0.8;
        } else {
            g += xi - 1.;
        }
    }
    return -std::pow((1./(double)m_dims) * g, 2.);
}

deflected_corrugated_spring::deflected_corrugated_spring (int dims) :
    synthetic ("deflected corrugated spring", dims, 0., 10., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("deflected corrugated spring opt");
    for (uint i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 5.));
    this->set_opt_param (opt);
}

double
deflected_corrugated_spring::evaluate (inst::set x)
{
    validate_param_set (x);

    double sum = 0.;
    for (uint i = 0; i < m_dims; i++)
        sum += std::pow (x->getdbl(i) - 5, 2.);
    return 0.1 * sum - std::cos (5. * std::sqrt (sum));
}

drop_wave::drop_wave ():
    synthetic ("drop wave", 2, -5.12, 5.12, -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("drop wave opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
drop_wave::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double d1 = std::pow (x1, 2.) + std::pow (x2, 2.);
    double n = 1 + std::cos (12. * std::sqrt (d1));
    double d = 0.5 * d1 + 2.;
    return - n/d;
}

easom::easom ():
    synthetic ("easom", 2, -10., 10., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("easom opt");
    opt->add_item (new inst::dbl_val ("0", M_PI));
    opt->add_item (new inst::dbl_val ("1", M_PI));
    this->set_opt_param (opt);
}

double
easom::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return -1. *
        std::cos (x1) *
        std::cos (x2) *
        std::exp (
                -std::pow (x1 - M_PI, 2.) -
                -std::pow (x2 - M_PI, 2.)
                );
}

egg_crate::egg_crate ():
    synthetic ("egg crate", 2, -5., 5., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::separable,
                properties::non_scalable,
                }));

    inst::node *opt = new inst::node ("egg create opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
egg_crate::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (x1, 2.) +
           std::pow (x2, 2.) +
           25 * (
                   std::pow (std::sin (x1), 2.) +
                   std::pow (std::sin (x2), 2.)
                );
}

egg_holder::egg_holder ():
    synthetic ("egg holder", 2, -512., 512., -959.640662711)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::separable,
                properties::non_scalable,
                }));

    inst::node *opt = new inst::node ("egg create opt");
    opt->add_item (new inst::dbl_val ("0", 512.));
    opt->add_item (new inst::dbl_val ("1", 404.2319));
    this->set_opt_param (opt);
}

double
egg_holder::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return -(x2 + 47) *
           std::sin (std::sqrt (std::fabs (x2 + x1/2. + 47.))) -
           x1 * std::sin (std::sqrt (std::fabs (x1 - (x2 + 47))));
}

el_attar_vidyasagar_dutta::el_attar_vidyasagar_dutta ():
    synthetic ("el_attar_vidyasagar_dutta", 2, -100., 100., 1.712780354)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("el_attar_vidyasagar_dutta opt");
    opt->add_item (new inst::dbl_val ("0", 3.40918683));
    opt->add_item (new inst::dbl_val ("1", -2.17143304));
    this->set_opt_param (opt);
}

double
el_attar_vidyasagar_dutta::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    return std::pow (std::pow (x1, 2.) + x2 - 10, 2.) +
           std::pow (x1 + std::pow (x2, 2.) - 7 , 2.) +
           std::pow (std::pow (x1, 2.) + std::pow (x2, 3.) - 1 , 2.);
}

exponential::exponential (int dims):
    synthetic ("exponential", dims, -1., 1., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("exponential opt");
    for (uint i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0));
    this->set_opt_param (opt);
}

double
exponential::evaluate (inst::set x)
{
    validate_param_set (x);

    double sum = 0.;
    for (uint i = 0; i < m_dims; i++) {
        sum += std::pow (x->getdbl(i), 2.);
    }
    return -std::exp (-0.5 * sum);
}

exp2::exp2 ():
    synthetic ("exp2", 2, 0., 20., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("exp2 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 10.));
    this->set_opt_param (opt);
}

double
exp2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double res = 0.;
    for (int i = 0; i < 10; i++) {
        res += std::pow (
                std::exp ((-i * x1)/10.) -
                5. * std::exp ((-i * x2)/10.) -
                std::exp (-i / 10.) +
                5 * std::exp (-i)
                , 2.);
    }
    return res;
}

franke::franke ():
    synthetic ("franke", 2, 0., 1., 0.00111528244)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("franke opt");
    opt->add_item (new inst::dbl_val ("0", 0.45571037432));
    opt->add_item (new inst::dbl_val ("1", 0.78419067287));
    this->set_opt_param (opt);
}

double
franke::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return
        0.75 * std::exp (-std::pow (9*x1-2, 2.)/4. - std::pow (9*x2-2, 2.)/4.) +
        0.75 * std::exp (-std::pow (9*x1+1, 2.)/49. - (9*x2+1) / 10.) +
        0.5 * std::exp (-std::pow (9*x1-7, 2.)/4. - std::pow (9*x2-3, 2.)/4.) -
        0.2 * std::exp (-std::pow(9*x1-4, 2.) - std::pow (8*x2-7, 2.));
}

} // end namespace syn
