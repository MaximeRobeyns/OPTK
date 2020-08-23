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
 * @brief Implements tests for the benchmark algorithms algorithm.
 */

#include <tests/benchmark_test.hpp>

#include <benchmarks/synthetic.hpp>

static int64_t
ulps_distance(const double a, const double b)
{
    // Save work if the floats are equal.
    // Also handles +0 == -0
    if (a == b) return 0;

    const auto max = std::numeric_limits<int64_t>::max();

    // Max distance for NaN
    if (std::isnan(a) || std::isnan(b)) return max;

    // If one's infinite and they're not equal, max distance.
    if (std::isinf(a) || std::isinf(b)) return max;

    int64_t ia, ib;
    memcpy(&ia, &a, sizeof(double));
    memcpy(&ib, &b, sizeof(double));

    // Don't compare differently-signed doubles
    if ((ia < 0) != (ib < 0)) return max;

    // Return the absolute value of the distance in ULPs.
    int64_t distance = ia - ib;
    if (distance < 0) distance = -distance;
    return distance;
}

static bool
nearly_equal(
        double a, double b,
        double fixedEpsilon = 1.e-10,
        int ulpsEpsilon = 10
) {
    // Handle the near-zero case.
    const double difference = fabs(a - b);
    if (difference <= fixedEpsilon) return true;

    return ulps_distance(a, b) <= ulpsEpsilon;
}

static void
test_synthetic_benchmarks ()
{
    // ackley1
    syn::ackley1 a1(10);

    assert (a1.get_name() == std::string("ackley1"));
    sspace::sspace_t *tmp_space = a1.get_search_space ();
    u_int dims = a1.get_dims();
    assert (dims == 10u);

    // iterate throguh all 5 parameters.
    for (u_int i = 0; i < dims; i++) {
        sspace::param_t *tmp = tmp_space->at(i);
        assert (tmp->get_type () == pt::uniform);
        sspace::uniform *tmp_uni = static_cast<sspace::uniform *>(tmp);
        assert (tutils::dbleq (tmp_uni->m_lower, -35));
        assert (tutils::dbleq (tmp_uni->m_upper, 35));
        assert (tmp_uni->get_name () == std::to_string (i));
    }

    double resa1 = a1.evaluate(a1.get_opt_param ());
    assert (nearly_equal (resa1, 0.));

    // ackley2
    syn::ackley2 a2;
    assert (a2.get_dims() == 2u);
    double resa2 = a2.evaluate(a2.get_opt_param ());
    assert (nearly_equal (resa2, -200));

    // ackley3
    syn::ackley3 a3;
    assert (a3.get_dims() == 2u);
    double resa3 = a3.evaluate(a3.get_opt_param ());
    assert (nearly_equal (resa3, a3.get_opt()));

    // adjiman
    syn::adjiman am;
    assert (am.get_dims() == 2u);
    double resam = am.evaluate(am.get_opt_param ());
    assert (nearly_equal (resam, am.get_opt(), 1.e-5));

    // alpine1
    syn::alpine1 al1(10);
    double resal1 = al1.evaluate(al1.get_opt_param ());
    assert (nearly_equal (resal1, al1.get_opt()));

    // alpine2
    syn::alpine2 al2(10);
    double resal2 = al2.evaluate(al2.get_opt_param ());
    assert (nearly_equal (resal2, al2.get_opt(), 1e-3*std::exp(al2.get_dims())));

    // brad
    syn::brad bd;
    double resbd = bd.evaluate(bd.get_opt_param ());
    std::cout << "resbd opt: " << resbd << std::endl;
    // assert (nearly_equal (resbd, bd.get_opt()));
}

void
run_benchmark_tests()
{
    test_synthetic_benchmarks();
    std::cout << "All gridsearch tests pass" << std::endl;
}
