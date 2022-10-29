#!/bin/bash
adb push ../build/demo.out /data/local/tmp/dlas/demo.out
adb push ../build/libdlas.a /data/local/tmp/dlas/libdlas.a

adb shell 
cd /data/local/tmp
chmod 777 -R dlas
./dlas/demo.out