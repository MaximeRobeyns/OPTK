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
 * Jamil et al. 2013 <https://arxiv.org/abs/1308.4008>, as well as various
 * others found from disparate publications and online respirces.
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

synthetic::~synthetic()
{
    sspace::free_ss (&m_sspace);

    // free the optimal parameters
    if (opt_params != NULL)
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

freudenstein_roth::freudenstein_roth ():
    synthetic ("freudenstein_roth", 2, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("freudenstein_roth opt");
    opt->add_item (new inst::dbl_val ("0", 5.));
    opt->add_item (new inst::dbl_val ("1", 4.));
    this->set_opt_param (opt);
}

double
freudenstein_roth::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return
        std::pow ((x1 - 13 + ((5 - x2) * x2 - 2)*x2) ,2.) +
        std::pow ((x1 - 29 + ((x2 + 1) * x2 - 14)*x2), 2.);
}

gear::gear ():
    synthetic ("gear", 4, 12., 60., 2.700857148886513e-12)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("gear opt");
    opt->add_item (new inst::dbl_val ("0", 16.));
    opt->add_item (new inst::dbl_val ("1", 19.));
    opt->add_item (new inst::dbl_val ("2", 43.));
    opt->add_item (new inst::dbl_val ("3", 49.));
    this->set_opt_param (opt);
}

double
gear::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x3 = x->getdbl(2), x4 = x->getdbl(3);

    return std::min(
            std::pow ( 1./6.931 -
                (std::floor (x1) * std::floor (x2))/
                (std::floor (x3) * std::floor (x4))
                , 2.),
            std::numeric_limits<double>::max()
            );
}

giunta::giunta ():
    synthetic ("giunta", 2, -1., 1., 0.06447042053690566)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("giunta opt");
    opt->add_item (new inst::dbl_val ("0", 0.4673200277395354));
    opt->add_item (new inst::dbl_val ("1", 0.4673200169591304));
    this->set_opt_param (opt);
}

double
giunta::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[2] = {x->getdbl(0), x->getdbl(1)};

    double res = 0.;
    for (int i = 0; i < 2; i++)
        res += std::sin (16./15. * xs[i] - 1) +
            std::pow (std::sin (16./15. * xs[i] - 1) , 2.) +
            1./50. * std::sin (4. * (16./15. * xs[i] - 1));
    return 0.6 + res;
}

goldstein_price::goldstein_price ():
    synthetic ("goldstein_price", 2, -2., 2., 3.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("goldstein_price opt");
    opt->add_item (new inst::dbl_val ("0", 0));
    opt->add_item (new inst::dbl_val ("1", -1));
    this->set_opt_param (opt);
}

double
goldstein_price::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double f1 = 1 + std::pow(x1 + x2 + 1, 2.) *
                (19 -
                 14 * x1 +
                 2 * std::pow (x1, 2.) -
                 14 * x2 +
                 6 * x1 * x2 +
                 3 * std::pow (x2, 2.));
    double f2 = 30 + std::pow (2 * x1 - 3 * x2, 2.) *
                (18 -
                 32 * x1 +
                 12 * std::pow (x1, 2.) +
                 48 * x2 -
                 36 * x1 * x2 +
                 27 * std::pow (x2, 2.));

    return f1 * f2;
}

griewank::griewank (int dims):
    synthetic ("griewank", dims, -50., 20., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("griewank opt");
    for (uint i = 0; i < m_dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 0));
    this->set_opt_param (opt);
}

double
griewank::evaluate (inst::set x)
{
    validate_param_set (x);

    double sum = 0., prod = 1.;
    for (uint i = 1; i < m_dims+1; i++) {
        double xi = x->getdbl(i-1);
        sum += std::pow (xi, 2.);
        prod *= std::cos (xi / std::sqrt((double) i));
    }

    return 1./4000. * sum - prod + 1;
}

gulf::gulf ():
    synthetic ("gulf", 3, 0.)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x0 = new sspace::uniform ("0", 0.1, 100.);
    sspace::param_t *x1 = new sspace::uniform ("1", 0., 25.6);
    sspace::param_t *x2 = new sspace::uniform ("2", 0., 5.);
    ss->push_back (x0);
    ss->push_back (x1);
    ss->push_back (x2);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("gulf opt");
    opt->add_item (new inst::dbl_val ("0", 50.));
    opt->add_item (new inst::dbl_val ("1", 25.));
    opt->add_item (new inst::dbl_val ("2", 1.5));
    this->set_opt_param (opt);
}

double
gulf::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);

    double sum = 0.;
    for (int i = 1; i < 99; i++) {
        double ti = (double) i / 100.;
        double ui = 25. + std::pow ( -50. * std::log (ti), 2./3.);
        sum += std::pow (
                std::exp (
                    - (std::pow (ui - x2, x3)/x1)
                    ) - ti
                ,2.);
    }
    return sum;
}

hansen::hansen ():
    synthetic ("hansen", 2, -10., 10., -176.54)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("hansen opt");
    opt->add_item (new inst::dbl_val ("0", -7.58989583));
    opt->add_item (new inst::dbl_val ("1", -7.70831466));
    this->set_opt_param (opt);
}

double
hansen::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double sum1 = 0., sum2 = 0.;
    for (int i = 0; i < 5; i++) {
        sum1 += ((double)i + 1.) *
            std::cos (((double) i * x1) + (double)i + 1.);
        sum2 += ((double)i + 1.) *
            std::cos (((double)i + 2.) * x2 + (double)i + 1.);
    }
    return sum1 * sum2;
}

