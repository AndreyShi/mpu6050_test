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
bcm175:
	tar zxvf bcm2835-1.75.tar.gz
	cd bcm2835-1.75
	./configure
	make
	sudo make check
	sudo make install
myprog:
	g++ MPU6050_example_1.cpp I2Cdev.cpp MPU6050.cpp -l bcm2835 -l m -o MPU6050_example_1