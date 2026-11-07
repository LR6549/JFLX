#include <mathematics/bsa.hpp>

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

namespace bsa{
    
    /*
    * Calculates µ with given n and p:
    * n = length of bernouli chain
    * p = chance of bernouli experiment
    */
    double calculateµ(double n, double p) {
        return n*p;
    }

    /*
    * Calculates Sigma with unknown
    * n = length of bernouli chain
    * p = chance of bernouli experiment
    */
    double calculateSigma(double n, double p) {
        return sqrt(calculateµ(n, p)*(1-p));
    }

    /*
    * Calculates Sigma with known µ
    * p = chance of bernouli experiment
    * µ = chance for a hit
    */
    double calculateSigmaWithMean(double p, double µ) {
        return sqrt(µ*(1-p));
    }

    /*
    * Calculates the k'th sigma interval with known µ and sigma
    * If sigma > 3:
    * k = 1 includes ~ 68%
    * k = 2 includes ~ 95%
    * k = 3 includes ~ 98%
    */
    vector<double> calculateSigmaIntervallWithMean(double µ, double sigma, int k) {
        return {
            µ-(k*sigma), 
            µ+(k*sigma)
        };
    }

    /*
    * Calculates the k'th sigma interval with n and p
    * If sigma > 3:
    * k = 1 includes ~ 68%
    * k = 2 includes ~ 95%
    * k = 3 includes ~ 98%
    */
    vector<double> calculateSigmaIntervall(double n, double p, int k) {
        double µ = calculateµ(n, p);
        double sigma = calculateSigmaWithMean(p, µ);
        return {
            µ-(k*sigma), 
            µ+(k*sigma)
        };
    }

    double fakültät(double n) {
        double result = 1;
        for (int i = 2; i < n+1; i++) {
            result *= i;
        }
        return result;
    }

    double nOverK(double n, double k) {
        return (fakültät(n)/(fakültät(k)*fakültät(n-k)));
    }

    double calculateHitChance(double n, double p, double k) {
        return nOverK(n, k)*pow(p, 2)*pow(1-p, 2);
    }

    double calculateHitChanceForInterval(double n, double p, double k, double min, double max) {
        double totalHitChance = 0;

        for (int i = min; i < max; i++) {
            totalHitChance += calculateHitChance(n, p, k);
        }
        return totalHitChance;
    }
}