hartman3::hartman3 ():
    synthetic ("hartman3", 3, 0., 1., -3.86278214782076)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("hartman3 opt");
    opt->add_item (new inst::dbl_val ("0", 0.1));
    opt->add_item (new inst::dbl_val ("1", 0.55592003));
    opt->add_item (new inst::dbl_val ("2", 0.85218259));
    this->set_opt_param (opt);
}

double
hartman3::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[3] = {x->getdbl(0), x->getdbl(1), x->getdbl(2)};

    double a[4][3] = {{3., 10., 30.},
                      {0.1, 10., 35.},
                      {3., 10., 30.},
                      {0.1, 10., 35.}};
    double c[4] = {1, 1.2, 3, 3.2};
    double p[4][3] = {{0.36890, 0.11700, 0.26730},
                      {0.46990, 0.43870, 0.74700},
                      {0.10910, 0.87320, 0.55470},
                      {0.03815, 0.57430, 0.88280}};

    double res = 0.;
    for (int i = 0; i < 4; i++) {
        double s1 = 0.;
        for (int j = 0; j < 3; j++) {
            s1 += a[i][j] * std::pow (xs[j] - p[i][j], 2.);
        }
        res += c[i] * std::exp (- s1);
    }
    return -res;
}

hartman6::hartman6 ():
    synthetic ("hartman6", 6, 0., 1., -3.32236801141551)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("hartman6 opt");
    opt->add_item (new inst::dbl_val ("0", 0.20168952));
    opt->add_item (new inst::dbl_val ("1", 0.15001069));
    opt->add_item (new inst::dbl_val ("2", 0.47687398));
    opt->add_item (new inst::dbl_val ("3", 0.27533243));
    opt->add_item (new inst::dbl_val ("4", 0.31165162));
    opt->add_item (new inst::dbl_val ("5", 0.65730054));
    this->set_opt_param (opt);
}

double
hartman6::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[6];
    for (int i = 0; i < 6; i++) xs[i] = x->getdbl(i);

    double a[4][6] = {{10., 3., 17., 3.5, 1.7, 8. },
                      { 0.05,10., 17., 0.1, 8., 14.},
                      { 3., 3.5, 1.7, 10., 17., 8.},
                      {17., 8. , 0.05, 10., 0.1, 14.}};

    double c[4] = {1, 1.2, 3, 3.2};
    double p[4][6] = {{0.1312, 0.1696, 0.5569, 0.0124, 0.8283, 0.5886},
                      {0.2329, 0.4135, 0.8307, 0.3736, 0.1004, 0.9991},
                      {0.2348, 0.1451, 0.3522, 0.2883, 0.3047, 0.665 },
                      {0.4047, 0.8828, 0.8732, 0.5743, 0.1091, 0.0381}};

    double res = 0.;
    for (int i = 0; i < 4; i++) {
        double s1 = 0.;
        for (int j = 0; j < 6; j++) {
            s1 += a[i][j] * std::pow (xs[j] - p[i][j], 2.);
        }
        res += c[i] * std::exp (- s1);
    }
    return -res;
}

helical_valley::helical_valley ():
    synthetic ("helical_valley", 3, -1., 2., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("helical valley opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    opt->add_item (new inst::dbl_val ("2", 0.));
    this->set_opt_param (opt);
}

double
helical_valley::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);

    return 100. * (
            std::pow (x3 - 10 * (std::atan2 (x2, x1) / (2 * M_PI)), 2.) +
            std::pow (
                std::sqrt (
                    std::pow (x1, 2.) +
                    std::pow (x2, 2.)
                    ) -
                1, 2.)
            ) +
            std::pow (x3, 2.);
}

himmelblau::himmelblau ():
    synthetic ("himmelblau", 2, -5., 5., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("himmelblau opt");
    opt->add_item (new inst::dbl_val ("0", 3.));
    opt->add_item (new inst::dbl_val ("1", 2.));
    this->set_opt_param (opt);
}

double
himmelblau::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return std::pow(
            std::pow (x1, 2.) + x2 - 11,
           2.) +
           std::pow (
            x1 + std::pow (x2, 2.) - 7,
            2.);
}

holder_table::holder_table ():
    synthetic ("holder_table", 2, -10., 10., -19.20850256788675)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("holder_table opt");
    opt->add_item (new inst::dbl_val ("0", 8.055023472141116));
    opt->add_item (new inst::dbl_val ("1", 9.664590028909654));
    this->set_opt_param (opt);
}

double
holder_table::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return - std::fabs (
            std::sin (x1) *
            std::cos (x2) *
            std::exp (
                std::fabs (
                    1 - std::sqrt (
                            std::pow (x1, 2.) +
                            std::pow (x2, 2.)
                        ) / M_PI
                    )
                )
            );
}

hosaki::hosaki ():
    synthetic ("hosaki", 2, -2.3458)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x0 = new sspace::uniform ("0", 0., 5.);
    sspace::param_t *x1 = new sspace::uniform ("1", 0., 6.);
    ss->push_back (x0);
    ss->push_back (x1);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("hosaki opt");
    opt->add_item (new inst::dbl_val ("0", 4.));
    opt->add_item (new inst::dbl_val ("1", 2.));
    this->set_opt_param (opt);
}

double
hosaki::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return (1 + x1 * (-8 + x1 * (7 + x1 * (-7./3. + x1 * 1./4.))))
        * x2
        * x2
        * std::exp (-x2);
}

jennrich_sampson::jennrich_sampson ():
    synthetic ("jennrich_sampson", 2, -1., 1., 124.3621824)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("jennrich sampson opt");
    opt->add_item (new inst::dbl_val ("0", 0.257825));
    opt->add_item (new inst::dbl_val ("1", 0.257825));
    this->set_opt_param (opt);
}

