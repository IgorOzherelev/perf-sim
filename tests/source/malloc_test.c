#include<stdlib.h>
int main()
{
	char* str;
	char c;
	str = (char*)malloc(3*sizeof(char));
	if (str == NULL)
		return -1;
	str[0] = 'a';
	str[1] = 'b';
	c = str[0] + str[1];
	free(str);
	return 0;
}
