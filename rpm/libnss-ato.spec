Name:           libnss-ato
Version:        0.2
Release:        1
Summary:        NSS module to match all request of unknown user to a single user (All To One)

License:        GPL
URL:            http://www.dm.unibo.it/~donatini

%description
NSS module to match all request of unknown user to a single user (All To One)

%install
%define _copy_to_buildroot() mkdir -p %{buildroot}%2; cp -r %{_topdir}/%1 %{buildroot}%2

# Copy the necessary files to the buildroot.
#
# There are lots of ways to do this:
# - Manually write the necessary mkdir/cp commands
# - Use macros to make writing these commands easier
# - Call a helper script to populate the buildroot (e.g. by parsing debian/*.install)
#
# Here is an example of doing this with macros.
%{_copy_to_buildroot libnss_ato.so.2 /lib64}
ln -s /lib64/libnss_ato.so.2 %{buildroot}/lib64/libnss_ato-2.3.6.so
%{_copy_to_buildroot libnss-ato.3 /usr/share/man/man3}
%{_copy_to_buildroot libnss-ato.conf /etc}

%debug_package

%files
# Specify the files to package.
/lib64/libnss_ato.so.2
/lib64/libnss_ato-2.3.6.so
%config(noreplace) /etc/libnss-ato.conf
/usr/share/man/man3/libnss-ato.3
