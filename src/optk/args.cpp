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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "args.hpp"
#include <argp.h>
#include <bits/types/error_t.h>

/* Constants ---------------------------------------------------------------- */

// arguments structure
arguments args;

const char *argp_program_version = "OPTK v0.0.1";
const char *argp_program_bug_address = "<maxime.robeyns.2018@bristol.ac.uk>";

static char doc[] =
    "OPTK a benchmarking toolkit for optimisation algorithms.";

static char args_doc[] = "";

// Parameters accepted:
static struct argp_option options[] = {
{"output", 'o', "OUTDIR", 0, "Store the benchmark results in this directory", 0},
{"benchmark", 'b', "BENCHMARKS", 0, "Only run the specified <benchmark>", 0},
{"threads", 't', "THREADS", 0, "Number of threads to use", 0},
{ 0 }
};

/* Argument Parser ---------------------------------------------------------- */

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    arguments *arguments = static_cast<::arguments *>(state->input);

    switch (key) {
        case 'o':
            arguments->output = arg;
            break;
        case 'b':
            // append the value here to the list of strings.
            arguments->benchmarks.append(arg);
            break;
        case 't':
            arguments->threads = atoi(arg);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = { NULL, NULL, NULL, doc };


context_t *init_ctx (int argc, char **argv) {

    // first initialise the arguments structure and allocate memory for strings.

    context_t ctx
    context_t *ctx = (context_t *) malloc (sizeof (context_t));
    arguments *args =
}
