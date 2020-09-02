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
 * @brief Defines the main types used in the program.
 *
 * @todo defer the problem of turning categorical parameters of strings and
 * enumerating them to the compatability layer which allows problems defined in
 * python to be run on the optimisers from OPTK. This is the component which
 * will be responsible for mapping numeric values to their non-numeric
 * counterparts which the python program needs to evaluate.
 */

#ifndef __TYPES_H_
#define __TYPES_H_

#include <cstdlib>
#include <cmath>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

#include <sys/types.h>

// Parameter values -----------------------------------------------------------

/** Entries in the inst namespace represent concrete instances or settings of a
 * search spce. */
namespace inst {

/** These allow concrete instnaces of nested search spaces to be represented. */
enum class inst_t : char {
    /** Used to designate the parameter set as containing a list of values    */
    node,
    /** represents an integer value                                           */
    int_val,
    /** represents a double-precision floating point value                    */
    dbl_val,
    /** represents a string value                                             */
    str_val
};

class param;
class node;

/** This is a concrete set of parameter value assignments, used to pass
 * parameter values between optimisers and benchmarks.
 *
 * The optimisation algorithm is responsible for managing the memory allocated
 * to pass these parameters, the benchmark or program simply consumes these
 * values. */
typedef node *set;

/** This is used to succinctly describe a map of key, parameter pairs. */
typedef std::unordered_map <std::string, param *> value_map;

/** This is the format used to pass sets of parameter values between
 * optimisation algorithms and the benchmarks / problems being optimised. */
class param {
    public:
        param (const std::string &k, inst_t t);

        inst_t get_type () { return type; }
        std::string get_key() { return key; }

    private:
        const std::string key;
        const inst_t type;
};

// Could have used a template class for the following three classes, however
// this was not done because it ends up bloating the syntax. Also since only
// integers, doubles and strings are allowed, explicitly defining them gives a
// bit more 'type safety'.

/** An concrete integer value. */
class int_val: public param {
    public:
        int_val (const std::string &k, int v);

        int get_val () { return val; }
        void update_val (int v) { val = v; };
        int *get_addr () { return &val; }

    private:
        int val;
};

/** An concrete double precision floating point value. */
class dbl_val: public param {
    public:
        dbl_val (const std::string &k, double v);

        double get_val () { return val; }
        void update_val (double v) { val = v; };
        double *get_addr () { return &val; }

    private:
        double val;
};

/** An concrete string value. */
class str_val: public param {
    public:
        str_val (const std::string &k, const std::string &v);

        std::string get_val () { return val; }
        void update_val (std::string v) { val = v; };
        std::string *get_addr () { return &val; }

    private:
        std::string val;
};

/**
 * This represents concrete values at one 'level' of the search space. This
 * could include a combination of both parameters and further nodes.
 */
class node : public param {
    public:
        /** The constructor
         * @parm k The key for this node. */
        node (const std::string &k);

        /** Add a single parameter to the list.
         * @param p The parameter to add. */
        void add_item (param *p);

        /** Add a vector of parameter in one go.
         * @param items A reference to the vector of items to add */
        void add_items (std::vector<param *> items);

        /** Returns a copy of the specified parameter.
         * @param k The unique parameter key. */
        param *get_item(const std::string &k);

        /** Removes a parameter from the list; fails silently on error.
         * @param k The unique key of that parameter. */
        void remove_item (const std::string &k);

        /**
         * Return all the values
         * @returns The unordered map of key, parameter pairs
         */
        value_map *get_values ()
        { return &values; }

        int getint(const std::string &key);
        int getint(int i);
        double getdbl(const std::string &key);
        double getdbl(int i);
        std::string getstr(const std::string &key);
        std::string getstr(int i);

