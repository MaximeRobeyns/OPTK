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

#include <optk/types.hpp>

std::string Param::get_name() {
    return name;
}

class uniform: Param {
    double lb, ub;

    uniform(std::string n, double lower = 0, double upper = 1) {
        name = n;
        lb = lower; ub = upper;
    }

    double getLb() {
        return lb;
    }

    double getUb() {
        return ub;
    }
};

class randint: Param {
    double lb, ub;

    public:
        randint (std::string n, double lower = 0, double upper = 0) {
            name = n;
            lb = lower; ub = upper;
        }

    double sample() {
        double f = (double)rand() / RAND_MAX;
        return lb + f * (ub - lb);
    }
};

