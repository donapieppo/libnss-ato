libnss-ato
==========

The libnss_ato module is a set of C library extensions which allows to map every nss request for unknown user to a single predefined user.

Description
=========

Suppose your system has only one account (apart from root and system users) named user_test:

```console
]$ id user_test
]$ uid=1000(user_test) gid=1000 groups=1000
```

If you add libnss-ato to the chain of nss modules (in /etc/nsswitch.conf) you get something like:

```console
]$ id randomname
]$ uid=1000(user_test) gid=1000 groups=1000
```

for every query of a random username not present in /etc/passwd.

Why?
=========

This module can be used for pubblic workstations where you only need to verify username / password from a pam module (for example pam-krb5 for Active Directory users) and there is no need to give the user his own uid gid homedir


