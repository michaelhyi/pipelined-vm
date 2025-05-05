FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
	build-essential \
    libcriterion-dev \
	gdb \
	valgrind \
    clang-format \
