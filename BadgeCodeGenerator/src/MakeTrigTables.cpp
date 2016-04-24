#include <cmath>
#include <cstdio>

int
main(int argc, char *argv[])
{
	float begin = 0.0, end = 6.28;
	int steps = 256;

	printf("#ifndef SineCosineLUT_H\n");
	printf("#define SineCosineLUT_H\n");
	printf("\n");

	printf("\tfloat sineTable[%i] = {\n", steps);
	printf("\t\t");
	for (int i = 0; i < (steps-1); i++) {
		printf("%f, ", sin(begin + (end-begin)/steps * i));
	}
	printf("%f };\n", sin(end));

	printf("\n");

	printf("\tfloat cosineTable[%i] = {\n", steps);
	printf("\t\t");
	for (int i = 0; i < (steps-1); i++) {
		printf("%f, ", cos(begin + (end-begin)/steps * i));
	}
	printf("%f };\n", cos(end));

	printf("\n");

	printf("#endif\n");
}
