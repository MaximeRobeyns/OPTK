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

#include <stdio.h>
#include "args.hpp"

/* Main function ------------------------------------------------------------ */

int
main (int argc, char **argv)
{
    context_t *ctx = init_ctx (argc, argv);

    free_ctx ();

    return (0);
}

