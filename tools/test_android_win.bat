adb push ../bin/demo.out /data/local/tmp/dlas/demo.out
adb push ../bin/libdlas.a /data/local/tmp/dlas/libdlas.a

adb shell "chmod 777 -R ./data/local/tmp/dlas/ && ./data/local/tmp/dlas/demo.out"