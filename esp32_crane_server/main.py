# pyrefly: ignore [missing-import]
import uasyncio as asyncio
# pyrefly: ignore [missing-import]
import machine
import socket

# Configuración UART
# UART2, TX en GPIO17, RX en GPIO16 (no usado pero requerido por init)
uart = machine.UART(2, baudrate=9600, tx=17, rx=16)

# Configuración LED de Status
status_led = machine.Pin(2, machine.Pin.OUT)

def send_uart_command(cmd):
    # Enviar comando al Arduino Nano
    uart.write(cmd)
    # Parpadear LED de estado
    status_led.value(not status_led.value())

async def serve_client(reader, writer):
    try:
        request_line = await reader.readline()
        if not request_line:
            await writer.aclose()
            return

        request_line = request_line.decode('utf-8')
        method, path, _ = request_line.split(' ', 2)
        
        # Leer el resto de las cabeceras (necesario para no dejar basura)
        while True:
            line = await reader.readline()
            if not line or line == b'\r\n':
                break

        # Manejo de rutas
        if path == '/' or path == '/index.html':
            try:
                with open('index.html', 'r') as f:
                    html = f.read()
                
                response = 'HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n' + html
                await writer.awrite(response.encode('utf-8'))
            except Exception as e:
                await writer.awrite(b'HTTP/1.1 500 Internal Server Error\r\n\r\nError al leer index.html')
        
        elif path.startswith('/api/control'):
            # Parsear query string
            cmd = 'S'
            if '?' in path:
                query = path.split('?')[1]
                params = query.split('&')
                for p in params:
                    if p.startswith('cmd='):
                        cmd = p.split('=')[1]
            
            # Validar y enviar
            if cmd in ['F', 'B', 'U', 'D', 'L', 'R', 'S']:
                send_uart_command(cmd)
                response = 'HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n{"status":"ok","cmd":"'+cmd+'"}'
            else:
                response = 'HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n{"status":"error","msg":"Invalid cmd"}'
                
            await writer.awrite(response.encode('utf-8'))
        
        else:
            await writer.awrite(b'HTTP/1.1 404 Not Found\r\n\r\n404')
            
    except Exception as e:
        print("Error serving client:", e)
    finally:
        await writer.aclose()

async def main():
    print("Iniciando servidor web...")
    status_led.on() # Encendido al iniciar
    server = await asyncio.start_server(serve_client, "0.0.0.0", 80)
    
    while True:
        await asyncio.sleep(1)

# Iniciar bucle de eventos
try:
    asyncio.run(main())
except KeyboardInterrupt:
    print("Servidor detenido")
