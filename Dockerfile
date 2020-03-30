FROM amd64/debian:latest

MAINTAINER Donapieppo <donapieppo@yahoo.it>

RUN apt-get update && apt-get install -y -y --no-install-recommends gcc vim locales make libc6-dev apt-utils 

WORKDIR /root
COPY . .

RUN useradd -ms /bin/bash testuser
RUN echo "t:x:1000:1000:Test User:/home/test:/bin/bash" > /etc/libnss-ato.conf 

RUN sed -i -e 's/passwd:         files/passwd:         files ato/' /etc/nsswitch.conf
RUN sed -i -e 's/shadow:         files/shadow:         files ato/' /etc/nsswitch.conf

# docker build -t libnss-ato .
# docker run -it libnss-ato  /bin/bash
# and then make && make install
# add ato to /etc/nsswitch.conf
# example sed -i -e 's/passwd:         files/passwd:         files ato/' /etc/nsswitch.conf
#         sed -i -e 's/shadow:         files/shadow:         files ato/' /etc/nsswitch.conf

 
