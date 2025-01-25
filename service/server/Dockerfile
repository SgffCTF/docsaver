FROM gcc:13.3 AS builder

WORKDIR /build

COPY ./src src
COPY ./lib lib
COPY main.c .

RUN gcc -fshort-enums -no-pie -o app main.c

FROM ubuntu:24.04@sha256:6e75a10070b0fcb0bead763c5118a369bc7cc30dfc1b0749c491bbb21f15c3c7

RUN apt-get update && apt-get install socat sudo -yqq \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /task

COPY --from=builder /build/app .

CMD ["socat", "tcp-l:1337,reuseaddr,fork", "EXEC:'/task/app'"]

EXPOSE 1337/tcp