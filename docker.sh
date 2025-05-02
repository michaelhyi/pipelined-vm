#!/bin/bash

docker buildx create --use
docker buildx build --platform=linux/amd64 -t pipelined-vm . --load

docker run --platform=linux/amd64 -it --rm \
	-v "$PWD":/workspace \
	-w /workspace \
	pipelined-vm bash

