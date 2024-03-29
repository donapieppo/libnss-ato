#!/bin/bash

set -e

fini() {
  local retval="$?"

  rm -rf ${destdir} \
    /usr/lib/libnss_ato*.so* \
    /etc/libnss-ato.conf

  echo ""
  make clean

  if [ $retval -eq 0 ]; then
    echo -e "\nAll tests PASSED\n"
  else
    echo -e "\nOne or more tests FAILED\n" 1>&2
  fi

  exit "$retval"
}

trap fini EXIT

echo -e "\nBuild test ...\n"
make
echo -e "\nBuild test PASS ...\n"

echo -e "\nInstall test ...\n"

destdir="$PWD/tmp"
make install DESTDIR="${destdir}"

ls ${destdir}/usr/lib/libnss_ato*.so*
test -s ${destdir}/etc/libnss-ato.conf
test -s ${destdir}/usr/share/man/man3/libnss-ato.3

echo -e "\nInstall test PASS ...\n"

echo -e "\nFunctionality tests ..."

echo "Try with root user ..."
./libnss_ato_test root

echo  -e "\nTry with non existent user without enabling libnss-ato ..."

if ./libnss_ato_test test123; then
  echo "Passed with non existent user without enabling libnss-ato ..." 1>&2
  exit 1
fi

echo -e "\nFailed as expected ..."

echo -e "\nEnable libnss-ato now ..."

cp ${destdir}/usr/lib/libnss_ato*.so* /usr/lib/
cp ${destdir}/etc/libnss-ato.conf /etc/

ldconfig

echo "t:x:1000:1000:Test User:/home/test:/bin/bash" >> /etc/libnss-ato.conf

sed -ie '/^passwd:/ s/$/ ato/' /etc/nsswitch.conf
sed -ie '/^shadow:/ s/$/ ato/' /etc/nsswitch.conf

echo  -e "\nTry with non existent user after enabling libnss-ato ..."

if ! ./libnss_ato_test test123; then
  echo "Failed with non existent user after enabling libnss-ato ..." 1>&2
  exit 1
fi

echo -e "\nFunctionality tests PASS ...\n"
exit 0
