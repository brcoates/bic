#!/bin/bash

echo "---------------------------------------------------------"
cat "data/test.txt"
echo "---------------------------------------------------------"

./build.sh

if [ $? -eq 0 ]; then
	./bin/bic "./test/basic.txt"
fi
