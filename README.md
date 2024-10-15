# Установка Wifi HOTSPOT и сервера MPU

Для установки точки доступа и сервера зайти в директорию rpihotspot и выполнить make.  
Все настройки точки доступа прописаны в Makefile: rpihotspot/Makefile, по умолчанию адрес сервера 172.16.0.1 порт 2222.  
Описание настроек точки доступа в rpihotspot/README.md.  
Установка будет длиться около 5 минут в конце будет reboot.  
После установки и перезапуска, сервер запустится автоматически.

Для получения данных  с MPU6050 команда give_MPU_bin_data, полученный результат 9 последовательных чисел типа float, выглядет так: yarn,pitch,roll,AccX,AccY,AccZ,GyroX,GyroY,GyroZ.

Для получения данных с дальномера команда get_distance, полученный результат строка вида: 2.48m.

пример получения данных в файле client.py  
для ручного запуска сервера зайти в папку /home/pi/mpu-setup и
выполнить команду sudo python server.py.

---

Программа получения сырых данных с MPU6050 и получения из них углов для raspberry pi zero. Вся математика: https://github.com/rfetick/MPU6050_light подключение по I2C: https://github.com/WiringPi/WiringPi Получение данных c DMP на raspberry pi zero, библиотека: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050

реализация точки доступа https://github.com/idev1/rpihotspot , пока получилось использовать только как точка доступа (без uap0), для этого в файле /etc/hostapd/hostapd.conf в строке interface=uap0 заменил uap0 на wlan0 скрипт запускался: sudo ./setup-network.sh --install --ap-ssid="abc-1" --ap-password="password@1" --ap-password-encrypt --ap-country-code="RU" --ap-ip-address="192.168.0.1" --wifi-interface="wlan0"
add: решение вопроса по работе точка доступа + инет на одном wifi: https://github.com/idev1/rpihotspot/issues/40 , создал файл /etc/udev/rules.d/90-wireless.rules
и туда прописал: ACTION=="add", SUBSYSTEM=="ieee80211", KERNEL=="phy0", RUN+="/sbin/iw phy %k interface add uap0 type __ap" , перезагрузил и заработало wifi инет + wifi ap  в ifconfig стал отображаться uap0

команда для сборки библиотеки:
make mylibshared1 - создает динамическую либу libMPU6050.so необходимую для запусков примеров testlibpy.py и testlib.cpp
если надо запустить пример на python:
sudo python testlibpy.py
если надо запустить пример на C++ то нужно его скомпилировать и запустить:
make myproglib

описание целей в make:
all - скомпилировать программу для получения сырых данных вся математика: https://github.com/rfetick/MPU6050_light, подключение по I2C: https://github.com/WiringPi/WiringPi
bcm - автоматизированный скрипт для установки bcm драйвера
unzipbcm175 - распаковка драйвера
zipbcm175 - упаковать драйвер
install_bcm175 - автоматизированный скрипт для установки bcm драйвера без скачивания
myprog - компиляция программы с предустановленной библиотекой bcm и последующий запуск отладчика
myprog1 - компиляция программы с исходниками драйвера bcm и последующий запуск отладчика
mylib - компиляция статической либы libMPU6050.a с предустановленной библиотекой bcm
mylibshared - компиляция динамической либы libMPU6050.so с предустановленной библиотекой bcm
mylibshared1 - компиляция динамической либы libMPU6050.so с исходниками драйвера bcm
myproglib - скомпилировать и запустить пример на C++
