#include <stdio.h>

int main()
{

	char *buf="huzheng";

	printf("%d \r\n",strlen(buf));
	printf("%d \r\n", sizeof(buf));


	char *point = (char*)malloc(sizeof(char)*128);
	printf("%d \r\n", sizeof(point));
	printf("%d\r\n" , strlen(point));
	return 1;
}
