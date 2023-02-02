/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/
#include "parser.h"
#include "perfect_power.h"
#include "helper.h"
#include <iostream>
#include <limits.h>
using namespace std;

/*  Method used for checking if given number x is a perfect power
|   - We use binary search to find out if x can be written as x = <a> ^ <base>
|   - The exponent <base> is checked linear from 1 to max_exponent
|     that is the number of reducers
|   - We do binary search for the <a> in range 1 : x
|   -
*/
void check_perfect_power(int x, int max_power,
                        vector<vector<vector<int>>> *perfect_powers,
                        int thread_id)
{
    for(int base = 2; base <= max_power; base ++) {
        int st = 1;
        int dr = x;

        while(st <= dr) {
            int mijl = (st + dr)/2;

            long long number = 1;
            for(int pow = 1; pow <= base; pow++) {
                number *= mijl;
                if(number > INT_MAX) {
                    break;
                }
            }

            if(number == x) {
                (*perfect_powers)[thread_id][base].push_back(x);
                break;
            } else if(number > x) {
                dr = mijl - 1;
            } else {
                st = mijl + 1;
            }

        }
    }

}
