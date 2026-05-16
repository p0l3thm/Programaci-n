# pyrefly: ignore [missing-import]
import network
import time

# Reemplaza con tus credenciales
SSID = 'Tu_Red_WiFi'
PASSWORD = 'Tu_Password'

def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('Conectando a la red...')
        wlan.connect(SSID, PASSWORD)
        timeout = 10
        while not wlan.isconnected() and timeout > 0:
            time.sleep(1)
            timeout -= 1
            print('.', end='')
    
    if wlan.isconnected():
        print('\nConexión exitosa!')
        print('Configuración de red:', wlan.ifconfig())
    else:
        print('\nFallo al conectar.')

connect_wifi()
