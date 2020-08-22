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

static void
test_synthetic_benchmarks ()
{
    syn::ackley1 a1(5);

    assert (a1.get_name() == std::string("ackley1"));
    sspace::sspace_t *tmp_space = a1.get_search_space();
    u_int dims = a1.get_dims();

    // iterate throguh all 5 parameters.
    for (u_int i = 0; i < dims; i++) {
        sspace::param_t *tmp = tmp_space->at(i);
        assert (tmp->get_type() == pt::uniform);
        sspace::uniform *tmp_uni = static_cast<sspace::uniform *>(tmp);
        assert (dbleq (tmp_uni->m_lower, -35));
        assert (dbleq (tmp_uni->m_upper, 35));
        assert (tmp_uni->get_name() == std::to_string(i));
    }
    // todo evaluate at optimal parameters and ensure correct
}

void
run_benchmark_tests()
{
    test_synthetic_benchmarks();
    std::cout << "All gridsearch tests pass" << std::endl;
}
