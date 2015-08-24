#!/bin/bash

if [ -d export ]; then
	rm -rf export;
fi

mkdir -p export/freertos;

make -C Demo/ARMv7_Mango_GCC clean
cp -r Makefile Demo Example Source export/freertos
tar -C export -czf export/freertos.tar.gz freertos
rm -rf export/freertos
