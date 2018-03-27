/*
 * libnss_ato.c
 *
 * Nss module to map every requested user to a fixed one.
 * Ato stands for "All To One"
 *
 * Copyright (c) Pietro Donatini (pietro.donatini@unibo.it), 2007.
 *
 * this product may be distributed under the terms of
 * the GNU Lesser General Public License.
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
#include <stdio.h>

/* for security reasons */
#define MIN_UID_NUMBER   500
#define MIN_GID_NUMBER   500
#define CONF_FILE "/etc/libnss-ato.conf"

/*
 * the configuration /etc/libnss-ato.conf is just one line
 * with the local user data as in /etc/passwd. For example:
 * dona:x:1001:1001:P D ,,,:/home/dona:/bin/bash
 * Extra lines are comments (not processed).
 */

struct passwd *
read_conf() 
{
	FILE *fd;
	struct passwd *conf;

	if ((fd = fopen(CONF_FILE, "r")) == NULL ) {
		return NULL;
	}
	conf = fgetpwent(fd);

	if ( conf->pw_uid < MIN_UID_NUMBER )
		conf->pw_uid = MIN_UID_NUMBER;

	if ( conf->pw_gid < MIN_GID_NUMBER )
		conf->pw_gid = MIN_GID_NUMBER;

	fclose(fd);
	return conf;
}

/* 
 * Allocate some space from the nss static buffer.  The buffer and buflen
 * are the pointers passed in by the C library to the _nss_ntdom_*
 * functions. 
 *
 *  Taken from glibc 
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
	struct passwd *conf;
  
	if ((conf = read_conf()) == NULL) {
		return NSS_STATUS_NOTFOUND;
	}

	*p = *conf;

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

        if ((s->sp_pwdp = get_static(&buffer, &buflen, strlen("*") + 1)) == NULL) {
                return NSS_STATUS_TRYAGAIN;
        }

        strcpy(s->sp_pwdp, "*");

        s->sp_lstchg = 13571;
        s->sp_min    = 0;
        s->sp_max    = 99999;
        s->sp_warn   = 7;

        return NSS_STATUS_SUCCESS;
}
