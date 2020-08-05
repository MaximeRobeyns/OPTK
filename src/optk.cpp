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

#ifdef __OPTK_TESTING
#include <tests/tests.hpp>
#endif

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

#ifdef __OPTK_TESTING
    OPTKtest::testmain();
    return 0;
#else

    std::cout << "OPTK" << std::endl;

    return 0;
#endif
}




