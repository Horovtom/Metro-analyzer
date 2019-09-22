//
// Created by lactosis on 22.9.19.
//

#ifndef METRO_SPECPERMUTATIONS_H
#define METRO_SPECPERMUTATIONS_H

#include <cstdlib>
#include <iostream>

namespace SpecPermutations {
    static long double memory[61][5];

    static long double getSpec(int n, int k) {
        if (n <= 0 || k <= 0) return 0;
        if (n > 60 || k > 4) exit(200);
        if (n == k) return 1;
        if (k == 2) return (n * (n - 1)) / 2;
        if (k == 1) return n;
        if (n < k) return 0;
        if (memory[n][k] != 0) return memory[n][k];
        long double result = 0;
        for (int i = n - 1; i > 0; --i) {
            result += getSpec(i, k - 1);
        }
        memory[n][k] = result;
//        std::cout << "n:"<<n << ", k:" << k << " calculated: " << result << std::endl;
        return result;
    }
}

#endif //METRO_SPECPERMUTATIONS_H
