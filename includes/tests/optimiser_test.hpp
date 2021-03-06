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
 * @brief Defines some tests for built-in optimisers.
 */

#ifndef __OPTIMISER_TEST_H_
#define __OPTIMISER_TEST_H_

#include <assert.h>
#include <iostream>
#include <cmath>
#include <limits>

#include <optk/types.hpp>

extern void run_static_gridsearch_tests ();

void run_gridsearch_tests ();
void run_random_search_tests ();

#endif // __OPTIMISER_TEST_H_

