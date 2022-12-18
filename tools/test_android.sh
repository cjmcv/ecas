#!/bin/bash
adb push ../build/demo.out /data/local/tmp/ecas/demo.out
adb push ../build/libecas.a /data/local/tmp/ecas/libecas.a

adb shell 
cd /data/local/tmp
chmod 777 -R ecas
./ecas/demo
