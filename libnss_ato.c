/*
 * libnss_ato.c
 *
 * Nss module to map every requested user to a fixed one.
 * Ato stands for "All To One"
 *
 * Copyright (c) Pietro Donatini (pietro.donatini@unibo.it), 2007.
 *
 * this product may be distributed under the terms of
 * the GNU Lesser Public License.
 *
 * version 0.2 
 * 
 * CHANGELOG:
 * strip end of line in reading /etc/libnss-ato 
 * suggested by Kyler Laird
 *
 * TODO:
 *
 * check bugs
 * 
 */

#include <nss.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* #include <syslog.h> */

/* for security reasons */
#define MIN_UID_NUMBER   500
#define MIN_GID_NUMBER   500
#define CONF_FILE "/etc/libnss-ato.conf"

/* 
 * The passwd structure in <pwd.h>
struct passwd
{
	char *pw_name;                Username. 
	char *pw_passwd;              Password. 
	__uid_t pw_uid;               User ID. 
	__gid_t pw_gid;               Group ID.
	char *pw_gecos;               Real name.
	char *pw_dir;                 Home directory.
	char *pw_shell;               Shell program. 
};

shadow passwd structure in <shadow.h>

struct spwd 
{
	char *sp_namp;          Login name 
	char *sp_pwdp;          Encrypted password 
	long sp_lstchg;         Date of last change
	long sp_min;            Min #days between changes
	long sp_max;            Max #days between changes
	long sp_warn;           #days before pwd expires to warn user to change it
	long sp_inact;          #days after pwd expires until account is disabled
	long sp_expire;         #days since 1970-01-01 until account is disabled 
	unsigned long sp_flag;  Reserved 
};

*/

/*  What can be configured in /etc/libnss-ato.conf */
typedef struct ato_conf {
	char *pw_name;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
	__uid_t pw_uid; 
	__gid_t pw_gid; 
} ato_conf_t;

/* constructor */
ato_conf_t *
new_conf()
{
	ato_conf_t *conf;

	if ((conf = (ato_conf_t *) malloc(sizeof (ato_conf_t))) == NULL) 
	    return NULL;

	conf->pw_name  = NULL;
	conf->pw_gecos = NULL;
	conf->pw_dir   = NULL;
	conf->pw_shell = NULL;
	
	return conf;
}	

/* destructor */
int 
free_conf(ato_conf_t *conf) 
{
	if ( conf->pw_name != NULL )
		free(conf->pw_name);
	if ( conf->pw_gecos != NULL )
		free(conf->pw_gecos);
	if ( conf->pw_dir != NULL )
		free(conf->pw_dir);
	if ( conf->pw_shell != NULL )
		free(conf->pw_shell);
	if ( conf != NULL )
		free(conf);
	return 0;
}

/*
 * the configuration /etc/libnss-ato.conf is just one line
 * whith the local user data as in /etc/passwd. For example:
 * dona:x:1001:1001:P D ,,,:/home/dona:/bin/bash 
 */

