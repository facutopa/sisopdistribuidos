## Descripción de la Lógica de `servidor.c` y `cliente.c`

### Introducción

Estos programas implementan un sistema cliente-servidor básico que permite realizar operaciones en archivos ubicados en el servidor a través de un cliente. Las operaciones disponibles incluyen listar los archivos del directorio, leer el contenido de un archivo, modificar un archivo, eliminar un archivo y crear un archivo nuevo.

### `servidor.c`

El servidor es responsable de aceptar conexiones de los clientes y manejar sus solicitudes. Cada solicitud es manejada en un hilo separado para permitir la concurrencia.

#### Lógica del Servidor

1. **Inicialización del Servidor**:
    - El servidor se inicia especificando una dirección IP.
    - Se crea un socket TCP/IP.
    - Se configura la dirección del servidor con el puerto definido (`PUERTO`).
    - El socket se enlaza a la dirección y puerto especificados.
    - El servidor comienza a escuchar conexiones entrantes.

2. **Aceptación de Conexiones**:
    - El servidor acepta conexiones entrantes en un bucle infinito.
    - Por cada conexión aceptada, se crea un nuevo hilo para manejar la interacción con ese cliente específico.

3. **Manejo de Conexiones**:
    - Cada hilo maneja las solicitudes del cliente de forma concurrente.
    - El hilo recibe solicitudes del cliente y las procesa en función del primer carácter del mensaje:
        - **'1'**: Listar los archivos del directorio.
        - **'2'**: Enviar el contenido de un archivo.
        - **'3'**: Modificar el contenido de un archivo.
        - **'4'**: Eliminar un archivo.
        - **'5'**: Crear un archivo nuevo.
        - **'9'**: Cerrar la conexión.
    - Después de procesar cada solicitud, el servidor envía un mensaje de "FIN" para indicar que la operación ha terminado.

### `cliente.c`

El cliente se conecta al servidor y permite al usuario realizar varias operaciones en los archivos del servidor.

#### Lógica del Cliente

1. **Inicialización del Cliente**:
    - El cliente se inicia especificando la dirección IP del servidor y el puerto.
    - Se crea un socket TCP/IP.
    - Se configura la dirección del cliente con la IP del servidor y el puerto especificado.
    - El cliente se conecta al servidor.

2. **Interfaz de Usuario**:
    - El cliente muestra un menú de opciones al usuario:
        - **1**: Listar los archivos del directorio.
        - **2**: Leer el contenido de un archivo.
        - **3**: Modificar el contenido de un archivo.
        - **4**: Eliminar un archivo.
        - **5**: Crear un archivo nuevo.
        - **9**: Cerrar la conexión.
    - El usuario selecciona una opción y el cliente envía la solicitud correspondiente al servidor.

3. **Manejo de Respuestas del Servidor**:
    - El cliente recibe y muestra las respuestas del servidor.
    - Para las operaciones que involucran la recepción de datos (listado de directorio, contenido de archivo, etc.), el cliente sigue recibiendo datos hasta que recibe el mensaje de "FIN".

### Interacción entre Cliente y Servidor

1. **Conexión**:
    - El cliente se conecta al servidor especificando la dirección IP y el puerto.

2. **Solicitud y Respuesta**:
    - El cliente envía una solicitud al servidor basada en la opción seleccionada por el usuario.
    - El servidor recibe la solicitud, la procesa y envía la respuesta correspondiente.
    - El cliente recibe la respuesta y la muestra al usuario.

3. **Operaciones en Archivos**:
    - **Listar Directorio**:
        - El cliente envía la solicitud de listado de directorio.
        - El servidor lista los archivos del directorio actual y envía la lista al cliente.
        - El cliente muestra la lista de archivos.
    - **Leer Archivo**:
        - El cliente envía el nombre del archivo a leer.
        - El servidor lee el contenido del archivo y lo envía al cliente.
        - El cliente muestra el contenido del archivo.
    - **Modificar Archivo**:
        - El cliente envía el nombre del archivo a modificar y el nuevo contenido.
        - El servidor modifica el contenido del archivo y envía una confirmación.
        - El cliente muestra la confirmación.
    - **Eliminar Archivo**:
        - El cliente envía el nombre del archivo a eliminar.
        - El servidor elimina el archivo y envía una confirmación.
        - El cliente muestra la confirmación.
    - **Crear Archivo**:
        - El cliente envía el nombre del nuevo archivo y su contenido.
        - El servidor crea el archivo con el contenido especificado y envía una confirmación.
        - El cliente muestra la confirmación.

4. **Cierre de Conexión**:
    - El cliente envía la solicitud de cierre de conexión.
    - El servidor cierra la conexión y envía una confirmación.
    - El cliente muestra la confirmación y cierra la conexión.

### Conclusión

Estos programas demuestran cómo implementar un sistema cliente-servidor básico para realizar operaciones en archivos a través de una red. El servidor maneja múltiples conexiones concurrentes utilizando hilos, mientras que el cliente interactúa con el servidor a través de un menú de opciones, permitiendo al usuario realizar diversas operaciones en los archivos del servidor.