    private:
        /** An unordered map of concrete values and nodes at this 'level' of
         * the search space, identified by their key */
        value_map values;
};

/**
 * This is a convenience function to access a member of an inst::node item.
 * The template type must be a \e pointer to one of the enumerated values in
 * inst::inst_t; namely a node, int_val, dbl_val or str_val.
 */
template <class T>
T get (node *n, const std::string &k)
{
    inst::param *tmp = n->get_item(k);
    if (tmp != NULL)
        return static_cast<T>(tmp);
    return NULL;
}

#define GETINT(dest, src, key) \
    inst::int_val *dest = static_cast<inst::int_val *>((src)->get_item (key))

#define GETDBL(dest, src, key) \
    inst::dbl_val *dest = static_cast<inst::dbl_val *>((src)->get_item (key))

#define GETSTR(dest, src, key) \
    inst::str_val *dest = static_cast<inst::str_val *>((src)->get_item (key))

#define GETNODE(dest, src, key) \
    inst::node *dest = static_cast<inst::node *>((src)->get_item (key))

/**
 * This will recursively free a search space instance, with elements allocated
 * on the heap.
 * @param n A pointer to the base node element, which is itself allocated on
 * the heap.
 */
void free_node (node *n);

} // namespace inst end

// Search space types ----------------------------------------------------------

/** pt ('param type') enumerates the types available for defining a parameter
 * space, and is compatible with NNI. */
enum class pt: char {
    /** Represents a choice between a finite list of integer values.
     * See optk::categorical.                                                 */
    categorical_int,
    /** Represents a choice between a finite list of double values.
     * See optk::categorical.                                                 */
    categorical_dbl,
    /** Represents a choice between a finite list of string values.
     * See optk::categorical.                                                 */
    categorical_str,
    /** A choice between parameters; possibly of different types. Allows for
     * nested search spaces. See optk::choice.                                */
    choice,
    /** Random integer sampled within a range.
     * See optk::randint.                                                     */
    randint,
    /** Real-valued parameter sampled uniformly at random.
     * See optk::uniform.                                                     */
    uniform,
    /** Quantised uniform parameter.
     * See optk::quniform.                                                    */
    quniform,
    /** Real-valued parameter drawn from a loguniform distribution.
     * See optk::loguniform.                                                  */
    loguniform,
    /** Quantised log-uniform distribution.
     * See optk::qloguniform.                                                 */
    qloguniform,
    /** Parameter drawn from a Guassian distribution.
     * See optk::normal.                                                      */
    normal,
    /** Quantised normal parameter.
     * See optk::qnormal.                                                     */
    qnormal,
    /** Real-valued parameter drawn from a lognormal distribution.
     * See optk::lognormal.                                                   */
    lognormal,
    /** Quantised log-normal parameter.
     * See optk::qlognormal.                                                  */
    qlognormal
};

namespace sspace {

/** param_t is the base parameter class which all other parameters in the
 * search space implement. */
class param_t
{
    public:
        /**
         * base constructor
         * @param n The name of the parameter
         */
        param_t (std::string n, pt t);

        /** A virtual destructor is required here so that pointers to derived
         * classes can be deleted. */
        virtual ~param_t () {};

        /**
         * Every parameter has a fixed type corresponding to pt::param_type
         * @returns the type of this parameter.
         */
        virtual pt
        get_type()
        {
            return m_type;
        }

        /**
         * All parameters must also be named.
         * @returns this parameter's name
         */
        virtual std::string
        get_name()
        {
            return m_name;
        }

    private:
        const std::string m_name;
        const pt m_type;
};

/** sspace_t represents the search space or parameter space in which the
 * parameters must be optimised. */
typedef std::vector<param_t *> sspace_t;

/**
 * A class to represent the choice between a finite list of static values.
 *
 * Common choices of value types include integers, double-precision floating
 * point numbers, as well as strings. These are all used to model categorical
 * paramteters.
 *
 * This type also serves to terminate the recursion for a choice type (see below).
 */
template <typename T>
class categorical: public param_t
{
    public:
        /**
         * Constructor for integer-valued categorical parameter.
         * @param n the parameter's name
         * @param options an array of doubles (passed by value)
         */
        categorical (std::string n, std::vector<int> *options):
            param_t (n, pt::categorical_int)
        {
            if (options->size() == 0)
                throw std::invalid_argument("Empty value list is invalid");
            m_options = *options;
            init_rand ();
        }

