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
	return;
}
int main() {
	char buf[20] = "abcdefghijklmno";
	double_to_char(34.56, buf);
	printf("%s\n", buf);
}
