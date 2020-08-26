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

    // deflected corrugated spring
    syn::deceptive dcs (10);
    double resdcs = dcs.evaluate(dcs.get_opt_param ());
    assert (nearly_equal (resdcs, dcs.get_opt()));

    // drop wave
    syn::drop_wave dw;
    double resdw = dw.evaluate(dw.get_opt_param ());
    assert (nearly_equal (resdw, dw.get_opt()));

    // easom wave
    syn::easom esm;
    double resesm = esm.evaluate(esm.get_opt_param ());
    assert (nearly_equal (resesm, esm.get_opt()));

    // egg create
    syn::egg_crate egc;
    double resegc = egc.evaluate(egc.get_opt_param ());
    assert (nearly_equal (resegc, egc.get_opt()));

    // egg holder
    syn::egg_holder egh;
    double resegh = egh.evaluate(egh.get_opt_param ());
    assert (nearly_equal (resegh, egh.get_opt(), 1e-9));

    // el_attar_vidyasagar_dutta
    syn::el_attar_vidyasagar_dutta elatt;
    double reselatt = elatt.evaluate(elatt.get_opt_param ());
    assert (nearly_equal (reselatt, elatt.get_opt(), 1e-9));

    // exponential
    syn::exponential exp (10);
    double resexp = exp.evaluate(exp.get_opt_param ());
    assert (nearly_equal (resexp, exp.get_opt()));

    // exp2
    syn::exp2 exp2;
    double resexp2 = exp2.evaluate(exp2.get_opt_param ());
    assert (nearly_equal (resexp2, exp2.get_opt()));

    // franke
    syn::franke fnk;
    double resfnk = fnk.evaluate(fnk.get_opt_param ());
    assert (nearly_equal (resfnk, fnk.get_opt(), 1e-1));

    // freudenstein_roth
    syn::freudenstein_roth frr;
    double resfrr = frr.evaluate(frr.get_opt_param ());
    assert (nearly_equal (resfrr, frr.get_opt()));

    // gear
    syn::gear gear;
    double resgear = gear.evaluate(gear.get_opt_param ());
    assert (nearly_equal (resgear, gear.get_opt()));

    // ginuta
    syn::giunta gta;
    double resgta = gta.evaluate(gta.get_opt_param ());
    assert (nearly_equal (resgta, gta.get_opt()));

    // goldstein_price
    syn::goldstein_price gsp;
    double resgsp = gsp.evaluate(gsp.get_opt_param ());
    assert (nearly_equal (resgsp, gsp.get_opt()));

    // griewank
    syn::griewank gwk(10);
    double resgwk = gwk.evaluate(gwk.get_opt_param ());
    assert (nearly_equal (resgwk, gwk.get_opt()));

    // gulf research problem
    syn::gulf gulf;
    double resgulf = gulf.evaluate(gulf.get_opt_param ());
    assert (nearly_equal (resgulf, gulf.get_opt(), 1e-30));

    // hansen
    syn::hansen hsn;
    double reshsn = hsn.evaluate(hsn.get_opt_param ());
    assert (nearly_equal (reshsn, hsn.get_opt(), 1e-2));

    // hartman3
    syn::hartman3 hrt3;
    double reshrt3 = hrt3.evaluate(hrt3.get_opt_param ());
    assert (nearly_equal (reshrt3, hrt3.get_opt(), 1e-3));

    // hartman6
    syn::hartman6 hrt6;
    double reshrt6 = hrt6.evaluate(hrt6.get_opt_param ());
    assert (nearly_equal (reshrt6, hrt6.get_opt(), 1e-3));

    // helical valley
    syn::helical_valley hv;
    double reshv = hv.evaluate(hv.get_opt_param ());
    assert (nearly_equal (reshv, hv.get_opt()));

    // himmelblau
    syn::himmelblau hb;
    double reshb = hb.evaluate(hb.get_opt_param ());
    assert (nearly_equal (reshb, hb.get_opt()));

    // holder table
    syn::holder_table ht;
    double resht = ht.evaluate(ht.get_opt_param ());
    assert (nearly_equal (resht, ht.get_opt()));

    // hosaki
    syn::hosaki hki;
    double reshki = hki.evaluate(hki.get_opt_param ());
    assert (nearly_equal (reshki, hki.get_opt(), 1e-4));

    // jennrich_sampson
    syn::jennrich_sampson js;
    double resjs = js.evaluate(js.get_opt_param ());
    assert (nearly_equal (resjs, js.get_opt(), 1e-7));

    // judge
    syn::judge jd;
    double resjd = jd.evaluate(jd.get_opt_param ());
    assert (nearly_equal (resjd, jd.get_opt(), 1e-6));

    // langermann 2
    syn::langermann2 l2;
    double resl2 = l2.evaluate(l2.get_opt_param ());
    assert (nearly_equal (resl2, l2.get_opt(), 1e-6));

    // lennard jones
    syn::lennard_jones lj;
    double reslj = lj.evaluate(lj.get_opt_param ());
    assert (nearly_equal (reslj, lj.get_opt()));

    // keane jones
    syn::keane kn;
    double reskn = kn.evaluate(kn.get_opt_param ());
    assert (nearly_equal (reskn, kn.get_opt(), 1e-6));

    // leon jones
    syn::keane ln;
    double resln = ln.evaluate(ln.get_opt_param ());
    assert (nearly_equal (resln, ln.get_opt(), 1e-6));

    // levy3
    syn::levy3 lv3(10);
    double reslv3 = lv3.evaluate(lv3.get_opt_param ());
    assert (nearly_equal (reslv3, lv3.get_opt()));

    // levy5
    syn::levy5 lv5;
    double reslv5 = lv5.evaluate(lv5.get_opt_param ());
    assert (nearly_equal (reslv5, lv5.get_opt()));

    // levy13
    syn::levy13 lv13;
    double reslv13 = lv13.evaluate(lv13.get_opt_param ());
    assert (nearly_equal (reslv13, lv13.get_opt()));

    // matyas
    syn::matyas mts;
    double reslmts = mts.evaluate(mts.get_opt_param ());
    assert (nearly_equal (reslmts, mts.get_opt()));

    // mccormick
    syn::mccormick mck;
    double resmck = mck.evaluate(mck.get_opt_param ());
    assert (nearly_equal (resmck, mck.get_opt()));

    // michalewicz02
    syn::michalewicz02 mcz02;
    double resmcz02 = mcz02.evaluate(mcz02.get_opt_param ());
    assert (nearly_equal (resmcz02, mcz02.get_opt()));

    // michalewicz06
    syn::michalewicz06 mcz06;
    double resmcz06 = mcz06.evaluate(mcz06.get_opt_param ());
    assert (nearly_equal (resmcz06, mcz06.get_opt()));

    // michalewicz12
    syn::michalewicz12 mcz12;
    double resmcz12 = mcz12.evaluate(mcz12.get_opt_param ());
    assert (nearly_equal (resmcz12, mcz12.get_opt()));
}

