#ifndef __OPTIMISER_H_
#define __OPTIMISER_H_

#include <optk/types.hpp>

class optimiser {
    public:
        std::string m_name;

        // function prototypes

        void update_search_space (search_space space);
};

#endif // __OPTIMISER_H_
