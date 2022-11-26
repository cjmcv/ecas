adb push ../bin/demo /data/local/tmp/dlas/demo
adb push ../bin/libdlas.a /data/local/tmp/dlas/libdlas.a

adb shell "chmod 777 -R ./data/local/tmp/dlas/ && ./data/local/tmp/dlas/demo"