static void
test_regression_benchmarks ()
{
    syn::court01 c1;
    assert (c1.get_name() == std::string("mc_court01"));

    sspace::sspace_t *tmp_space = c1.get_search_space ();
    u_int dims = c1.get_dims();
    assert (dims == 7u);

    for (u_int i = 0; i < dims; i++) {
        sspace::param_t *tmp = tmp_space->at(i);
        assert (tmp->get_type () == pt::uniform);
        sspace::uniform *tmp_uni = static_cast<sspace::uniform *>(tmp);
        assert (tutils::dbleq (tmp_uni->m_lower, 0.));
        assert (tutils::dbleq (tmp_uni->m_upper, 1.));
        assert (tmp_uni->get_name () == std::to_string (i));
    }
    double resc1 = c1.evaluate(c1.get_opt_param ());
    assert (nearly_equal (resc1, c1.get_opt(), 1e-7));

    // court02
    syn::court02 c2;
    double resc2 = c2.evaluate(c2.get_opt_param ());
    assert (nearly_equal (resc2, c2.get_opt(), 1e-8));

    // court03
    syn::court03 c3;
    double resc3 = c3.evaluate(c3.get_opt_param ());
    assert (nearly_equal (resc3, c3.get_opt(), 1e-7));

    // court04
    syn::court04 c4;
    double resc4 = c4.evaluate(c4.get_opt_param ());
    assert (nearly_equal (resc4, c4.get_opt(), 1e-7));

    // court05
    syn::court05 c5;
    double resc5 = c5.evaluate(c5.get_opt_param ());
    assert (nearly_equal (resc5, c5.get_opt(), 1e-5));

    // court06
    syn::court06 c6;
    double resc6 = c6.evaluate(c6.get_opt_param ());
    assert (nearly_equal (resc6, c6.get_opt(), 1e-5));

    // court07
    syn::court07 c7;
    double resc7 = c7.evaluate(c7.get_opt_param ());
    assert (nearly_equal (resc7, c7.get_opt(), 5e-1));

    // court08
    syn::court08 c8;
    double resc8 = c8.evaluate(c8.get_opt_param ());
    assert (nearly_equal (resc8, c8.get_opt(), 5e-1));

    // court09
    syn::court09 c9;
    double resc9 = c9.evaluate(c9.get_opt_param ());
    assert (nearly_equal (resc9, c9.get_opt(),1e-7));

    // court10
    syn::court10 c10;
    double resc10 = c10.evaluate(c10.get_opt_param ());
    assert (nearly_equal (resc10, c10.get_opt(),1e-7));

    // court11
    syn::court11 c11;
    double resc11 = c11.evaluate(c11.get_opt_param ());
    assert (nearly_equal (resc11, c11.get_opt(),2));

    // court13
    syn::court13 c13;
    double resc13 = c13.evaluate(c13.get_opt_param ());
    assert (nearly_equal (resc13, c13.get_opt()));

    // court14
    syn::court14 c14;
    double resc14 = c14.evaluate(c14.get_opt_param ());
    assert (nearly_equal (resc14, c14.get_opt()));

    // court15
    syn::court15 c15;
    double resc15 = c15.evaluate(c15.get_opt_param ());
    assert (nearly_equal (resc15, c15.get_opt()));

    // court16
    syn::court16 c16;
    double resc16 = c16.evaluate(c16.get_opt_param ());
    assert (nearly_equal (resc16, c16.get_opt(),1e-9));

    // court17
    syn::court17 c17;
    double resc17 = c17.evaluate(c17.get_opt_param ());
    assert (nearly_equal (resc17, c17.get_opt(), 1)); // TODO pls fix precision

    // court18
    syn::court18 c18;
    double resc18 = c18.evaluate(c18.get_opt_param ());
    assert (nearly_equal (resc18, c18.get_opt(),1e-7));

    // court19
    syn::court19 c19;
    double resc19 = c19.evaluate(c19.get_opt_param ());
    assert (nearly_equal (resc19, c19.get_opt(), 1e-4));

    // court20
    syn::court20 c20;
    double resc20 = c20.evaluate(c20.get_opt_param ());
    assert (nearly_equal (resc20, c20.get_opt()));

    // court21
    syn::court21 c21;
    double resc21 = c21.evaluate(c21.get_opt_param ());
    assert (nearly_equal (resc21, c21.get_opt(), 1.e-5));

    // court22
    syn::court22 c22;
    double resc22 = c22.evaluate(c22.get_opt_param ());
    assert (nearly_equal (resc22, c22.get_opt(), 1.e-4));

    // court24
    syn::court24 c24;
    double resc24 = c24.evaluate(c24.get_opt_param ());
    assert (nearly_equal (resc24, c24.get_opt(), 1)); // TODO pls fix precision

    // court25
    syn::court25 c25;
    double resc25 = c25.evaluate(c25.get_opt_param ());
    assert (nearly_equal (resc25, c25.get_opt(), 1e-6));

    // court26
    syn::court26 c26;
    double resc26 = c26.evaluate(c26.get_opt_param ());
    assert (nearly_equal (resc26, c26.get_opt()));

    // court27
    syn::court27 c27;
    double resc27 = c27.evaluate(c27.get_opt_param ());
    assert (nearly_equal (resc27, c27.get_opt(), 1e-6));

    // court28
    syn::court28 c28;
    double resc28 = c28.evaluate(c28.get_opt_param ());
    assert (nearly_equal (resc28, c28.get_opt(), 1e-7));
}