        /**
         * Constructor for double-valued categorical parameter.
         * @param n the parameter's name
         * @param options an array of doubles (passed by value)
         */
        categorical (std::string n, std::vector<double> *options):
            param_t (n, pt::categorical_dbl)
        {
            if (options->size() == 0)
                throw std::invalid_argument("Empty value list is invalid");
            m_options = *options;
            init_rand ();
        }

        /**
         * Constructor for string-valued categorical parameter.
         * @param n the parameter's name
         * @param options an array of doubles (passed by value)
         */
        categorical (std::string n, std::vector<std::string> *options):
            param_t (n, pt::categorical_str)
        {
            if (options->size() == 0)
                throw std::invalid_argument("Empty value list is invalid");
            m_options = *options;
            init_rand ();
        }

        /** values */
        std::vector<T> *
        values()
        {
            return &m_options;
        }

        /** Counts the number of options. */
        long unsigned int
        count ()
        {
            return m_options.size();
        }

        /**
         * Retreives the value of the ith element
         *
         * @param i the index of the option
         * @returns The ith option
         * @exception std::range_error
         */
        T
        get(long unsigned int i)
        {
            if (i < 0 || i > m_options.size())
                throw "Out of range";
            return m_options[i];
        }

        /**
         * Sample a random value uniformly at random from the list of values.
         */
        T
        sample ()
        {
            return get(dist(generator));
        }

    private:

        /**
         * Called from constructors; initialises the random number generator.
         */
        void
        init_rand ()
        {
            generator = std::mt19937 (rd ());
            uint u = m_options.size() - 1;
            if (u < 0) u = 0;
            dist = std::uniform_int_distribution<int> (0, u);
        }

        std::vector<T> m_options;

        // used to implement random sampling
        std::random_device rd;
        std::mt19937 generator;
        std::uniform_int_distribution<int> dist;
};

/**
 * A parameter of type \c choice takes on one of the provided options as its
 * value.
 *
 * This differs from the categorical parameter type above since parameter values
 * can be other parameter types. This allows for nested search spaces to be created.
 *
 * Proper handling of this type by optimisation algorithms requires recursively
 * registering each of the \c options.
 */
class choice: public param_t
{
    public:
        /**
         * The constructor
         * @param n the name of the parameter
         * @param options the values choice may take on.
         */
        choice (std::string n, sspace_t *options);

        /**
         * lists out all the options that this parameter can take on
         * @returns a pointer to the list of options
         */
        sspace_t *options();

        /**
         * Counts the options
         * @returns The number of options for this parameter.
         */
        long unsigned int count ();

        /**
         * Retreive the ith element
         * @param i the index of the element to fetch
         * @returns the ith parameter in the options array.
         * @exception std::range_error on i out of bounds
         */
        param_t *get (long unsigned int i);

    private:
        /** This is what allows us to construct nested search spaces. Memory
         * for the nested space is allocated */
        sspace_t *m_options;
};

/**
 * A \c randint parameter has value chosen at random within the upper and
 * lower bounds.
 *
 * The lower bound is \e inclusive, while the upper bound is \e exclusive.
 */
class randint: public param_t
{
    public:
        /**
         * The constructor
         * @param n the name of this parameter
         * @param lower the lower bound
         * @param upper the upper bound
         */
        randint (std::string n, int lower, int upper);

        /**
         * Sample from the random distribution (exclusive of upper bound).
         * @returns new random integer sample.
         */
        int sample ();

        /**
         * Bounds of the selection range.
         * Encapsulation should be irrelevant here so we save the effort of
         * making getters/setters and just make these members public...
         */
        int m_lower, m_upper;

    private:
        std::random_device rd;
        std::mt19937 generator;
        std::uniform_int_distribution<int> dist;
};

/**
 * A \c uniform parameter is real-valued and sampled uniformly at random between
 * the lower and upper bound (inclusive).
 */
class uniform: public param_t
{
    public:

        /**
         * The constructor to be used when instantiating a uniform parameter
         * manually.
         * @param n the name of this parameter
         * @param lower the lower bound; as a double
         * @param upper the upper bound; as a double
         */
        uniform (std::string n, double lower, double upper);