ato_conf_t *
read_conf() 
{
	ato_conf_t *conf;
	FILE *fd;
	char buff[BUFSIZ];
	char *b;
	char *value;

	if ((conf = new_conf()) == NULL) 
	    return NULL;
  
	if ((fd = fopen(CONF_FILE, "r")) == NULL ) {
		free_conf(conf);
		return NULL;
	}

	if (fgets (buff, BUFSIZ, fd) == NULL) {
		fclose(fd);
		free_conf(conf);
		return NULL;
	}

	fclose(fd);

	/* start reading configuration file */
	b = buff;
  
	/* pw_name */
	value = b;
       
	while (*b != ':' && *b != '\0')
		b++;
    
	if (*b != ':') 
		goto format_error;
  
	*b = '\0';
	b++;
	
	conf->pw_name = strdup(value);
  
	/* NOT USED pw_passwd will be set equal to  x (we like shadows) */
	while (*b != ':' && *b != '\0')
		b++;

	if ( *b != ':' )
		goto format_error;

	b++;
    
	/* pw_uid */
	value = b;

	while (*b != ':' && *b != '\0')
		b++;

	if (*b != ':')
		goto format_error;

	b++;

	conf->pw_uid = atoi(value);

	if ( conf->pw_uid < MIN_UID_NUMBER )
                conf->pw_uid = MIN_UID_NUMBER;

	/* pw_gid */
	value = b;

	while (*b != ':' && *b != '\0')
		b++;

	if (*b != ':')
		goto format_error;

	b++;
	conf->pw_gid = atoi(value);
	if ( conf->pw_gid < MIN_GID_NUMBER )
		conf->pw_gid = MIN_GID_NUMBER;

	/* pw_gecos */  
	value = b;

	while (*b != ':' && *b != '\0')
		b++;

	if (*b != ':')
		goto format_error;
          
	*b = '\0';
	b++;
  
	conf->pw_gecos = strdup (value);

	/* pw_dir */  
	value = b;

	while (*b != ':' && *b != '\0')
		b++;

	if (*b != ':')
		goto format_error;
          
	*b = '\0';
	b++;

	conf->pw_dir = strdup (value);
  
	/* pw_shell takes the rest */  
        /* Kyler Laird suggested to strip end line */
	value = b;

        while (*b != '\n' && *b != '\0')
                b++;

        *b = '\0';

	conf->pw_shell = strdup(value);

	return conf;

	format_error: 
		free (conf);
		return NULL;
}

/* 
 * Allocate some space from the nss static buffer.  The buffer and buflen
 * are the pointers passed in by the C library to the _nss_ntdom_*
 * functions. 
 *
 *  Taken fron glibc 
 */

static char * 
get_static(char **buffer, size_t *buflen, int len)
{
	char *result;

	/* Error check.  We return false if things aren't set up right, or
         * there isn't enough buffer space left. */

	if ((buffer == NULL) || (buflen == NULL) || (*buflen < len)) {
		return NULL;
	}

	/* Return an index into the static buffer */

	result = *buffer;
	*buffer += len;
	*buflen -= len;

	return result;
}


enum nss_status
_nss_ato_getpwnam_r( const char *name, 
	   	     struct passwd *p, 
	             char *buffer, 
	             size_t buflen, 
	             int *errnop)
{
	ato_conf_t *conf;
  
	if ((conf = read_conf()) == NULL) {
		return NSS_STATUS_NOTFOUND;
	}


	/* If out of memory */
	if ((p->pw_name = get_static(&buffer, &buflen, strlen(name) + 1)) == NULL) {
		return NSS_STATUS_TRYAGAIN;
	}

	/* pw_name stay as the name given */
	strcpy(p->pw_name, name);

	if ((p->pw_passwd = get_static(&buffer, &buflen, strlen("x") + 1)) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

	strcpy(p->pw_passwd, "x");

        p->pw_uid = conf->pw_uid; /* UID_NUMBER; */
        p->pw_gid = conf->pw_gid; /* GID_NUMBER; */

	if ((p->pw_gecos = get_static(&buffer, &buflen, strlen(conf->pw_gecos) + 1 )) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(p->pw_gecos, conf->pw_gecos);

	if ((p->pw_dir = get_static(&buffer, &buflen, strlen(conf->pw_dir) + 1 )) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(p->pw_dir, conf->pw_dir);

	if ((p->pw_shell = get_static(&buffer, &buflen, strlen(conf->pw_shell) + 1 )) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(p->pw_shell, conf->pw_shell);

        free_conf(conf);
        
	return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_ato_getspnam_r( const char *name,
                     struct spwd *s,
                     char *buffer,
                     size_t buflen,
                     int *errnop)
{

        /* If out of memory */
        if ((s->sp_namp = get_static(&buffer, &buflen, strlen(name) + 1)) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(s->sp_namp, name);

        if ((s->sp_pwdp = get_static(&buffer, &buflen, strlen("!") + 1)) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(s->sp_pwdp, "!");

        s->sp_lstchg = 13571;
        s->sp_min    = 0;
        s->sp_max    = 99999;
        s->sp_warn   = 7;

        return NSS_STATUS_SUCCESS;
}


