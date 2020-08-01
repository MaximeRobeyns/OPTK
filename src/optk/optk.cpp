/*
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
 */

#include "iostream"
#include <optk/types.hpp>
#include <optk/benchmark.hpp>
#include <optk/optimiser.hpp>

/* Benchmark list ----------------------------------------------------------- */

class benchmarks {
    public:
        benchmarks() {
            iterator = 0;
        }

        void register_bench(benchmark *b) {
            m_arr.push_back(b);
        }

        benchmark *get_next() {
            if (iterator < m_arr.size())
                return m_arr[iterator++];
            else
                return NULL;
        }

    private:
        int iterator;
        std::vector<benchmark *> m_arr;
};

class optimisers {
    public:
        optimisers() {
            iterator = 0;
        }

        void register_opt(optimiser *o) {
            m_arr.push_back(o);
        }

    private:
        int iterator;
        std::vector<optimiser *>m_arr;
};


/* Main function ------------------------------------------------------------ */

int
main (int argc, char **argv)
{

    // 1. initialise the list of benchmarks
    benchmarks bs;

    ackley ack;
    bs.register_bench(&ack);
    bs.register_bench(&ack);

    // example iterator
    benchmark *b = bs.get_next();
    for ( ; b != NULL; b = bs.get_next()) {
        std::cout << (b)->get_name() << std::endl;
    }
    std::cout << '\n';
        
    // 2. initialise the list of optimisation algorithms

    optimisers opts;

    // 3. iterate through each of the algorithms, and each of the benchmarks in turn
    // 4. initialise the optimisation algorithm
    // 5. query the benchmark for its search space; and send to opt
    // 6. request parameters from opt
    // 7. evaluate benchmark
    // 8. record results and send to opt
    // 9. if < budget; GOTO 6; else continue
    // 10. repeat for other algorithms
    // 11. calculate statistics on results

    // TODO update the benchmark base class to accept constructors like this:
    /*
    benchmark b("testing") : dim(5);

    std::cout << b.get_name() << std::endl;

    double points[] = {42.0, 12.5, 23.5};

    vecd_t x(points, points + sizeof(points) / sizeof(double));

    vecd_t result = b.evaluate(x);

    for (vecd_t::iterator i = result.begin(); i != result.end(); i++)
        std::cout << ' ' << *i;
    std::cout << '\n';
    */

    /* need a way of encoding the search space as an integer vector, which is
     * expressive enough to convey the NNI interface.
     * 
     * Implicitly, the pamgo program treats
     * */


    /*
    for (std::vector<int>::iterator i = ints.begin(); i != ints.end(); i++)
        std::cout << ' ' << *i;
    std::cout << '\n';
    */


    return (0);
}

