adb push ../bin/demo /data/local/tmp/ecas/demo
adb push ../bin/libecas.a /data/local/tmp/ecas/libecas.a

adb shell "chmod 777 -R ./data/local/tmp/ecas/ && ./data/local/tmp/ecas/demo"