double
jennrich_sampson::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double res = 0.;
    for (int i = 0; i < 11; i++) {
        res += std::pow (
                2. +
                2. * i -
                (std::exp (i * x1) + std::exp (i * x2)),
                2.);
    }
    return res;
}

judge::judge ():
    synthetic ("judge", 2, -10., 10., 16.0817307)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("judge sampson opt");
    opt->add_item (new inst::dbl_val ("0", 0.86479));
    opt->add_item (new inst::dbl_val ("1", 1.2357));
    this->set_opt_param (opt);
}

double
judge::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);


    double A[20] = { 4.284, 4.149, 3.877, 0.533, 2.211, 2.389, 2.145, 3.231,
        1.998, 1.379, 2.106, 1.428, 1.011, 2.179, 2.858, 1.388, 1.651, 1.593,
        1.046, 2.152 };
    double B[20] = { 0.286, 0.973, 0.384, 0.276, 0.973, 0.543, 0.957, 0.948,
        0.543, 0.797, 0.936, 0.889, 0.006, 0.828, 0.399, 0.617, 0.939, 0.784,
        0.072, 0.889, };
    double C[20] = { 0.645, 0.585, 0.310, 0.058, 0.455, 0.779, 0.259, 0.202,
        0.028, 0.099, 0.142, 0.296, 0.175, 0.180, 0.842, 0.039, 0.103, 0.620,
        0.158, 0.704 };

    double res = 0.;
    for (int i = 0; i < 20; i++)
        res += std::pow (
                (x1 + B[i] * x2 + C[i] * std::pow (x2, 2.)) - A[i],
                2.);
    return res;
}

langermann2::langermann2 ():
    synthetic ("langermann2", 2, 0., 10., -5.1621259)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("langermann5 opt");
    opt->add_item (new inst::dbl_val ("0", 2.00299219));
    opt->add_item (new inst::dbl_val ("1", 1.006096));
    this->set_opt_param (opt);
}

double
langermann2::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double a[5] = {3, 5, 2, 1, 7};
    double b[5] = {5, 2, 1, 4, 9};
    double c[5] = {1, 2, 5, 2, 3};

    double res = 0.;
    for (int i = 0; i < 5; i++) {
        double s1 = std::pow (x1 - a[i], 2.) + std::pow (x2 - b[i], 2.);
        res += c[i] *
            std::exp ((-1./M_PI) * s1) *
            std::cos (M_PI * s1);
    }
    return -res;
}

langermann5::langermann5 ():
    synthetic ("langermann5", 10, 0., 10., -1.4)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    /* TODO come back to this */
    inst::node *opt = new inst::node ("langermann5 opt");
    this->set_opt_param (opt);
}

double
langermann5::evaluate (inst::set x)
{
    validate_param_set (x);

    double a[5][10] = {
        {9.681, 0.667, 4.783, 9.095, 3.517, 9.325, 6.544, 0.211, 5.122, 2.020},
        {9.400, 2.041, 3.788, 7.931, 2.882, 2.672, 3.568, 1.284, 7.033, 7.374},
        {8.025, 9.152, 5.114, 7.621, 4.564, 4.711, 2.996, 6.126, 0.734, 4.982},
        {2.196, 0.415, 5.649, 6.979, 9.510, 9.166, 6.304, 6.054, 9.377, 1.426},
        {8.074, 8.777, 3.467, 1.863, 6.708, 6.349, 4.534, 0.276, 7.633, 1.5  }};

    double c[5] = { 0.806, 0.517, 1.5, 0.908, 0.9 };

    double res = 0.;
    for (int i = 0; i < 5; i++) {
        double s1 = 0.;
        for (int j = 0; j < 10; j++)
            s1 += std::pow (x->getdbl(i) - a[i][j], 2.);
        res += c[i] *
            std::exp ((-1./M_PI) * s1) *
            std::cos (M_PI * s1);
    }
    return -res;
}

lennard_jones::lennard_jones ():
    synthetic ("lennard_jones", 6, -3., 3., -1.)
{
    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::non_differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("lennard_jones opt");
    double opts[6] = {-2.66666470373, 2.73904387714, 1.42304625988,
        -1.95553276732, 2.81714839844, 2.12175295546};
    for (int i = 0; i < 6; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), opts[i]));
    this->set_opt_param (opt);
}

double
lennard_jones::evaluate (inst::set x)
{
    validate_param_set (x);

    int k = m_dims/3;
    double res = 0.;
    for (int i = 0; i < k-1; i++) {
        for (int j = i+1; j < k; j++) {
            int a = 3 * i;
            int b = 3 * j;
            double xd = x->getdbl(a) - x->getdbl(b);
            double yd = x->getdbl(a+1) - x->getdbl(b+1);
            double zd = x->getdbl(a+2) - x->getdbl(b+2);
            double ed = xd*xd + yd*yd + zd*zd;
            double ud = std::pow (ed, 3.) + 1e-8;
            if (ed > 0)
                res += (1. / ud - 2.) / ud;
        }
    }
    return res < std::numeric_limits<float>::max() ?
        res :
        std::numeric_limits<float>::max();
}

keane::keane ():
    synthetic ("keane", 2, 0., 10., 0.673668)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("keane opt");
    opt->add_item (new inst::dbl_val ("0", 0));
    opt->add_item (new inst::dbl_val ("1", 1.39325));
    this->set_opt_param (opt);
}

