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

/* Argument Parsing --------------------------------------------------------- */

const char *argp_program_version = "OPTK v0.1.2";
const char *argp_program_bug_address = "<maximerobeyns@gmail.com>";

static char doc[] =
    "OPTK is a benchmarking toolkit for optimisation algorithms.";

static char args_doc[] = "";

// parameter description
static struct argp_option options[] = {
    { "output",    'o', "OUTDIR",     0,
        "Store the benchmark results in this directory",        0 },

    { "benchmark", 'b', "BENCHMARKS", 0,
        "Only run the specified <benchmark>",                   0 },

    { "threads",   't', "THREADS",    0,
        "The number of threads to use",                         0 },

    { "iterations",   'i', "ITERATIONS",    0,
        "The maximum number of iterations for each benchmark.", 0 },

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
        case 'i':
            arguments->max_iters = atoi(arg);
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

static bool
validate_args (
        optk::arguments *args,
        optk::optimisers *opts,
        optk::benchmarks *bmks
) {
    bool error = false;

    if (args->threads <= 0) {
        std::cerr <<
            "Error: number of threads must be strictly positive" << std::endl;
        error = true;
    }

    if (args->max_iters <= 0) {
        std::cerr <<
            "Error: number of iterations must be strictly positive" << std::endl;
        error = true;
    }

    // TODO validate output file directory

    return error;
}

/**
 * In this setup function we 'register' all the optimisation algorithms, as
 * well as the bechmarks.
 */
static optk::ctx_t *
do_setup (
        optk::arguments *args,
        optk::optimisers *opts,
        optk::benchmarks *bmks
) {
    // Program context
    optk::ctx_t *ctx = new optk::ctx_t;

    // initialise the relevant benchmarks
    if (std::string(args->benchmark) == "synthetic") {
        syn::synthetic_benchmark *sbm = new syn::synthetic_benchmark;
        bmks->register_benchmark (sbm);
    }

    // no matching benchmarks were added
    if (!bmks->collection()->size()) {
        ctx->error = true;
        std::cerr << "Error: benchmark '" << args->benchmark <<
            "' did not match any known benchmarks." << std::endl;
        return ctx;
    }

    // initialise only the relevant optimisers
    if (std::string(args->algorithm) == "gridsearch") {
        gridsearch *gs = new gridsearch();
        opts->register_optimiser(gs);
    }
    if (std::string(args->algorithm) == "random_search") {
        random_search *rs = new random_search();
        opts->register_optimiser(rs);
    }

    // no matching optimisation algorithms were added
    if (!opts->collection()->size()) {
        ctx->error = true;
        std::cerr << "Error: optimiser '" << args->algorithm <<
            "' did not match any known optimisers." << std::endl;
        return ctx;
    }

    // must be called after initialising opts and bmks
    ctx->error = validate_args (args, opts, bmks);
    if (ctx->error)
        return ctx;

    // other arguments:
    ctx->threads = args->threads;
    ctx->max_iters = args->max_iters;

    ctx->outfile =
        std::string(args->output) + "/" + std::string(args->benchmark) +
        "-" + std::to_string(std::time(0)) + ".csv";

    // Write the header of the csv output file.
    std::ofstream f;
    f.open(ctx->outfile);
    f << "Benchmark, Optimiser";
    for (int i = 0; i < args->max_iters; i++) {
        f << "," << std::to_string(i);
    }
    f << std::endl;
    f.close();

    return ctx;
}

/**
 * In this teardown function we free all memory allocated by the setup and
 * during the program.
 */
static void
do_teardown (optk::ctx_t *ctx)
{
    delete ctx;
}

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


#ifdef __OPTK_TESTING
    OPTKtest::testmain();
    return 0;
#else

    optk::arguments args{
        .threads = 1,
        .max_iters = 20000,
        .output = "outputs",
        .benchmark = "synthetic",
        .algorithm = "gridsearch"
    };

    argp_parse (&argp, argc, argv, 0, 0, &args);

    optk::ctx_t *ctx = do_setup (&args, &opts, &bmks);

    if (ctx->error) {
        do_teardown(ctx);
        return 0;
    }

    std::vector<optk::benchmark_set *> * bms = bmks.collection();
    std::vector<optk::benchmark_set *>::iterator it;
    for (it = bms->begin (); it != bms->end(); it++)
        (*it)->run(&opts, ctx);

    do_teardown (ctx);

    return 0;
#endif
}

