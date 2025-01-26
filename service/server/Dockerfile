FROM ubuntu:24.04@sha256:6e75a10070b0fcb0bead763c5118a369bc7cc30dfc1b0749c491bbb21f15c3c7

RUN apt-get update && apt-get install gcc socat sudo -yqq \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /task

COPY . .

RUN gcc -fshort-enums -o main main.c 

CMD ["socat", "tcp-l:1337,reuseaddr,fork", "EXEC:'/task/main'"]

EXPOSE 1337/tcp