        /**
         * This constructor is used by classes inheriting from uniform, and
         * uses run-time polymorphism to set the desired type (per the
         * inheriting class).
         *
         * It should (probably) never be called directly.
         */
        uniform (std::string n, double lower, double upper, pt type);

        /**
         * Produces a double-precision value drawn uniformly at random from
         * the given range.
         *
         * This is a virtual method to allow for runtime polymorphism when using
         * derived classes in the choice parameter type.
         *
         * @returns a new random sample
         */
        virtual double sample();

        /** Lower and upper bounds on the uniform distribution. */
        double m_lower, m_upper;

    protected:
        std::random_device rd;
        std::mt19937 generator;
        std::uniform_real_distribution<double> dist;
};

/**
 * quniform is a parameter which has a value between the upper and lower
 * bounds (inclusive), incrementing in steps of size \c q.
 *
 * This is suitable for a discrete-valued parameter, with respect to which
 * the response surface is smooth, but should be bounded above and below.
 *
 * To uniformly choose an \e integer from the range [low, high], select a value
 * of $q = 1$.
 */
class quniform: public uniform {
    public:
        /**
         * The constructor
         * @param n the name of this parameter
         * @param lower the lower bound; as a double
         * @param upper the upper bound; as a double
         * @param q the 'quantisation'; as a double
         */
        quniform (std::string n, double lower, double upper, double q);

        /**
         * Overloads the sample method from uniform such that a value is
         * returned according to
         * \f[
         * \text{clip}\bigg(\text{round}\big(
         * \frac{\text{uniform}(low, high)}{q}\cdot q,
         * low, high \big)\bigg)
         * \f]
         *
         * @returns a double-precision floating point value sampled from a
         * quantised uniform distribution.
         */
        double sample();

        double m_q;
};

/**
 * loguniform parameters are drawn such that they follow a log-uniform
 * (reciprocal) distribution.
 *
 * The variable value is drawn from within the closed range of the upper and
 * lower bound, such that the logarithm of the return value is uniformly
 * distributed.
 */
class loguniform: public uniform {
    public:
        /**
         * The constructor
         * @param n the name of this parameter
         * @param lower the lower bound; must be strictly positive
         * @param upper the upper bound; must be strictly positive
         * @param q the 'quantisation'; as a double
         * @exception invalid_argument on negative or zero-valued bounds
         */
        loguniform (std::string n, double lower, double upper);

        /**
         * Using the same polymorphic constructor pattern as the vanilla
         * uniform type, this should be called by classes inheriting this one.
         */
        loguniform (std::string n, double lower, double upper, pt t);

        /**
         * Overloads the sample method from uniform such that a value is
         * returned according to
         * \f$ v = \exp \bigg(\text{uniform}\big(\log (l),\log(u)\big)\bigg)\f$
         *
         * @returns a double-precision floating point value sampled from a
         * loguniform distribution.
         */
        double sample ();
};

/**
 * qloguniform parameters are like loguniform parameters except that they
 * are clipped above and below, and vary in fixed increments.
 *
 * This is suitable for discrete-valued parameters with respect to which the
 * objective function is smooth, and gets smoother with the size of the
 * value.
 */
class qloguniform: public loguniform {

    public:

        /**
         * The constructor
         * @param n the name of this parameter
         * @param lower the lower bound; must be strictly positive
         * @param upper the upper bound; must be strictly positive
         * @param q the 'quantisation'; as a double
         * @exception invalid_argument on negative or zero-valued bounds
         */
        qloguniform (std::string n, double lower, double upper, double q);

        /**
         * Overloads the sample method from loguniform to quantise it;
         * \f[
         * v = \text{clip}\bigg(
         * \text{round}\big(\text{loguniform}(\text{low},\text{high})
         * \big)\bigg)
         * \f]
         *
         * @returns a double-precision floating point value sampled from a
         * quantised loguniform distribution.
         */
        double sample ();