double
keane::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return (std::pow (std::sin (x1 - x2), 2.) *
            std::pow (std::sin (x1 + x2), 2.)) /
           std::sqrt (
               std::pow (x1, 2.) +
               std::pow (x2, 2.)
               );
}

leon::leon ():
    synthetic ("leon", 2, -1.2, 1.2, 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("leon opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 1.));
    this->set_opt_param (opt);
}

double
leon::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return 100. *
        std::pow (x2 - std::pow (x1, 2.), 2.) +
        std::pow (1 - x1, 2.);
}

levy3::levy3 (int dims):
    synthetic ("levy3", dims, -10, 10, 0.)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("levy3 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 1.));
    this->set_opt_param (opt);
}

static double
levy3_w (int i, inst::set x)
{
    return 1 + (x->getdbl(i)-1.)/4.;
}

double
levy3::evaluate (inst::set x)
{
    validate_param_set (x);

    double res = std::pow (std::sin (M_PI * levy3_w(0, x)), 2.);
    for (uint i = 0; i < m_dims-1; i++)
        res += std::pow ( levy3_w(i, x) - 1, 2.) *
            ( 1. +
              10. * std::pow (
                  std::sin ( M_PI * levy3_w(i, x) + 1),
                  2.)
              );

    return res +
        std::pow (levy3_w(m_dims-1,x)-1, 2.) *
        (1 +
         std::pow (
             std::sin (
                 2 * M_PI * levy3_w(m_dims-1, x)
                 ),
             2.)
         );
}

levy5::levy5 ():
    synthetic ("levy5", 2, -2, 2, -135.27125929718)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("levy5 opt");
    opt->add_item (new inst::dbl_val ("0", -0.34893137569));
    opt->add_item (new inst::dbl_val ("1", -0.79113519694));
    this->set_opt_param (opt);
}

double
levy5::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double s1 = 0., s2 = 0.;
    for (int i = 1; i < 6; i++) {
        s1 += i * std::cos ((i-1) * x1 + i);
        s2 += i * std::cos ((i+1) * x2 + i);
    }

    return s1 * s2 +
        std::pow (x1 * 5 + 1.42513, 2.) +
        std::pow (x2 * 5 + 0.80032, 2.);
}

levy13::levy13 ():
    synthetic ("levy13", 2, -10, 10, 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("levy13 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 1.));
    this->set_opt_param (opt);
}

double
levy13::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return
        std::pow (std::sin (3 * M_PI * x1), 2.) +
        std::pow (x1 - 1, 2.) * std::pow (1 + std::sin (3 * M_PI * x2), 2.) +
        std::pow (x2 - 1, 2.) * std::pow (1 + std::sin (2 * M_PI * x2), 2.);
}

matyas::matyas ():
    synthetic ("matyas", 2, -10, 10, 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));

    inst::node *opt = new inst::node ("matyas opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 0.));
    this->set_opt_param (opt);
}

double
matyas::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return 0.26 * (std::pow (x1, 2.) + std::pow (x2, 2.)) -
        0.48 * x1 * x2;
}

mccormick::mccormick():
    synthetic ("mccormick", 2, -1.913222954981037)
{
    sspace::sspace_t *ss = this->get_search_space ();
    sspace::param_t *x0 = new sspace::uniform ("0", -1.5, 4.);
    sspace::param_t *x1 = new sspace::uniform ("1", -3., 3.);
    ss->push_back (x0);
    ss->push_back (x1);

    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mccormick opt");
    opt->add_item (new inst::dbl_val ("0", -0.5471975602214493));
    opt->add_item (new inst::dbl_val ("1", -1.547197559268372));
    this->set_opt_param (opt);
}

double
mccormick::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return
        std::sin (x1 + x2) +
        std::pow (x1 - x2, 2.) -
        1.5 * x1 +
        2.5 * x2 +
        1;
}

// Regression-type functions --------------------------------------------------

regression::regression (
        rdims dims,
        double *e_mat,
        double *centres,
        double *coefs,
        const std::string &n,
        double fmin,
        double *minloc
        ) :
    synthetic (n, dims.prob, 0., 1., fmin),
    m_e_mat(e_mat), m_centres(centres), m_coefs(coefs), m_dims(dims)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node (n + " opt");
    for (int i = 0; i < dims.prob; i++) {
        opt->add_item (new inst::dbl_val (std::to_string(i), minloc[i]));
    }
    this->set_opt_param (opt);
}

double
regression::evaluate (inst::set x) {
    validate_param_set (x);

    double *ret = (double *) calloc (sizeof (double), m_dims.coef);
    double xs[m_dims.prob];
    for (int i = 0; i < m_dims.prob; i++) xs[i] = x->getdbl(i);

    kernel (xs, ret);
    double res = 0.;
    for (int i = 0; i < m_dims.coef; i++)
        res += m_coefs[i] * ret[i];
    free (ret);
    return res;
}

void
regression::dist_sq_1 (double *xs, double *ret)
{
    for (int i = 0; i < m_dims.coef; i++)
        for (int j = 0; j < m_dims.prob; j++)
            ret[i] += std::fabs ((xs[j] - m_centres[i*m_dims.prob+j]) *
                      std::sqrt (m_e_mat[i*m_dims.prob+j]));
}

void
regression::dist_sq_2 (double *xs, double *ret)
{
    for (int i = 0; i < m_dims.coef; i++)
        for (int j = 0; j < m_dims.prob; j++)
            ret[i] +=
                std::pow (xs[j] - m_centres[i*m_dims.prob+j], 2.) *
                m_e_mat[i*m_dims.prob+j];
}


