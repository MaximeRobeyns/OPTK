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

#ifndef __TYPES_H_
#define __TYPES_H_

#include <sys/types.h>
#include <vector>
#include <string>

typedef std::vector<double> vecd_t;

/* Parameter types ---------------------------------------------------------- */

namespace optk {
    typedef enum {
        choice,
        randint,
        uniform,
        quniform,
        loguniform,
        qloguniform,
        normal,
        qnormal,
        lognormal,
        qlognormal,
    } param_type;
};

/* This is the base parameter class which all other parameters in the search
 * space implement. */
class param_t {
    public:
        // virtual double sample() = 0;
        optk::param_type get_type() { return m_type; }
        std::string get_name() { return m_name; }

    private:
        optk::param_type m_type;
        std::string m_name;
};

// T should be either double, integer or strings
template <typename T>
class choice: param_t {
    public:
        choice(std::string n, std::vector<T> choice);
    private:
        std::vector<T> m_choice;
        optk::param_type m_type = optk::choice;
};

class randint: param_t {
    public:
        randint(std::string n, double lower, double upper);
    private:
        optk::param_type m_type = optk::randint;
};

class uniform: param_t {
    public:
        uniform(std::string n, double low, double high);
    private:
        optk::param_type m_type = optk::uniform;
};

class quniform: param_t {
    public:
        quniform(std::string n, double low, double high, int q);
    private:
        optk::param_type m_type = optk::quniform;
};

class loguniform: param_t {
    public:
        loguniform(std::string n, double low, double high);
    private:
        optk::param_type m_type = optk::loguniform;
};

class qloguniform: param_t {
    public:
        qloguniform(std::string n, double low, double high, double q);
    private:
        optk::param_type m_type = optk::qloguniform;
};

class normal: param_t {
    public:
        normal(std::string n, double mu, double sigma);
    private:
        optk::param_type m_type = optk::normal;
};

class qnormal: param_t {
    public:
        qnormal(std::string n, double mu, double sigma, double q);
    private:
        optk::param_type m_type = optk::qnormal;
};

class lognormal: param_t {
    public:
        lognormal(std::string n, double mu, double sigma);
    private:
        optk::param_type m_type = optk::lognormal;
};

class qlognormal: param_t {
    public:
        qlognormal(std::string n, double mu, double sigma, double q);
    private:
        optk::param_type m_type = optk::qlognormal;
};

typedef std::vector<param_t> search_space;

#endif // __TYPES_H_
