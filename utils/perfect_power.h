/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/
#ifndef _PERFECT_POWER_H

#include <unordered_map>
#include <vector>

/*
    Perfect_powers
    |   - Used to describe the logic of checking the perfect powers
*/

void check_perfect_power(int x, 
            int max_power,
            vector<vector<vector<int>>>     *perfect_powers,
            int thread_id);

#endif