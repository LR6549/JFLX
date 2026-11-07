//
// Binomial standard deviation ("Binomische Standardabweichung").
//
// Public interface of the bsa component (src/bsa.cpp): µ, sigma, sigma
// intervals, and hit chances of a Bernoulli chain.
//
// Note: the source ships two sigma variants that originally shared one name.
// They are exposed as calculateSigma()/calculateSigmaIntervall() (given n & p)
// and calculateSigmaWithMean()/calculateSigmaIntervallWithMean() (given µ).
//

#ifndef JFLX_BSA_HPP
#define JFLX_BSA_HPP

#include <vector>

namespace bsa {

    /**
     * @brief µ of a Bernoulli chain.
     * @param n Length of the chain.
     * @param p Chance of a single Bernoulli experiment.
     */
    double calculateµ(double n, double p);

    /**
     * @brief Sigma with unknown µ, calculated from n and p.
     */
    double calculateSigma(double n, double p);

    /**
     * @brief Sigma with known µ.
     * @param p Chance of a single Bernoulli experiment.
     * @param µ Expected number of hits.
     */
    double calculateSigmaWithMean(double p, double µ);

    /**
     * @brief k-th sigma interval around µ, calculated from n and p.
     *        k = 1 ≈ 68 %, k = 2 ≈ 95 %, k = 3 ≈ 98 %.
     * @return {µ - k·sigma, µ + k·sigma}
     */
    std::vector<double> calculateSigmaIntervall(double n, double p, int k);

    /**
     * @brief k-th sigma interval around a known µ.
     * @return {µ - k·sigma, µ + k·sigma}
     */
    std::vector<double> calculateSigmaIntervallWithMean(double µ, double sigma, int k);

    /// Factorial of @p n (as double).
    double fakültät(double n);

    /// Binomial coefficient n over k.
    double nOverK(double n, double k);

    /// Hit chance of exactly @p k hits in @p n experiments.
    double calculateHitChance(double n, double p, double k);

    /// Sum of the hit chances over the interval [min, max).
    double calculateHitChanceForInterval(double n, double p, double k, double min, double max);

}

#endif // JFLX_BSA_HPP
