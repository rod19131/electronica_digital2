# Código de ejemplo AdafruitIO
# Universidad del Valle de Guatemala
# IE3027 - Electrónica Digital 2
# José Alejandro Rodríguez Porras

# Adafruit IO
# https://io.adafruit.com/

# if Module not Found. Open Terminal/CMD and execute:
# pip3 install Adafruit_IO
#

from Adafruit_IO import Client, RequestError, Feed
import serial                                        #remover espacios entre letras
ADAFRUIT_IO_KEY = "insertar key" # key: a i o _ Q o o P 7 2 N v g O f t F b i A l P U j A 8 L 7 Q u 5 6
ADAFRUIT_IO_USERNAME = "rod19131"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
feed1 = int
feed2 = int
contador = int
serialcom2 = serial.Serial("COM2", baudrate = 9600, timeout = 1)
print("connected to: " + serialcom2.portstr)

#Digital Feed
while 1:
    try:
        feed1 = int(input("feed1:"))
        feed2 = int(input("feed2:"))
    except:
        exit()
    digital_feed1 = aio.feeds('adc')
    aio.send_data(digital_feed1.key, feed1)
    digital_data1 = aio.receive(digital_feed1.key)
    digital_feed2 = aio.feeds('temperaturalab5')
    aio.send_data(digital_feed2.key, feed2)
    digital_feed3 = aio.feeds('adacontlab5')
    digital_data3 =  aio.receive(digital_feed3.key)
    serialcom2.write([int(digital_data3.value)])
    contador = serialcom2.readline()
    print(contador)
