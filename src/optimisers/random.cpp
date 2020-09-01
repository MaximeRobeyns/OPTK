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
 * @brief This file implements a random search algorithm.
 */

#include <optimisers/random.hpp>

random_search::random_search ():
    optk::optimiser ("random search optimiser")
{ }

void
random_search::update_search_space (sspace::sspace_t *space)
{
    m_space = space;
}

#define get_val(type, ctype) \
    { \
    type *tmp = static_cast<type *>(p); \
    parent->add_item ( \
            new ctype (p->get_name(), tmp->sample()) \
            ); \
    break; \
    }


/**
 * Samples a single value from a parameter which returns a double, and adds it
 * to the parent parameter instance.
 * @param parent The 'level' of the searh space instance to which to add the
 * sampled value
 * @param p The parameter description.
 */
static void
sample_double (inst::node *parent, sspace::param_t *p)
{
    pt t = p->get_type();
    switch (t) {
        case pt::categorical_dbl:
            get_val(sspace::categorical<double>, inst::dbl_val)
        case pt::normal:
            get_val(sspace::normal, inst::dbl_val)
        case pt::qnormal:
            get_val(sspace::qnormal, inst::dbl_val)
        case pt::lognormal:
            get_val(sspace::lognormal, inst::dbl_val)
        case pt::qlognormal:
            get_val(sspace::qlognormal, inst::dbl_val)
        case pt::uniform:
            get_val(sspace::uniform, inst::dbl_val)
        case pt::quniform:
            get_val(sspace::quniform, inst::dbl_val)
        case pt::loguniform:
            get_val(sspace::loguniform, inst::dbl_val)
        case pt::qloguniform:
            get_val(sspace::qloguniform, inst::dbl_val)
        default:
            // won't happen
            break;
    }
}

/**
 * As above, samples a single value from a parameter returning an integer.
 * @param parent The 'level' of the searh space instance to which to add the
 * sampled value
 * @param p The parameter description.
 */
static void
sample_int (inst::node *parent, sspace::param_t *p)
{
    pt t = p->get_type();
    switch (t) {
        case pt::categorical_int:
            get_val(sspace::categorical<int>, inst::int_val)
        case pt::randint:
            get_val(sspace::randint, inst::int_val)
        default:
            break;
    }
}

void
random_search::sample_ss (inst::node *parent)
{
    std::vector<sspace::param_t *>::iterator it;
    for (it = m_space->begin (); it != m_space->end (); it++) {

        pt t = (*it)->get_type();

        switch (t) {
            case pt::categorical_dbl:
            case pt::normal:
            case pt::qnormal:
            case pt::lognormal:
            case pt::qlognormal:
            case pt::uniform:
            case pt::quniform:
            case pt::loguniform:
            case pt::qloguniform:
            {
                sample_double (parent, *it);
                break;
            }
            case pt::categorical_int:
            case pt::randint:
            {
                sample_int (parent, *it);
                break;
            }
            case pt::categorical_str:
            {
                sspace::categorical<std::string> *tmp =
                    static_cast<sspace::categorical<std::string> *>(*it);
                parent->add_item (
                        new inst::str_val ((*it)->get_name(), tmp->sample())
                        );
                break;
            }
            case pt::choice:
            {
                inst::node *ss = new inst::node ((*it)->get_name());
                this->sample_ss (ss);
                parent->add_item (ss);
                break;
            }
        }
    }
}

inst::set
random_search::generate_parameters (int param_id)
{
    inst::node *root = new inst::node ("random parameters");

    sample_ss (root);

    add_to_trials (param_id, root);

    return root;
}

void
random_search::receive_trial_results (int pid, inst::set params, double value)
{
    free_node (params);
    trials.erase(pid);
    return;
}
