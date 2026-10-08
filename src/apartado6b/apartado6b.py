# //ESCRIBIR EN EL TERMINAL C:\Users\guill\.platformio\penv\Scripts\python.exe apartado6b.py 

import socket

# 1. Configuración del Servidor TCP
HOST = '0.0.0.0'  # '0.0.0.0' permite recibir conexiones de cualquier IP dentro de tu red local
PORT = 8888       # Mismo puerto que configuraste en el código de la ESP32

def iniciar_servidor_tcp():
    # Crear el socket IPv4 (AF_INET) y TCP (SOCK_STREAM)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Permitir reutilizar el puerto inmediatamente si se reinicia el script
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    # Enlazar dirección y puerto
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    
    print(f"Servidor TCP iniciado. Esperando conexión de la ESP32 en el puerto {PORT}...")

    while True:
        # Aceptar la conexión entrante de la ESP32
        conn, addr = server_socket.accept()
        print(f"\n¡ESP32 conectada exitosamente desde la IP: {addr[0]}!")
        
        try:
            while True:
                # Recibir datos del socket (bloque de hasta 1024 bytes)
                data = conn.recv(1024)
                
                # Si data está vacío, la ESP32 se desconectó
                if not data:
                    print("\nLa ESP32 se ha desconectado.")
                    break
                
                # Decodificar los bytes a texto UTF-8 e imprimirlos
                mensaje = data.decode('utf-8')
                print(mensaje, end='')
                
        except ConnectionResetError:
            print("\nConexión interrumpida bruscamente por la ESP32.")
        finally:
            conn.close()
            print("Esperando nueva conexión...")

if __name__ == "__main__":
    iniciar_servidor_tcp()