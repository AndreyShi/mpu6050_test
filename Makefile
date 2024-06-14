all:
	gcc main.c -pthread -lwiringPi -lm -o main
bcm:
	cd ~
	wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.68.tar.gz
	tar zxvf bcm2835-1.68.tar.gz
	cd bcm2835-1.68
	./configure
	make
	sudo make check
	sudo make install
unzipbcm175:
	tar zxvf bcm2835-1.75.tar.gz
install_bcm175:
	cp bcm2835.c bcm2835-1.75/src
	cd bcm2835-1.75
	./configure
	make
	sudo make check
	sudo make install
myprog:
	g++ -g MPU6050_example_1.cpp I2Cdev.cpp MPU6050.cpp MPU6050_6Axis_MotionApps20.cpp -l bcm2835 -l m -o MPU6050_example_1
	sudo gdb ./MPU6050_example_1
mylib:
	g++ -c MPU6050_example_1.cpp I2Cdev.cpp MPU6050.cpp MPU6050_6Axis_MotionApps20.cpp -l bcm2835 -l m 
	ar r libMPU6050.a *.o
	rm *.o
mylibshared:
	g++ -c -fPIC MPU6050_example_1.cpp I2Cdev.cpp MPU6050.cpp MPU6050_6Axis_MotionApps20.cpp -l bcm2835 -l m
	g++ -shared -o libMPU6050.so *.o
	rm *.o
myproglib:
	g++ testlib.cpp -pthread -L./ -lMPU6050 -l bcm2835 -l m -o testlib
