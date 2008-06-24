/*
 *	rshutdownd.c
 *
 *	remote shutdown Linux daemon
 * */


#include <shadow.h>
#include <stdio.h>

int check(const char * username,const char * password)
{
	struct spwd * s=getspnam(username);
	char * cp;
	if(s==NULL) return 0;
	cp=crypt(password,s->sp_pwdp);

	if(strcmp(cp,s->sp_pwdp)==0)
		return 1;
	else
		return 0;
}

int main()
{
	return 0;
}