static void
test_unknown_benchmarks ()
{
    // ackley1
    syn::langermann5 lm5;
    assert (lm5.get_name() == std::string ("langermann5"));
    sspace::sspace_t *space = lm5.get_search_space ();
    u_int dims = lm5.get_dims();
    assert (dims == 10u);

    // iterate through all 10 paramteers
    for (uint i = 0; i < lm5.get_dims(); i++) {
        sspace::param_t *tmp = space->at(i);
        assert (tmp->get_type () == pt::uniform);
        sspace::uniform *tmp_uni = static_cast<sspace::uniform *>(tmp);
        assert (tutils::dbleq (tmp_uni->m_lower, 0));
        assert (tutils::dbleq (tmp_uni->m_upper, 10));
        assert (tmp_uni->get_name () == std::to_string (i));
    }

    gridsearch gs = gridsearch ();
    // ridiculously course grid just for testing...
    sspace::sspace_t *lmss = lm5.get_gridsearch_ss(5);
    gs.update_search_space(lmss);
    lm5.free_ss (lmss); delete lmss;
    uint idx = 0;
    inst::set params = gs.generate_parameters(idx++);
    double res = lm5.evaluate (params);
    while (params != NULL) {
        res = lm5.evaluate (params);
        gs.receive_trial_results(idx-1, params, res);
        params = gs.generate_parameters (idx++);
    }

}

void
run_benchmark_tests()
{
    test_synthetic_benchmarks ();
    test_regression_benchmarks ();
    test_unknown_benchmarks ();
    std::cout << "All gridsearch tests pass" << std::endl;
}
