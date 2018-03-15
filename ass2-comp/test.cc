#include "stdio.h"
#include "stdint.h"

typedef uint8_t uint8;

typedef unsigned int uint;

void double_to_char(double num, char buf[]) {
	int x = (int) num;
	int i;
	//printf("%s\n", buf);
	for (i = 0; x > 0; i++) 
	{
		buf[i] = x % 10 + '0';
		x /= 10;
	}
	//printf("i = %d\n", i);
	//printf("%s\n", buf);
	for (int j=0; j < i/2; j++)
	{
		char t = buf[j];
	  	buf[j] = buf[i-1  - j];
	  	buf[i-1 - j] = t;
	}
	//printf("%s\n", buf);
	if (num == (double)(int) num) {
		buf[i++] = 0;
		return;	
	}
	buf[i++] = '.';
	float fracs = num * 10.0;
	for (int j = 0; fracs != 0 && j < 5; j++)
	{	
		buf[i++] = ((uint)fracs)%10 + '0';
		fracs -= (int) fracs;
		fracs *= 10.0;
	}
	buf[i++] = 0;
}

double char_to_double(char buf[]) {
	double x = 0;
	int i;
	//printf("%s\n", buf);
	for (i = 0; buf[i] != '.' && buf[i] != 0; i++) 
	{
		x *= 10;
		x += buf[i] - '0';
	}
	if (buf[i] == 0) {
		return x;
	}
	++i;
	printf("string = %s, i = %d, double = %lf\n", &buf[i], i, x);
	int j;
	int divisor = 1;
	for (j = 0; buf[i] != 0; i++, ++j) 
	{
		x *= 10;
		x += buf[i] - '0';
		divisor *= 10;
	}
	printf("j = %d, double = %lf\n\n", j, x);
	x /= divisor;
	return x;
}


int main() {
	double num = 256.8;
	char buf[20] = "abcdefghijklmno";
	double_to_char(num, buf);
	printf("%s\n", buf);
	num = -1;
	num = char_to_double(buf);
	printf("%lf\n", num);
}
