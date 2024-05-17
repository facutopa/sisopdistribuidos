### Explicación del `server.c`

El código de `server.c` implementa un servidor concurrente que maneja múltiples clientes utilizando un pool de hilos. El servidor acepta conexiones de clientes y asigna cada cliente a un hilo basado en su prioridad. Además, maneja la desconexión ordenada de todos los clientes si el servidor se cierra.

#### Funciones y Componentes Principales:

1. **Estructuras de Datos**:
    - `ThreadArgs`: Contiene la información necesaria para manejar la conexión de un cliente, incluyendo el socket del cliente, el ID del hilo asignado y la prioridad del cliente.
    - `ThreadInfo`: Contiene información sobre los hilos en el pool, incluyendo el ID del hilo, el estado de ocupación y los argumentos del hilo.
    - `PriorityQueueNode`: Nodo utilizado para implementar una cola de prioridad para manejar a los clientes basándose en su prioridad.

2. **Declaraciones Globales**:
    - `thread_pool`: Array que representa el pool de hilos.
    - `priority_queue`: Cola de prioridad para gestionar los clientes según su prioridad.
    - `server_fd`: Descriptor de archivo del socket del servidor.

3. **Funciones**:
    - `insert_priority_queue(ThreadArgs *client_args)`: Inserta un cliente en la cola de prioridad en función de su prioridad.
    - `get_next_client()`: Obtiene el próximo cliente con mayor prioridad de la cola.
    - `handle_client(void *args)`: Función que maneja la comunicación con el cliente. Lee mensajes del cliente y permite al servidor responder.
    - `close_all_clients()`: Cierra todos los sockets de cliente y marca los hilos como no ocupados.
    - `handle_sigint(int sig)`: Manejador de la señal `SIGINT` que cierra todos los clientes y el socket del servidor cuando se recibe la señal (Ctrl+C).

4. **Flujo Principal**:
    - Configura el manejador de señal para `SIGINT`.
    - Inicializa el pool de hilos marcando todos los hilos como no ocupados.
    - Configura el socket del servidor (creación, enlace y escucha).
    - Entra en un bucle principal para aceptar conexiones de clientes.
    - Para cada cliente conectado:
        - Recibe la prioridad del cliente.
        - Crea los argumentos del hilo (`ThreadArgs`) y los inserta en la cola de prioridad.
        - Asigna el cliente al siguiente hilo disponible en el pool, creando un nuevo hilo para manejar la comunicación.

### Explicación del `cliente.c`

El código de `cliente.c` implementa un cliente que se conecta a un servidor, envía su prioridad y permite la comunicación bidireccional. El cliente puede enviar mensajes al servidor y recibir respuestas. Además, detecta cuando el servidor se desconecta y muestra un mensaje en la pantalla.

#### Funciones y Componentes Principales:

1. **Configuración del Cliente**:
    - Toma la dirección IP del servidor y la prioridad del cliente como argumentos de línea de comandos.
    - Crea un socket y configura la dirección del servidor.
    - Conecta al servidor utilizando el socket configurado.

2. **Comunicación con el Servidor**:
    - Envía la prioridad del cliente al servidor después de conectarse.
    - Entra en un bucle donde:
        - Solicita al usuario que ingrese un mensaje.
        - Envía el mensaje al servidor.
        - Lee la respuesta del servidor y la muestra en la pantalla.
        - Detecta si el servidor se ha desconectado y muestra un mensaje apropiado antes de salir del bucle.

3. **Detección de Desconexión**:
    - Si la función `send` devuelve un valor negativo, significa que hubo un error al enviar el mensaje, y el cliente sale del bucle.
    - Si la función `read` devuelve 0, significa que el servidor ha cerrado la conexión, y el cliente muestra el mensaje "Server disconnected" antes de salir del bucle.
