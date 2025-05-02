#!/bin/bash

if ! docker buildx inspect pipelined-vm-builder > /dev/null 2>&1; then
    docker buildx create --name pipelined-vm-builder --use
else
    docker buildx use pipelined-vm-builder
fi

docker buildx build --platform=linux/amd64 -t pipelined-vm . --load

docker run --platform=linux/amd64 -it --rm \
	-v "$PWD":/workspace \
	-w /workspace \
	pipelined-vm bash
