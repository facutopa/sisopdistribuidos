### PLANIFICACION DE HILOS
Nuestro último aspecto del diseño relativo a los hilos es la planificación. Los
hilos se pueden planificar mediante distintos algoritmos, entre los que se
encuentran por prioridad, round robin y FCFS. Los paquetes de hilos
proporcionan a menudo ciertas llamadas para que el usuario pueda especificar
el algoritmo de planificación y establecer las prioridades, en su caso.

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

Se logra la implementación de la siguiente manera:

1. **Planificación por Prioridad:**
   - En tu servidor, cuando un cliente se conecta, se recibe su prioridad inicial. Esto se hace en la parte donde se lee `priority_buffer` y se convierte a un entero (`client_priority`).
   - Luego, se crean los argumentos del hilo (`ThreadArgs`) que incluyen esta prioridad y se insertan en una cola de prioridad mediante la función `insert_priority_queue`.
   - La cola de prioridad (`priority_queue`) organiza a los clientes de manera que los de mayor prioridad sean atendidos primero. Esto se ve en la función `insert_priority_queue`, donde se inserta el nuevo cliente en la posición adecuada según su prioridad.

2. **Asignación de Hilos del Pool:**
   - Cuando un hilo del pool está disponible, se extrae el cliente con mayor prioridad de la cola de prioridad mediante la función `get_next_client`.
   - El cliente extraído se asigna a un hilo disponible, marcando el hilo como ocupado y creando el hilo con `pthread_create`, lo que asegura que los clientes con mayor prioridad sean atendidos primero, si hay hilos disponibles.

3. **Ejemplo de Código Relevante:**
   - Inserción en la cola de prioridad:
     ```c
     void insert_priority_queue(ThreadArgs *client_args) {
         PriorityQueueNode *new_node = (PriorityQueueNode *)malloc(sizeof(PriorityQueueNode));
         new_node->client_args = client_args;
         new_node->next = NULL;

         if (priority_queue == NULL || priority_queue->client_args->priority < client_args->priority) {
             new_node->next = priority_queue;
             priority_queue = new_node;
         } else {
             PriorityQueueNode *current = priority_queue;
             while (current->next != NULL && current->next->client_args->priority >= client_args->priority) {
                 current = current->next;
             }
             new_node->next = current->next;
             current->next = new_node;
         }
     }
     ```
   - Extracción del cliente de mayor prioridad:
     ```c
     ThreadArgs *get_next_client() {
         if (priority_queue == NULL) {
             return NULL;
         }
         PriorityQueueNode *node = priority_queue;
         priority_queue = priority_queue->next;
         ThreadArgs *client_args = node->client_args;
         free(node);
         return client_args;
     }
     ```

Este enfoque asegura que los clientes se manejen de acuerdo a sus prioridades.
