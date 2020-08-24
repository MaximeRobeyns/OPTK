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
    assert (nearly_equal (resbd, bd.get_opt(), 1e-8));

    // bartels_conn
    syn::bartels_conn bc;
    double resbc = bc.evaluate(bc.get_opt_param ());
    assert (nearly_equal (resbc, bc.get_opt()));

    // beale
    syn::beale be;
    double resbe = be.evaluate(be.get_opt_param ());
    assert (nearly_equal (resbe, be.get_opt()));

    // biggs_exp2
    syn::biggs_exp2 bx2;
    double resbx2 = bx2.evaluate(bx2.get_opt_param ());
    assert (nearly_equal (resbx2, bx2.get_opt()));

    // biggs_exp3
    syn::biggs_exp3 bx3;
    double resbx3 = bx3.evaluate(bx3.get_opt_param ());
    assert (nearly_equal (resbx3, bx3.get_opt()));

    // biggs_exp4
    syn::biggs_exp4 bx4;
    double resbx4 = bx4.evaluate(bx4.get_opt_param ());
    assert (nearly_equal (resbx4, bx4.get_opt()));

    // biggs_exp5
    syn::biggs_exp5 bx5;
    double resbx5 = bx5.evaluate(bx5.get_opt_param ());
    assert (nearly_equal (resbx5, bx5.get_opt()));

    // biggs_exp6
    syn::biggs_exp6 bx6;
    double resbx6 = bx6.evaluate(bx6.get_opt_param ());
    assert (nearly_equal (resbx6, bx6.get_opt()));

    // bird
    syn::bird brd;
    double resbrd = brd.evaluate(brd.get_opt_param ());
    assert (nearly_equal (resbrd, brd.get_opt(), 1e-6));

    // bohachevsky1
    syn::bohachevsky1 bchy1;
    double resbchy1 = bchy1.evaluate(bchy1.get_opt_param ());
    assert (nearly_equal (resbchy1, bchy1.get_opt()));

    // bohachevsky2
    syn::bohachevsky2 bchy2;
    double resbchy2 = bchy2.evaluate(bchy2.get_opt_param ());
    assert (nearly_equal (resbchy2, bchy2.get_opt()));

    // bohachevsky3
    syn::bohachevsky3 bchy3;
    double resbchy3 = bchy3.evaluate(bchy3.get_opt_param ());
    assert (nearly_equal (resbchy3, bchy3.get_opt()));

    // booth
    syn::booth booth;
    double resbooth = booth.evaluate(booth.get_opt_param ());
    assert (nearly_equal (resbooth, booth.get_opt()));

    // Box-Betts Quadratic Sum Function
    syn::box_betts bb;
    double resbb = bb.evaluate(bb.get_opt_param ());
    assert (nearly_equal (resbb, bb.get_opt()));

    // branin1
    syn::branin1 br1;
    double resbr1 = br1.evaluate(br1.get_opt_param ());
    assert (nearly_equal (resbr1, br1.get_opt()));

    // branin2
    syn::branin2 br2;
    double resbr2 = br2.evaluate(br2.get_opt_param ());
    assert (nearly_equal (resbr2, br2.get_opt(), 1e-6));

    // brent
    syn::brent br;
    double resbr = br.evaluate(br.get_opt_param ());
    assert (nearly_equal (resbr, br.get_opt()));

    // brown(10)
    syn::brown brn(10);
    double resbrn = brn.evaluate(brn.get_opt_param ());
    assert (nearly_equal (resbrn, brn.get_opt()));

    // bukin2
    syn::bukin2 bk2;
    double resbk2 = bk2.evaluate(bk2.get_opt_param ());
    assert (nearly_equal (resbk2, bk2.get_opt()));

    // bukin4
    syn::bukin4 bk4;
    double resbk4 = bk4.evaluate(bk4.get_opt_param ());
    assert (nearly_equal (resbk4, bk4.get_opt()));

    // bukin6
    syn::bukin6 bk6;
    double resbk6 = bk6.evaluate(bk6.get_opt_param ());
    assert (nearly_equal (resbk6, bk6.get_opt()));

    // camel3
    syn::camel3 c3;
    double resc3 = c3.evaluate(c3.get_opt_param ());
    assert (nearly_equal (resc3, c3.get_opt()));

    // camel6
    syn::camel6 c6;
    double resc6 = c6.evaluate(c6.get_opt_param ());
    assert (nearly_equal (resc6, c6.get_opt(), 1e-6));

    // chen bird
    syn::chichinadze cz;
    double rescz = cz.evaluate(cz.get_opt_param ());
    assert (nearly_equal (rescz, cz.get_opt()));

    // chung reynolds (10)
    syn::chung_reynolds cr(10);
    double rescr = cr.evaluate(cr.get_opt_param ());
    assert (nearly_equal (rescr, cr.get_opt()));

    // cola
    syn::cola cla;
    double rescla = cla.evaluate(cla.get_opt_param ());
    assert (nearly_equal (rescla, cla.get_opt(), 1e-5));

    // colville
    syn::colville cv;
    double rescv = cv.evaluate(cv.get_opt_param ());
    assert (nearly_equal (rescv, cv.get_opt()));

    // corana
    // syn::corana cna;
    // double rescna = cna.evaluate(cna.get_opt_param ());
    // assert (nearly_equal (rescna, cna.get_opt()));

    // cosine_mixture4
    syn::cosine_mixture cm4(4);
    double rescm4 = cm4.evaluate(cm4.get_opt_param ());
    assert (nearly_equal (rescm4, cm4.get_opt()));

    // cosine_mixture15
    syn::cosine_mixture cm15(15);
    double rescm15 = cm15.evaluate(cm15.get_opt_param ());
    assert (nearly_equal (rescm15, cm15.get_opt()));

    // cross in tray
    syn::cross_in_tray cit;
    double rescit = cit.evaluate(cit.get_opt_param ());
    assert (nearly_equal (rescit, cit.get_opt()));

    // csendes
    syn::csendes cs(10);
    double rescs = cs.evaluate(cs.get_opt_param ());
    assert (nearly_equal (rescs, cs.get_opt()));

    // cube
    syn::cube cbe;
    double rescbe = cbe.evaluate(cbe.get_opt_param ());
    assert (nearly_equal (rescbe, cbe.get_opt()));

    // damavandi
    syn::damavandi dam;
    double resdam = dam.evaluate(dam.get_opt_param ());
    assert (nearly_equal (resdam, dam.get_opt()));

    // deb1
    syn::deb1 deb1(10);
    double resdeb1 = deb1.evaluate(deb1.get_opt_param ());
    assert (nearly_equal (resdeb1, deb1.get_opt()));

    // deb2
    syn::deb2 deb2(10);
    double resdeb2 = deb2.evaluate(deb2.get_opt_param ());
    assert (nearly_equal (resdeb2, deb2.get_opt()));

    // deckkers_aarts
    syn::deckkers_aarts da;
    double resda = da.evaluate(da.get_opt_param ());
    assert (nearly_equal (resda, da.get_opt(), 6));

    // devillers_glasser1
    syn::devillers_glasser1 dvg1;
    double resdvg1 = dvg1.evaluate(dvg1.get_opt_param ());
    assert (nearly_equal (resdvg1, dvg1.get_opt(), 1e-1));

    // devillers_glasser 2
    syn::devillers_glasser2 dvg2;
    double resdvg2 = dvg2.evaluate(dvg2.get_opt_param ());
    assert (nearly_equal (resdvg2, dvg2.get_opt(), 2));

    // dixon & price (10)
    syn::dixon_price dxp (10);
    double resdxp = dxp.evaluate(dxp.get_opt_param ());
    assert (nearly_equal (resdxp, dxp.get_opt()));

    // dolan
    syn::dolan dln;
    double resdln = dln.evaluate(dln.get_opt_param ());
    assert (nearly_equal (resdln, dln.get_opt()));

    // deceptive (10)
    syn::deceptive dec (10);
    double resdec = dec.evaluate(dec.get_opt_param ());
    assert (nearly_equal (resdec, dec.get_opt()));
}

void
run_benchmark_tests()
{
    test_synthetic_benchmarks();
    std::cout << "All gridsearch tests pass" << std::endl;
}
