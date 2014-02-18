#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>

struct spwd *getspnam(const char *name);

int main (int argc, char *argv[]){

	struct passwd *p;
	struct spwd *s;

	char *usern="pippo.pluto";
	
	if(argc ==2)
	{
		usern=argv[1];
	}

	if (!(p=getpwnam(usern))) {
		return -1;
	}
	
	printf("FROM PASSWORD:\n");

	printf("\tname:  %s\n", p->pw_name);
	printf("\tdir:   %s\n", p->pw_dir);
	printf("\tgecos: %s\n", p->pw_gecos);

	printf("FROM SHADOW:\n");

	if (!(s = getspnam(usern))) {
		return -1;
	}

	printf ("\tname:  %s\n", s->sp_namp);
	printf ("\tpass:  %s\n", s->sp_pwdp);

	return 0;

}

