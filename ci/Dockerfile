FROM fedora:latest

MAINTAINER Donapieppo <donapieppo@yahoo.it>

RUN dnf install -y gcc --refresh

# docker build -t libnss-ato .
# docker run -it -v$(pwd):/build -w/build libnss-ato /bin/bash
#
# and then make && make installi
#
# make necessary modifications to /etc/libnss-ato.conf, example
# echo "t:x:1000:1000:Test User:/home/test:/bin/bash" > /etc/libnss-ato.conf
#
# add ato to /etc/nsswitch.conf, example:
# sed -ie '/^passwd:/ s/$/ ato/' /etc/nsswitch.conf
# sed -ie '/^shadow:/ s/$/ ato/' /etc/nsswitch.conf
