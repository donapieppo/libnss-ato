#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>

struct spwd *getspnam(const char *name);

int main (){

	struct passwd *p;
	struct spwd *s;

	if (!(p=getpwnam("pippo.pluto"))) {
		return -1;
	}
	
	printf("FROM PASSWORD:\n");

	printf("\tname:  %s\n", p->pw_name);
	printf("\tdir:   %s\n", p->pw_dir);
	printf("\tgecos: %s\n", p->pw_gecos);

	printf("FROM SHADOW:\n");

	if (!(s = getspnam("pippo.pluto"))) {
		return -1;
	}

	printf ("\tname:  %s\n", s->sp_namp);
	printf ("\tpass:  %s\n", s->sp_pwdp);

	return 0;

}

