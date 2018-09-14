#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>


int itoa(int num, char *str, int power)
{
	if(power != 10)
		return -1;
	while(num)
	{
		*str = (num % 10) + '0';
		num /= 10;
		str++;
	}
	return 0;
}

void set_log(char *str, ...)
{
	char strbuf[2048] = {0}, *pw = strbuf;
	char numstr[10] = {0};

	va_list multiArg = NULL;
	va_start(multiArg, str);	//第二个参数是可变参数前面的哪一个参数

	//根据占位符判断类型，组合字符串
	while(*str != '\0') {
		if(*str == '%'){
			switch(*(str+1))
			{
			case 'c':
				*pw = (char)va_arg(multiArg, int);
				pw++; str++;
				break;
			case 'd':
				itoa(va_arg(multiArg, int), numstr, 10);
				strcat(pw, numstr);
				pw += strlen(pw);
				str++;
				break;
			case 's':
				strcat(pw, va_arg(multiArg, char *));
				pw += strlen(pw);
				str++;
				break;
			default:
				break;
			}
		}else{
			*pw = *str;
			pw++;
		}
		str++;
	}
	va_end(multiArg);

	printf("%s\n", strbuf);
	return;
}

int main()
{
	char buf[10] = "123";
	int aqa = 123;
	char ch = '2';
	set_log("wo ai chi %s and %d and %c hehe\n", buf, aqa, ch);
	exit(1);
}

