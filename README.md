libnss-ato (Name Service Switch module All-To-One)
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

This module can be used for pubblic workstations where you only need to verify username / password from a pam module (for example pam-krb5 for Active Directory users) and there is no need to give the user his own uid, gid or homedir.

Installation from source
=========
From source just make and make install.

The only configuration file is `/etc/libnss-ato.conf` which consists of one line in the passwd format. For example:

```console
test_user:x:1000:1000:Test User,:/home/test:/bin/bash
```

Only the first line of the file `/etc/libnss-ato.conf` is parsed.

Here an example of the system file `/etc/nsswitch.conf` to make use of libnss-ato:

```console
passwd:         files ato
group:          files
shadow:         files ato
```

Installation from Debian packages
=========
To build the Debian packages, run the following:
```console
fakeroot debian/rules binary
```
This should create the correct `.deb` in the parent directory.

This package can then be installed as with any other package:
```console
sudo dpkg -i <package_name>
```
You will then need to modify the config files, as above.
