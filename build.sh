#!/bin/bash

BIN_DIR=bin

[ -d $BIN_DIR ] && rm -rf $BIN_DIR
mkdir $BIN_DIR

cmake --build .