void
regression::dist_sq_inf (double *xs, double *ret)
{

    for (int i = 0; i < m_dims.coef; i++)
        for (int j = 0; j < m_dims.prob; j++) {
            double nval = std::fabs (
                    (xs[j] - m_centres[i*m_dims.prob+j]) *
                    std::sqrt (m_e_mat[i*m_dims.prob+j])
                    );
            if (nval > ret[i])
                ret[i] = nval;
        }
}

court01::court01 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court01",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court01::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1 / std::sqrt (1 + ret[i]);
}

court02::court02 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court02",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court02::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1 / std::sqrt (1 + ret[i]);
}

court03::court03 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court03",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court03::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court04::court04 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court04",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court04::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::cos (M_PI * std::sqrt (ret[i])) * std::exp (-ret[i]);
}

court05::court05 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court05",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court05::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court06::court06 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court06",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court06::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::sqrt (1 + ret[i]);
}

court07::court07 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court07",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court07::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = (1 + ret[i]) * std::exp (-ret[i]);
}

court08::court08 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court08",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court08::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = (1 + ret[i] + 0.333 * std::pow (ret[i], 2.)) *
                std::exp (-ret[i]);
}

court09::court09 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court09",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court09::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::cos (M_PI * std::sqrt (ret[i])) *
            std::exp (-ret[i]);
}

court10::court10 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court10",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court10::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1. / std::sqrt (1 + ret[i]);
}

court11::court11 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court11",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court11::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court13::court13 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court13",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));
}

void
court13::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court14::court14 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court14",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court14::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court15::court15 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court15",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court15::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-std::sqrt (ret[i]));
}

court16::court16 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court16",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court16::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1. / std::sqrt (1 + ret[i]);
}

court17::court17 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court17",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court17::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1. / std::sqrt (1 + ret[i]);
}

court18::court18 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court18",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court18::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = (1. + std::sqrt(ret[i])) * std::exp (-std::sqrt(ret[i]));
}

court19::court19 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court19",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court19::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
}

court20::court20 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court20",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court20::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court21::court21 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court21",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court21::kernel (double *xs, double *ret) {
    dist_sq_inf (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court22::court22 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court22",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court22::kernel (double *xs, double *ret) {
    dist_sq_inf (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court24::court24 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court24",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court24::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1. / (1. + ret[i]);
}

court25::court25 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court25",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court25::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = 1. / (1. + ret[i]);
}

court26::court26 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court26",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court26::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court27::court27 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court27",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court27::kernel (double *xs, double *ret) {
    dist_sq_1 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

court28::court28 ():
    regression (
            dims,
            (double *)e_mat,
            (double *)centres,
            (double *)coefs,
            "mc_court28",
            fmin,
            (double *)minloc
            ) {

    this->set_properties(std::vector<properties>({
                properties::discontinuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::unimodal
                }));
}

void
court28::kernel (double *xs, double *ret) {
    dist_sq_2 (xs, ret);
    for (int i = 0; i < dims.coef; i++)
        ret[i] = std::exp (-ret[i]);
}

michalewicz02::michalewicz02():
    synthetic ("michalewicz02", 2, 0., M_PI, -1.8013034100985499)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("michalewicz02 opt");
    opt->add_item (new inst::dbl_val ("0", 2.202905513296628));
    opt->add_item (new inst::dbl_val ("1", 1.570796322320509));
    this->set_opt_param (opt);
}

double
michalewicz02::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[2];
    xs[0] = x->getdbl(0), xs[1] = x->getdbl(1);

    double res = 0.;
    for (int i = 0; i < 2; i++) {
        res += std::sin (xs[i]) *
            std::pow (
                    std::sin (((i+1) * std::pow(xs[i], 2.)) / M_PI)
                    , 20);
    }
    return -res;
}

michalewicz06::michalewicz06():
    synthetic ("michalewicz06", 6, 0., M_PI, -5.687658179087978)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("michalewicz06 opt");
    opt->add_item (new inst::dbl_val ("0", 2.202905513296628));
    opt->add_item (new inst::dbl_val ("1", 1.570796322320509));
    opt->add_item (new inst::dbl_val ("2", 1.284991564577549));
    opt->add_item (new inst::dbl_val ("3", 1.923058467505610));
    opt->add_item (new inst::dbl_val ("4", 1.720469766517768));
    opt->add_item (new inst::dbl_val ("5", 1.570796319218113));
    this->set_opt_param (opt);
}

double
michalewicz06::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[6];
    for (int i = 0; i < 6; i++)
        xs[i] = x->getdbl(i);

    double res = 0.;
    for (int i = 0; i < 6; i++) {
        res += std::sin (xs[i]) *
            std::pow (
                    std::sin (((i+1) * std::pow(xs[i], 2.)) / M_PI)
                    , 20);
    }
    return -res;
}

michalewicz12::michalewicz12():
    synthetic ("michalewicz12", 12, 0., M_PI, -11.595826967415329)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("michalewicz12 opt");
    opt->add_item (new inst::dbl_val ("0",  2.202905513296628));
    opt->add_item (new inst::dbl_val ("1",  1.570796322320509));
    opt->add_item (new inst::dbl_val ("2",  1.284991564577549));
    opt->add_item (new inst::dbl_val ("3",  1.923058467505610));
    opt->add_item (new inst::dbl_val ("4",  1.720469766517768));
    opt->add_item (new inst::dbl_val ("5",  1.570796319218113));
    opt->add_item (new inst::dbl_val ("6",  1.454413962081172));
    opt->add_item (new inst::dbl_val ("7",  1.756086513575824));
    opt->add_item (new inst::dbl_val ("8",  1.655717409323190));
    opt->add_item (new inst::dbl_val ("9",  1.570796319387859));
    opt->add_item (new inst::dbl_val ("10", 1.497728796097675));
    opt->add_item (new inst::dbl_val ("11", 1.923739461688219));
    this->set_opt_param (opt);
}

