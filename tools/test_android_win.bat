adb push ../build/demo.out /data/local/tmp/dlas/demo.out
adb push ../build/libdlas.a /data/local/tmp/dlas/libdlas.a

adb shell "chmod 777 -R ./data/local/tmp/dlas/ && ./data/local/tmp/dlas/demo.out"