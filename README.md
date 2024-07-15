Программа получения сырых данных с MPU6050 и получения из них углов для raspberry pi zero. Вся математика: https://github.com/rfetick/MPU6050_light подключение по I2C: https://github.com/WiringPi/WiringPi
Получение данных c DMP на raspberry pi zero, библиотека: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050

реализация точки доступа https://github.com/idev1/rpihotspot , пока получилось использовать только как точка доступа (без uap0),
для этого в файле /etc/hostapd/hostapd.conf в строке interface=uap0 заменил uap0 на wlan0
скрипт запускался:
sudo ./setup-network.sh --install --ap-ssid="abc-1" --ap-password="password@1" --ap-password-encrypt --ap-country-code="RU" --ap-ip-address="192.168.0.1" --wifi-interface="wlan0"