double
michalewicz12::evaluate (inst::set x)
{
    validate_param_set (x);

    double xs[12];
    for (int i = 0; i < 12; i++)
        xs[i] = x->getdbl(i);

    double res = 0.;
    for (int i = 0; i < 12; i++) {
        res += std::sin (xs[i]) *
            std::pow (
                    std::sin (((i+1) * std::pow(xs[i], 2.)) / M_PI)
                    , 20);
    }
    return -res;
}

miele_cantrell::miele_cantrell():
    synthetic ("miele_cantrell", 4, -1., 1., 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("miele_cantrell opt");
    opt->add_item (new inst::dbl_val ("0", 0.));
    opt->add_item (new inst::dbl_val ("1", 1.));
    opt->add_item (new inst::dbl_val ("2", 1.));
    opt->add_item (new inst::dbl_val ("3", 1.));
    this->set_opt_param (opt);
}

double
miele_cantrell::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);
    double x3 = x->getdbl(2), x4 = x->getdbl(3);

    return
        std::pow (std::exp (-x1) - x2, 4.) +
        100 * std::pow (x2 - x3, 6.) +
        std::pow (std::tan (x3 - x4), 4.) +
        std::pow (x1, 8.);

}

mishra01::mishra01(int dims):
    synthetic ("mishra01", dims, -0., 1., 2)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra01 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 1.));
    this->set_opt_param (opt);
}

double
mishra01::evaluate (inst::set x)
{
    validate_param_set (x);

    double sx = 0.;
    for (uint i = 0; i < m_dims-1; i++)
        sx += x->getdbl(i);
    double gn = m_dims - sx;
    return std::pow (1 + gn, gn);
}

mishra02::mishra02(int dims):
    synthetic ("mishra02", dims, -0., 1., 2)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra02 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 1.));
    this->set_opt_param (opt);
}

double
mishra02::evaluate (inst::set x)
{
    validate_param_set (x);

    double sx = 0.;
    for (uint i = 0; i < m_dims-1; i++)
        sx += (x->getdbl(i) + x->getdbl(i+1)) / 2.;
    double gn = m_dims - sx;
    return std::pow (1 + gn, gn);
}

mishra03::mishra03():
    synthetic ("mishra03", 2, -10., 10., -0.184651333342989)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra03 opt");
    opt->add_item (new inst::dbl_val ("0", -8.466613775046579));
    opt->add_item (new inst::dbl_val ("1", -9.998521308999999));
    this->set_opt_param (opt);
}

double
mishra03::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return std::pow (
            std::fabs(
                std::cos (
                    std::fabs (
                        std::pow (x1, 2.) + std::pow (x2, 2.)
                        )
                    )
                )
            , 0.5) +
        (x1 + x2) / 100.;
}

mishra04::mishra04():
    synthetic ("mishra04", 2, -10., 10., -0.199406970088833)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra04 opt");
    opt->add_item (new inst::dbl_val ("0", -9.941127263635860));
    opt->add_item (new inst::dbl_val ("1", -9.999571661999983));
    this->set_opt_param (opt);
}

double
mishra04::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return std::pow (
            std::fabs(
                std::sin (
                    std::fabs (
                        std::pow (x1, 2.) + std::pow (x2, 2.)
                        )
                    )
                )
            , 0.5) +
        (x1 + x2) / 100.;
}

mishra05::mishra05():
    synthetic ("mishra05", 2, -10., 10., -1.019829519930943)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra05 opt");
    opt->add_item (new inst::dbl_val ("0", -1.986820662153768));
    opt->add_item (new inst::dbl_val ("1", -10.));
    this->set_opt_param (opt);
}

double
mishra05::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double f1 =
        std::pow (std::sin (std::pow(std::cos(x1) + std::cos(x2), 2.)), 2.);
    double f2 =
        std::pow (std::sin (std::pow(std::cos(x1) + std::cos(x2), 2.)), 2.);

    return std::pow (f1 + f2 + x1, 2.) + 0.01 * x1 + 0.1 * x2;
}

mishra06::mishra06():
    synthetic ("mishra06", 2, -10., 10., -2.283949838474759)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra06 opt");
    opt->add_item (new inst::dbl_val ("0", 2.886307215440481));
    opt->add_item (new inst::dbl_val ("1", 1.823260331422321));
    this->set_opt_param (opt);
}

double
mishra06::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double f1 =
        std::pow (std::sin (std::pow (std::cos (x1) + std::cos (x2), 2.)), 2.);
    double f2 =
        std::pow (std::cos (std::pow (std::sin (x1) + std::sin (x2), 2.)), 2.);

    double f3 =
        0.1 * (std::pow (x1-1, 2.) + std::pow (x2 - 1, 2.));

    return - std::log (std::pow (f1 - f2 + x1, 2.)) + f3;
}

mishra08::mishra08():
    synthetic ("mishra08", 2, -10., 10., 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra08 opt");
    opt->add_item (new inst::dbl_val ("0", 2.));
    opt->add_item (new inst::dbl_val ("1", -3.));
    this->set_opt_param (opt);
}

double
mishra08::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    double gc[11] =
        {1, -20, 180, -960, 3360, -8064, 11340, -15360, 11520, -5120, 2624};
    double hc[5] = {1, 12, 54, 108, 81};

    double g = 0.;
    for (int i = 0; i < 11; i++)
        g += gc[i] * std::pow (x1, 10 - i);
    double h = 0.;
    for (int i = 0; i < 5; i++)
        g += hc[i] * std::pow (x2, 4 - i);
    return 0.001 * std::pow (std::fabs (g) * std::fabs (h), 2.);
}

