FROM amd64/debian:latest

MAINTAINER Donapieppo <donapieppo@yahoo.it>

RUN apt-get update && apt-get install -y -y --no-install-recommends gcc vim locales make libc6-dev apt-utils 

WORKDIR /root
COPY . .

RUN useradd -ms /bin/bash testuser

# docker build -t libnss-ato .
# docker run -it libnss-ato  /bin/bash
# and then make && make install
# echo "test_user:x:1000:1000:Test User:/home/test:/bin/bash" > /etc/libnss-ato.conf and add ato to /etc/nsswitch.conf
 
