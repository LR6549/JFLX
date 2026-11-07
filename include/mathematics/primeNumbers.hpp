#ifndef PrimeNumbersHPP
#define PrimeNumbersHPP

#include <stdexcept>
#include <cstdlib>
#include <climits>
#include <vector>
#include <limits>
#include <ctime>
#include <cmath>
#include <set>

#include <iostream>

using namespace std;

/*
* Some Calculations for prime numbers
*/
namespace PrimeNumbers {
    /*
    * O(sqrt(n))
    * @param[in] pNumber -> The speculated Prime
    * @return[in] boolean -> pNumber is prime
    */
    inline bool isPrime(unsigned long long pNumber) {
        if (pNumber <= 1) {
            return false;
        } else if (pNumber == 2 || pNumber == 3) {
            return true;
        } else if (pNumber % 2 == 0 || pNumber % 3 == 0) {
            return false;
        } else {
            for (int i = 5; i < sqrt(pNumber); i++) {
                if (pNumber % i == 0 || pNumber % (i+2) == 0) {
                    return false;
                }
            }
            return true;
        }
    }

    /*
    * @Returns a Vector<bool> with all numbers till @param[in] maxSize
    * Vector[n] is true -> n is a prime number
    */
    inline vector<bool> generatePrimeVectorBool(unsigned long long minSize = 0, unsigned long long maxSize = ULLONG_MAX) {
        if (minSize >= maxSize) {
            throw out_of_range("MinSize can not be bigger or as big as MaxSize!");
        }

        set<int> s = {1, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 49, 53, 59};

        // Initialize the sieve with enough wheels to include maxSize
        vector<bool> primes(maxSize + 1, false);

        // Put in candidate primes
        for (unsigned long long x = 1; x * x <= maxSize; ++x) {
            for (unsigned long long y = 1; y * y <= maxSize; ++y) {
                unsigned long long n = 4 * x * x + y * y;
                if (n < minSize) {
                    primes[n] = false;
                    continue;
                }
                if (n <= maxSize && (n % 60 == 1 || n % 60 == 13 || n % 60 == 17 || n % 60 == 29 ||
                                n % 60 == 37 || n % 60 == 41 || n % 60 == 49 || n % 60 == 53)) {
                    primes[n] = !primes[n];
                }

                n = 3 * x * x + y * y;
                if (n <= maxSize && (n % 60 == 7 || n % 60 == 19 || n % 60 == 31 || n % 60 == 43)) {
                    primes[n] = !primes[n];
                }

                if (x > y) {
                    n = 3 * x * x - y * y;
                    if (n <= maxSize && (n % 60 == 11 || n % 60 == 23 || n % 60 == 47 || n % 60 == 59)) {
                        primes[n] = !primes[n];
                    }
                }
            }
        }

        // Eliminate composites by sieving
        for (unsigned long long n = minSize; n * n <= maxSize; ++n) {
            if (primes[n]) {
                for (unsigned long long k = n * n; k <= maxSize; k += n * n) {
                    primes[k] = false;
                }
            }
        }

        return primes;
    }

    /*
    * Returns a Vector<unsigned long long> with all prime numbers till maxSize
    * Vector[n] is a prime number
    */
    inline vector<unsigned long long> generatePrimeVectorULL(unsigned long long minSize = 0, unsigned long long maxSize = ULLONG_MAX) {
        if (minSize >= maxSize) {
            throw out_of_range("MinSize can not be bigger or as big as MaxSize!");
        }

        vector<bool> primeBoolVect= generatePrimeVectorBool(minSize, maxSize);
        vector<unsigned long long> primes = {};

        for (unsigned long long i = minSize; i < primeBoolVect.size(); i++) {
            primes.push_back(i);
        }

        return primes;
    }

    /*
    * Fast Calculate Primes from minSize to maxSize:
    * (Displayed in cout << prime << "\n";)
    */
    inline void primeCalculation(unsigned long long minSize = 0, unsigned long long maxSize = ULLONG_MAX) {
        if (minSize >= maxSize) {
            throw out_of_range("MinSize can not be bigger or as big as MaxSize!");
        }

        for (unsigned long long n = minSize; n <= maxSize; ++n) {
            if (isPrime(n)) {
                cout << n << "\n";
            }
        }
    }

    /*
    * Helper Function For randomPrimeNumber(unsigned long long minSize, unsigned long long maxSize) {...}
    */
    inline unsigned long long randomChoice(vector<bool> primes, unsigned long long minSize) {
        vector<unsigned long long> validIndices;
        for (unsigned long long i = minSize; i < primes.size(); ++i) {
            if (primes[i]) {
                validIndices.push_back(i);
            }
        }
        if (validIndices.empty()) {
            throw std::runtime_error("No valid indices to choose from.");
        }

        srand(time(nullptr));

        int randomIndex = rand() % validIndices.size();
        return validIndices[randomIndex];
    }

    /*
    * Returns a random prime number between minSize and maxSize
    */
    inline unsigned long long randomPrimeNumber(unsigned long long minSize, unsigned long long maxSize) {
        if (minSize >= maxSize) {
            throw out_of_range("MinSize can not be bigger or as big as MaxSize!");
        }

        vector<bool> primes = generatePrimeVectorBool(minSize, maxSize);

        return randomChoice(primes, minSize);
    }
}

#endif