mishra09::mishra09():
    synthetic ("mishra09", 3, -10., 10., 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra09 opt");
    opt->add_item (new inst::dbl_val ("0", 1.));
    opt->add_item (new inst::dbl_val ("1", 2.));
    opt->add_item (new inst::dbl_val ("2", 3.));
    this->set_opt_param (opt);
}

double
mishra09::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1), x3 = x->getdbl(2);

    double a =
        2. * std::pow (x1, 3.) +
        5. * x1 * std::pow (x2, 2.) +
        4. * x3 -
        2. * std::pow (x1, 2.) * x3 -
        18.;
    double b =
        x1 +
        std::pow (x2, 3.) +
        x1 * std::pow (x2, 2.) +
        x1 * std::pow (x3, 2.) -
        22.;
    double c = 8 * std::pow (x1, 2.) +
        2. * x2 * x3 +
        2. * std::pow (x2, 2.) +
        3. * std::pow (x2, 3.) -
        52.;

    return std::pow (
            a * std::pow (b, 2.) * c +
            a * b * std::pow (c, 2.) +
            std::pow (b, 2.) +
            std::pow (x1 + x2 - x3, 2.)
            , 2.);
}

mishra10::mishra10():
    synthetic ("mishra10", 2, -10., 10., 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::non_scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra10 opt");
    opt->add_item (new inst::dbl_val ("0", 2));
    opt->add_item (new inst::dbl_val ("1", 2.));
    this->set_opt_param (opt);
}

double
mishra10::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl(0), x2 = x->getdbl(1);

    return std::pow (
                (std::floor (x1) * std::floor(x2)) -
                std::floor (x1) -
                std::floor (x2)
            , 2.);
}

