arm-none-linux-gnueabi-g++ -O1 -march=armv7-a -std=c++11 -static -o hw_arm.out helloWorld.c
arm-none-linux-gnueabi-g++ -Wall -O1 -march=armv7-a -std=c++11 -static -pg -o armProfLess_aam.out db_Benchmark.cpp aam_driver.c