        double m_q;
};

/**
 * This class is for normally-distributed parameters parametrised by \c mu
 * and \c sigma; this is an unconstrained parameter.
 */
class normal: public param_t {
    public:
        /**
         * The constructor.
         *
         * @param n The name of this parameter
         * @param mu The mean of the normal distribution from which to sample
         * @param variance The mean of the normal distribution.
         */
        normal (std::string n, double mu, double sigma);

        /**
         * The polymorphic constructor which allows for inheriting classes to
         * specify their types. Do not call.
         */
        normal (std::string n, double mu, double sigma, pt t);

        /**
         * Samples a double-precision floating point value from the normal
         * probability distribution:
         * \f[ p(x|\mu\,\sigma) \= \frac{1}{\sigma \sqrt{2 \pi}}
         * e\^{- \frac{{x - \mu}\^ {2}}{2 \sigma \^ {2}} } \f]
         *
         * @returns A single sapmled value.
         */
        virtual double sample ();

        /** The parameters of the underlying normal distribution. */
        double m_mu, m_sigma;

    protected:
        std::random_device rd;
        std::mt19937 generator;
        std::normal_distribution<double> dist;
};

/**
 * qnormal is for discrete-valued variables which probably take a value of mu.
 *
 * These variables are still fundamentally unbounded.
 */
class qnormal: public normal {
    public:
        /**
         * the constructor
         * @param n The name of this parameter
         * @param mu The mean of the normal distribution
         * @param sigma The standard deviation of the normal distribution
         * @param q The quantisation for the sampled values
         */
        qnormal (std::string n, double mu, double sigma, double q);

        /**
         * Sample a value from the quantised normal distribution according to
         * the following:
         * \f[
         * v = \text{round}\big(\text{normal}(\mu, \sigma) / q\big) \cdot q.
         * \f]
         */
        double sample ();

        double m_q;
};

/**
 * A lognormal parameter follows a log-normal distribution. Implicitly it is
 * constrained to take only positive real values.
 */
class lognormal: public normal {
    public:
        /**
         * The constructor
         * @param n The name of this parameter
         * @param n The mean of the normal distribution.
         * @param n The standard deviation of the normal distribution.
         */
        lognormal (std::string n, double mu, double sigma);

        /**
         * The polymorphic constructor. Do not call.
         */
        lognormal (std::string n, double mu, double sigma, pt t);

        /**
         * Draw values from the exponentiated normal distribution;
         *
         * \f[
         * p(x \vert \mu, \sigma ) = \exp \bigg(\mathcal{N}(x; \mu, \sigma)\bigg).
         * \f]
         */
        double sample ();
};

/**
 * qlognormal is a quantised parameter which follows a log-normal distribution.
 *
 * This is suitable for discrete-valued variables with respect to which the
 * objective function becomes smoother as the size of the variable, which
 * is bounded to one size, increases.
 */
class qlognormal: public lognormal {
    public:
        /**
         * The constructor
         * @param n The name of this parameter
         * @param mu The mean of the normal distribution
         * @parma sigma The standard deviation of the normal distribution
         * @param q The quantisation to apply to the output
         */
        qlognormal (std::string n, double mu, double sigma, double q);

        /**
         * Samples values from a quantised exponential normal distribution
         * according to the formula:
         * \f[
         * v = \text{round}\bigg(\text{lognormal}(\mu, \sigma) / q\bigg)\cdot q.
         * \f]
         */
        double sample ();

        double m_q;
};

/**
 * This function takes a set of concrete parameters, and the corresponding
 * description of the parameter space, and ensures that each concrete value is
 * a valid instance of the description.
 * @param vals The set of concrete values; this usually comes from the
 * optimisation algorithm.
 * @param sspace The description of the search space
 * @exception std::invalid_argument This is thrown when a discrepency between
 * an instance of a parameter and its description is detected.
 */
void validate_param_values (inst::value_map *vals, sspace::sspace_t *sspace);

/**
 * A convenience method to delete a search space description which was
 * allocated on the heap. This function is often used in class destructors.
 * @param ss The heap-allocated search space description to free.
 */
void free_ss (sspace_t *ss);

} // namespace sspace

#endif // __TYPES_H_
