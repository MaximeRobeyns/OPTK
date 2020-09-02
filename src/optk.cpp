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
 * @brief The main entrypoint to the application
 *
 * @mainpage
 *
 * Welcome to the OPTK documentation!
 *
 * This is intended to be a reference for developers who want to extend or
 * maintain this project.
 *
 * @todo Write a more comprehensive introduction for the main page.
 */

#include <optk/optk.hpp>
#include <optk/optimiser.hpp>
#include <optk/benchmark.hpp>

// optimisers
#include <optimisers/gridsearch.hpp>
#include <optimisers/random.hpp>

// benchmarks
#include <benchmarks/synthetic.hpp>

#ifdef __OPTK_TESTING
#include <tests/tests.hpp>
#endif

/* Argument Parsing --------------------------------------------------------- */

// optk::arguments args;

const char *argp_program_version = "OPTK v0.1.2";
const char *argp_program_bug_address = "<maximerobeyns@gmail.com>";

static char doc[] =
    "OPTK is a benchmarking toolkit for optimisation algorithms.";

static char args_doc[] = "";

// parameter description
static struct argp_option options[] = {
    { "output",    'o', "OUTDIR",     0,
        "Store the benchmark results in this directory", 0 },

    { "benchmark", 'b', "BENCHMARKS", 0,
        "Only run the specified <benchmark>",            0 },

    { "threads",   't', "THREADS",    0,
        "The number of threads to use",                  0 },

    { 0 }
};

// Argument parser
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    optk::arguments *arguments = static_cast<optk::arguments *>(state->input);

    switch (key) {
        case 'o':
            // TODO update to append the string to a list to support multiple
            // optimisers
            arguments->output = arg;
            break;
        case 'b':
            // TODO update to append the string to a list to support multiple
            // benchmarks
            arguments->benchmark = arg;
            break;
        case 't':
            arguments->threads = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            arguments->algorithm = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
            break;
    }
    return 0;
}

struct argp argp = { options, parse_opt, args_doc, doc };

/* Setup and teardown ------------------------------------------------------- */

/**
 * In this setup function we 'register' all the optimisation algorithms, as
 * well as the bechmarks.
 */
static void
do_setup (optk::optimisers *opts, optk::benchmarks *bmks)
{
    gridsearch *gs = new gridsearch();
    opts->register_optimiser(gs);
    random_search *rs = new random_search();
    opts->register_optimiser(rs);

    syn::synthetic_benchmark *syn = new syn::synthetic_benchmark();
    bmks->register_benchmark (syn);
}

/**
 * In this teardown function we free all memory allocated by the setup and
 * during the program.
 * TODO remove if unnecessary...
 */
static void
do_teardown (optk::optimisers *opts, optk::benchmarks *bmks)
{ }

/* Main --------------------------------------------------------------------- */

/**
 * The main function.
 *
 * For now we don't use the parameters, however in the future we shall have to
 * parse arguments specifying the optimisation algorithms to benchmark, as well
 * as the benchmark tasks to run.
 *
 * @param argc The number of arguments
 * @param argv The argument vector
 */
int
main (int argc, char **argv)
{
    optk::optimisers opts;
    optk::benchmarks bmks;

    do_setup (&opts, &bmks);

#ifdef __OPTK_TESTING
    OPTKtest::testmain();
    return 0;
#else

    do_teardown ();

    optk::arguments args{
        .threads = 1,
        .output = "",
        .benchmark = "synthetic",
        .algorithm = "random_search"
    };

    argp_parse (&argp, argc, argv, 0, 0, &args);

    if (args.threads <= 0) {
        std::cerr <<
            "Error: number of threads must be strictly positive" << std::endl;
        return 1;
    }
    // ensure that the algorithm name is valid, and that the optimisation
    // algorithm selection is also valid.

    // print dummy output
    std::cout << "output: " << args.output << std::endl;
    std::cout << "benchmarks: " << args.benchmark << std::endl;
    std::cout << "optimisers: " << args.algorithm << std::endl;
    std::cout << "threads: " << args.threads << std::endl;

    // first parse arguments, including the name of the optimiser to use
    std::cout << "OPTK" << std::endl;

    return 0;
#endif
}