mishra11::mishra11 (int dims):
    synthetic ("mishra11", dims, 0., 10., 0)
{
    this->set_properties(std::vector<properties>({
                properties::continuous,
                properties::differentiable,
                properties::non_separable,
                properties::scalable,
                properties::multimodal
                }));

    inst::node *opt = new inst::node ("mishra11 opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string(i), 5));
    this->set_opt_param (opt);
}

double
mishra11::evaluate (inst::set x)
{
    validate_param_set (x);

    double sx = 0., px = 1.;
    for (uint i = 0; i < m_dims; i++) {
        double xi = std::fabs (x->getdbl(i)); sx += xi;
        px *= xi;
    }

    return std::pow (
            (1./(double) m_dims) * sx -
            std::pow (px, 1./(double) m_dims)
            , 2.);
}

manifoldmin::manifoldmin (int dims):
    synthetic ("manifoldmin", dims, -10., 10., 0.)
{
    this->set_properties(std::vector<properties>({
                properties::multimodal,
                properties::scalable
                }));

    inst::node *opt = new inst::node ("manifoldmin opt");
    for (int i = 0; i < dims; i++)
        opt->add_item (new inst::dbl_val (std::to_string (i), 0));
    this->set_opt_param (opt);
}

double
manifoldmin::evaluate (inst::set x)
{
    validate_param_set (x);

    double sum = 0., prod = 1.;
    for (uint i = 0; i < m_dims; i++) {
        double xi = std::fabs (x->getdbl(i));
        sum += xi; prod *= xi;
    }
    return sum * prod;
}


mog01::mog01 ():
    synthetic ("mixtureofgaussians01", 2, -1, 1, -0.50212488514)
{
    this->set_properties(std::vector<properties>({
                properties::non_scalable,
                properties::continuous,
                properties::multimodal
                }));


    inst::node *opt = new inst::node ("mixtureofgaussians01 opt");
    opt->add_item (new inst::dbl_val ("0", -0.19870980807));
    opt->add_item (new inst::dbl_val ("1", -0.49764469526));
    this->set_opt_param (opt);
}

double
mog01::evaluate (inst::set x)
{
    validate_param_set (x);

    double x1 = x->getdbl("0"), x2 = x->getdbl("1");

    return - (
            0.5 * std::exp ( -10 * (
                    0.8 * std::pow (x1 + 0.2, 2.) +
                    .7  * std::pow (x2 + 0.5, 2.)
                    )
                ) +
            0.5 * std::exp ( -8 * (
                    0.3 * std::pow (x1 - 0.8, 2.) +
                    0.6 * std::pow (x2 - 0.3, 2.)
                )
            )
        );
}

synthetic_benchmark::synthetic_benchmark ():
    benchmark_set ("synthetic")
{ }

#define bench(benchmark, b) \
    benchmark b; \
    of << b.get_name() << "," << opt->get_name(); \
    optk::core_loop (&b, opt, trace); \
    for (uint i = 0; i < ctx->max_iters; i++) \
        of << "," << trace[i]; \
    of << std::endl;

#define bench_scalable(benchmark, b, dims) \
    benchmark b(dims); \
    of << b.get_name() << "," << opt->get_name(); \
    optk::core_loop (&b, opt, trace); \
    for (uint i = 0; i < ctx->max_iters; i++) \
        of << "," << trace[i]; \
    of << std::endl;

//* @todo parallelise this function - embarrassing! */
void
synthetic_benchmark::run (optk::optimisers *opts, optk::ctx_t *ctx)
{
    double trace[ctx->max_iters];

    std::ofstream of;
    of.open(ctx->outfile, std::ios::app);

    // for all the optimisers in the set
    std::vector<optk::optimiser *> *optc = opts->collection();
    for (uint i = 0; i < optc->size(); i++) {
        optk::optimiser *opt = optc->at(i);

        // for all the synthetic benchmarks
        bench_scalable(syn::ackley1, a1, 10);
        bench(syn::ackley2, a2);
        bench(syn::ackley3, a3);
        bench(syn::adjiman, am);
        bench_scalable(syn::alpine1, al1, 10);
        bench_scalable(syn::alpine2, al2, 10);
        bench(syn::brad, bd);
        bench(syn::bartels_conn, bc);
        bench(syn::beale, be);
        bench(syn::biggs_exp2, bx2);
        bench(syn::biggs_exp3, bx3);
        bench(syn::biggs_exp4, bx4);
        bench(syn::biggs_exp5, bx5);
        bench(syn::biggs_exp6, bx6);
        bench(syn::bird, brd);
        bench(syn::bohachevsky1, bchy1);
        bench(syn::bohachevsky2, bchy2);
        bench(syn::bohachevsky3, bchy3);
        bench(syn::booth, booth);
        bench(syn::box_betts, bb);
        bench(syn::branin1, br1);
        bench(syn::branin2, br2);
        bench(syn::brent, br);
        bench_scalable(syn::brown, brn, 10);
        bench(syn::bukin2, bk2);
        bench(syn::bukin4, bk4);
        bench(syn::bukin6, bk6);
        bench(syn::camel3, c3);
        bench(syn::camel6, c6);
        bench(syn::chichinadze, cz);
        bench_scalable(syn::chung_reynolds, cr, 10);
        bench(syn::cola, cla);
        bench(syn::colville, cv);
        bench_scalable(syn::cosine_mixture, cm4, 4);
        bench_scalable(syn::cosine_mixture, cm15, 15);
        bench(syn::cross_in_tray, cit);
        bench_scalable(syn::csendes, cs, 10);
        bench(syn::cube, cbe);
        bench(syn::damavandi, dam);
        bench_scalable(syn::deb1, deb1, 10);
        bench_scalable(syn::deb2, deb2, 10);
        bench(syn::deckkers_aarts, da);
        bench(syn::devillers_glasser1, dvg1);
        bench(syn::devillers_glasser2, dvg2);
        bench_scalable(syn::dixon_price, dxp, 10);
        bench(syn::dolan, dln);
        bench_scalable(syn::deceptive, dec, 10);
        bench_scalable(syn::deceptive, dcs, 10);
        bench(syn::drop_wave, dw);
        bench(syn::easom, esm);
        bench(syn::egg_crate, egc);
        bench(syn::egg_holder, egh);
        bench(syn::el_attar_vidyasagar_dutta, elatt);
        bench_scalable(syn::exponential, exp, 10);
        bench(syn::exp2, exp2);
        bench(syn::franke, fnk);
        bench(syn::freudenstein_roth, frr);
        bench(syn::gear, gear);
        bench(syn::giunta, gta);
        bench(syn::goldstein_price, gsp);
        bench_scalable(syn::griewank, gwk, 10);
        bench(syn::gulf, gulf);
        bench(syn::hansen, hsn);
        bench(syn::hartman3, hrt3);
        bench(syn::hartman6, hrt6);
        bench(syn::helical_valley, hv);
        bench(syn::himmelblau, hb);
        bench(syn::holder_table, ht);
        bench(syn::hosaki, hki);
        bench(syn::jennrich_sampson, js);
        bench(syn::judge, jd);
        bench(syn::langermann2, l2);
        bench(syn::lennard_jones, lj);
        bench(syn::keane, kn);
        bench(syn::keane, ln);
        bench_scalable(syn::levy3, lv3, 10);
        bench(syn::levy5, lv5);
        bench(syn::levy13, lv13);
        bench(syn::matyas, mts);
        bench(syn::mccormick, mck);
        bench(syn::michalewicz02, mcz02);
        bench(syn::michalewicz06, mcz06);
        bench(syn::michalewicz12, mcz12);
        bench(syn::miele_cantrell, mcll);
        bench_scalable(syn::mishra01, mish01,10);
        bench_scalable(syn::mishra02, mish02, 10);
        bench(syn::mishra03, mish03);
        bench(syn::mishra04, mish04);
        bench(syn::mishra05, mish05);
        bench(syn::mishra06, mish06);
        // TODO fix this one - output seems wrong
        // bench(syn::mishra08, mish08);
        bench(syn::mishra09, mish09);
        bench(syn::mishra10, mish10);
        bench_scalable(syn::mishra11, mish11, 10);

        bench(syn::court01, crt1);
        bench(syn::court02, crt2);
        bench(syn::court03, crt3);
        bench(syn::court04, crt4);
        bench(syn::court05, crt5);
        bench(syn::court06, crt6);
        bench(syn::court07, crt7);
        bench(syn::court08, crt8);
        bench(syn::court09, crt9);
        bench(syn::court10, crt10);
        bench(syn::court11, crt11);
        bench(syn::court13, crt13);
        bench(syn::court14, crt14);
        bench(syn::court15, crt15);
        bench(syn::court16, crt16);
        bench(syn::court17, crt17);
        bench(syn::court18, crt18);
        bench(syn::court19, crt19);
        bench(syn::court20, crt20);
        bench(syn::court21, crt21);
        bench(syn::court22, crt22);
        bench(syn::court24, crt24);
        bench(syn::court25, crt25);
        bench(syn::court26, crt26);
        bench(syn::court27, crt27);
        bench(syn::court28, crt28);
    }

    of.close();
}

} // end namespace syn

