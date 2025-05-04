#!/bin/bash

docker build -t pipelined-vm .

docker run -it --rm \
	-v "$PWD":/workspace \
	-w /workspace \
	pipelined-vm bash
