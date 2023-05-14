#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LEFT 0.0
#define RGHT 1.0

#define NOT_CONVERGING (-1)

double f(double);

double bisection(double x1, double x2, double eps, int limit);

int
main(int argc, char *argv[]) {
	double x;
	x = bisection(LEFT, RGHT, 1e-8, 1000);
	printf("(main) x = %.10f, f(x) = %12.10f\n", x, f(x));
	x = bisection(LEFT, RGHT, 1e-10, 1000);
	printf("(main) x = %.10f, f(x) = %12.10f\n", x, f(x));
	x = bisection(LEFT, RGHT, 1e-12, 1000);
	printf("(main) x = %.10f, f(x) = %12.10f\n", x, f(x));
	return 0;
}

double
f(double x) {
	return sin(5*x) + cos(10*x) + x*x/10;
}

double
bisection(double x1, double x2, double eps, int limit) {
	double fx1, fx2, mid, fmid;
	int iterations=0;
	fx1 = f(x1);
	fx2 = f(x2);
	while (x2-x1 > eps) {
		iterations = iterations+1;
		if (iterations==limit) {
			exit(NOT_CONVERGING);
		}
		mid = (x1+x2)/2;
		fmid = f(mid);
		if (fx1*fmid < 0) {
			/* root is to left of middle */
			x2 = mid;
			fx2 = fmid;
		} else {
			/* root is to right */
			x1 = mid;
			fx1 = fmid;
		}
	}
	printf("(bisection) eps=%.1e, iterations=%d\n",
			eps, iterations);
	return (x1+x2)/2;
}