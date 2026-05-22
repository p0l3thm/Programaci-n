# Control y Monitoreo de ESP32 vía Red Local (Sockets TCP) 📡

Este proyecto demuestra cómo establecer una comunicación bidireccional en tiempo real entre una interfaz gráfica en una computadora (creada con Python y Tkinter) y un microcontrolador ESP32 (usando MicroPython). 

A través de esta conexión de red local, podemos monitorear sensores (como un potenciómetro) y controlar actuadores (como el LED integrado) de forma remota.

## 📂 Estructura del Proyecto

El repositorio está dividido en dos carpetas principales:

1. **`ESP32/`**: Contiene el código `main.py` que debe ser ejecutado en el microcontrolador. En esta arquitectura, la ESP32 actúa como el **Servidor** que escucha las peticiones en la red.
2. **`servidor/`**: Contiene el código de la interfaz gráfica de la PC. Aunque la carpeta se llama así por el entorno de trabajo, este script actúa realmente como el **Cliente** que se conecta a la ESP32.

## ⚙️ Requisitos Previos

* **Hardware:** Una placa ESP32, un cable USB de datos y un potenciómetro conectado al pin 32.
* **Software PC:** Visual Studio Code (VS Code) con Python 3.x instalado.
* **Software ESP32:** Thonny IDE para cargar el código MicroPython a la placa.
* **Red:** Tanto la PC como la ESP32 deben estar conectadas a la **misma red Wi-Fi**.

## 🚀 Guía de Ejecución

Sigue estos pasos en el orden exacto para que la conexión sea exitosa:

### Paso 1: Iniciar el Servidor en la ESP32
1. Abre **Thonny IDE** y conecta tu ESP32.
2. Abre el archivo que se encuentra en la carpeta `ESP32/`.
3. Verifica que las credenciales de la red Wi-Fi (`SSID` y `PASSWORD`) sean correctas.
4. Ejecuta el código. En la consola de Thonny verás un mensaje indicando "Conectado!" junto con la **Dirección IP** que el router le asignó a la ESP32.
5. **Copia esa dirección IP**, la necesitarás en el siguiente paso.

### Paso 2: Iniciar la Interfaz en la Computadora
1. Abre **Visual Studio Code** y carga la carpeta `servidor/`.
2. Abre el archivo de Python.
3. En la sección de configuración, reemplaza el valor de la variable `IP_ESP32` por la IP que copiaste en el paso anterior.
4. Ejecuta el código en VS Code. 
5. Se abrirá la ventana de la interfaz. Si todo está correcto, la consola mostrará "Conectado al ESP32" y verás la barra del potenciómetro moverse en tiempo real, además de poder encender y apagar el LED de la placa con los botones.
