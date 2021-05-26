#pragma once
#include "cocos2d.h"

namespace lstg
{
	class Random
	{
	public:
		enum class GeneratorType
		{
			well512,
			minstd_rand0, minstd_rand,
			mt19937, mt19937_64,
			ranlux24_base, ranlux48_base,
			ranlux24, ranlux48,
			knuth_b
		};

		// static Random* create();
		// static Random* create(
		// 	uint32_t seed, GeneratorType type = GeneratorType::well512);
		static uint32_t getDeviceSeed();

		void setSeed(uint32_t seed);
		uint32_t getSeed();

		/** get random int32 in range [a, b] */
		int32_t randInt(int32_t a, int32_t b);
		/** get random float in range [a, b] */
		float randFloat(float a, float b);
		/** get random 1 or -1 */
		int32_t randSign();

			//-------------------- integer methods  -------------------

			/** Choose a random item from range(0, start).
			 * It does not include the endpoint.
			 * If range is empty, it returns start. */
			int64_t range(uint64_t start);
			/** Choose a random item from range(start, stop[, step]).
			 * It does not include the endpoint.
			 * If range is empty, it returns start. */
			int64_t range(int64_t start, int64_t stop, int64_t step = 1);
			/** Return a random int in the range [0, n).
			 * Return 0 if n is 0. */
			uint64_t below(uint64_t n);

			//-------------------- real-valued distributions  -------------------

			/** Uniform distribution.
			 * Get a random number in the range [a, b]. */
			double uniform(double a, double b);
			/** Triangular distribution.
			 * Continuous distribution bounded by given lower and upper limits,
			 * and having a given mode value in-between.
			 * http://en.wikipedia.org/wiki/Triangular_distribution */
			double triangular();
			double triangular(double low, double high);
			double triangular(double low, double high, double mode);
			/** Normal distribution.
			 * mu is the mean, and sigma is the standard deviation.*/
			double normal(double mu, double sigma);
			/** Log normal distribution.
			 * If you take the natural logarithm of this distribution, you'll get a
			 * normal distribution with mean mu and standard deviation sigma.
			 * mu can have any value, and sigma must be greater than zero.*/
			double log_normal(double mu, double sigma);
			/** Exponential distribution.
			 * lambda is 1.0 divided by the desired mean. It should be
			 * nonzero. Returned values range from 0 to
			 * positive infinity if lambda is positive, and from negative
			 * infinity to 0 if lambda is negative.*/
			double expo(double lambda);
			/** von Mises distribution (Circular data distribution).
			 * mu is the mean angle, expressed in radians between 0 and 2*pi, and
			 * kappa is the concentration parameter, which must be greater than or
			 * equal to zero. If kappa is equal to zero, this distribution reduces
			 * to a uniform random angle over the range 0 to 2*pi.*/
			double vonMises(double mu, double kappa);
			/** Gamma distribution. (Not the gamma function)
			 * Conditions on the parameters are alpha > 0 and beta > 0.*/
			double gamma(double alpha, double beta);
			/** Gaussian distribution.
			 * mu is the mean, and sigma is the standard deviation. This is
			 * slightly faster than the normal() function.*/
			double gauss(double mu, double sigma);
			/** Beta distribution.
			 * Conditions on the parameters are alpha > 0 and beta > 0.
			 * Returned values range between 0 and 1.*/
			double beta(double alpha, double beta);
			/** Pareto distribution.
			 * alpha is the shape parameter.*/
			double pareto(double alpha);
			/** Weibull distribution.
			 * alpha is the scale parameter and beta is the shape parameter.*/
			double weibull(double alpha, double beta);


		int64_t uniform_int(int64_t a, int64_t b);
		/** produces bool values on a Bernoulli distribution. */
		bool bernoulli(double p);
		/** produces integer values on a binomial distribution. */
		int32_t binomial(int32_t t, double p);
		/** produces integer values on a negative binomial distribution. */
		int32_t negative_binomial(int32_t k, double p);
		/** produces integer values on a geometric distribution. */
		int32_t geometric(double p);

		/** produces integer values on a poisson distribution. */
		int32_t poisson(double mean);
		/** produces real values on an extreme value distribution. */
		double extreme_value(double a, double b);

		/** produces real values on a chi-squared distribution. */
		double chi_squared(double n);
		/** produces real values on a Cauchy distribution. */
		double cauchy(double a, double b);
		/** produces real values on a Fisher's F-distribution. */
		double fisher_f(double m, double n);
		/** produces real values on a Student's t-distribution. */
		double student_t(double n);

		/** produces random integers on a discrete distribution. */
		int32_t discrete(const std::vector<double>& weights);
		/** produces real values distributed on constant subintervals. */
		double piecewise_constant(
			const std::vector<double>& sequence,
			const std::vector<double>& weights);
		/** produces real values distributed on defined subintervals. */
		double piecewise_linear(
			const std::vector<double>& sequence,
			const std::vector<double>& weights);


		Random(uint32_t seed = 0, GeneratorType type = GeneratorType::well512);

	};
}
