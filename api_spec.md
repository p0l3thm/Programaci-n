openapi: 3.0.3
info:
  title: API de Control de Grúa Torre
  description: >
    Documentación técnica formal para el sistema de control remoto de la Grúa Torre.
    Define los endpoints del servidor web (ESP32) y la especificación del protocolo UART de comunicación entre el ESP32 y el Arduino Nano.
  version: 1.0.0
servers:
  - url: http://{esp32_ip}
    description: Servidor Web Local en el ESP32
    variables:
      esp32_ip:
        default: 192.168.4.1
        description: La dirección IP asignada al ESP32 en la red Wi-Fi.

paths:
  /:
    get:
      summary: Interfaz de Usuario Web
      description: Retorna el archivo HTML/CSS/JS con el panel de control (frontend) de la grúa.
      responses:
        '200':
          description: Archivo HTML (index.html)
          content:
            text/html:
              schema:
                type: string
        '500':
          description: Error interno del servidor al leer el archivo HTML.

  /api/control:
    get:
      summary: Control de Motores (Comando Único)
      description: >
        Envía un comando de acción para controlar el movimiento de la grúa (traslación, elevación o rotación). 
        El comando es reenviado de forma síncrona vía UART hacia el Arduino Nano.
      parameters:
        - name: cmd
          in: query
          required: true
          description: Letra del comando a ejecutar según el protocolo.
          schema:
            type: string
            enum: ['F', 'B', 'U', 'D', 'L', 'R', 'S']
      responses:
        '200':
          description: Comando ejecutado exitosamente.
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
                    example: "ok"
                  cmd:
                    type: string
                    example: "F"
        '400':
          description: Comando inválido.
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
                    example: "error"
                  msg:
                    type: string
                    example: "Invalid cmd"

components:
  schemas:
    UARTProtocol:
      description: >
        El protocolo UART define la mensajería serial asíncrona entre el servidor Web (ESP32) y el actuador (Arduino Nano).
        *   **Velocidad:** 9600 baudios.
        *   **Bits de Datos:** 8
        *   **Paridad:** Ninguna (None)
        *   **Bits de Parada:** 1
        *   **Formato del Paquete:** Carácter único codificado en ASCII (1 byte).
      type: object
      properties:
        Mensajes_Soportados:
          type: array
          items:
            type: string
            description: >
              **Tabla Detallada de Comandos UART:**
              
              | Comando | ASCII (Hex) | Acción | Motor Afectado | Descripción |
              |:---:|:---:|---|---|---|
              | `F` | `0x46` | Adelante | DC Motor A (Carro) | Mueve el carro de traslación hacia el frente. |
              | `B` | `0x42` | Atrás | DC Motor A (Carro) | Mueve el carro de traslación hacia la base. |
              | `U` | `0x55` | Subir | DC Motor B (Elevación) | Recoge el cable del gancho (Sube). |
              | `D` | `0x44` | Bajar | DC Motor B (Elevación) | Suelta el cable del gancho (Baja). |
              | `L` | `0x4C` | Izquierda | Stepper (Rotación) | Gira la grúa en sentido antihorario. |
              | `R` | `0x52` | Derecha | Stepper (Rotación) | Gira la grúa en sentido horario. |
              | `S` | `0x53` | Stop | Todos | Detiene inmediatamente cualquier movimiento activo. |
              
              *Nota de Seguridad:* El firmware del Arduino incorpora un timeout (500 ms por defecto). Si no se recibe repetidamente el carácter de un comando activo antes del timeout, el sistema transiciona automáticamente al estado `Stop`.
