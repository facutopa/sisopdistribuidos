# Flujo del Servidor y Cliente

## Flujo del Servidor

### 1. Configuración del Socket
- Se crea un socket para el servidor usando `socket(AF_INET, SOCK_STREAM, 0)`.
- Se configura la dirección y el puerto del servidor (`s_sock`) usando `inet_addr(IP)` y `htons(PUERTO)`.
- Se une el socket a la dirección y puerto configurados usando `bind`.
- Se pone el socket en modo de escucha para aceptar conexiones entrantes usando `listen`.

### 2. Bucle Principal del Servidor
- En un bucle infinito, el servidor espera conexiones entrantes usando `accept`.
- Cuando se acepta una conexión, se llama a `UbicacionDelCliente` para imprimir la información del cliente.
- Luego se llama a `RecibeEnviaComandos` para manejar la comunicación con el cliente.

### 3. Función `RecibeEnviaComandos`
- En esta función, se lee un comando del cliente usando `read`.
- Si se recibe un comando, se duplica el descriptor de archivo de salida estándar (`stdout`) para redirigir la salida del comando al socket del cliente.
- Se ejecuta el comando recibido usando `system`.
- Se restaura el descriptor de archivo de salida estándar.
- Se envía una secuencia especial "\nEND\n" al cliente para indicar el final de la transmisión.
- Se lee el siguiente comando del cliente y se repite el proceso hasta que el cliente cierre la conexión (cuando `read` devuelve 0).

## Flujo del Cliente

### 1. Configuración del Socket
- Se crea un socket para el cliente usando `socket(AF_INET, SOCK_STREAM, 0)`.
- Se configura la dirección y el puerto del servidor (`remoto`) usando `inet_addr(ipServidor)` y `htons(puertoServidor)`.
- Se establece una conexión con el servidor usando `connect`.

### 2. Bucle Principal del Cliente
- En un bucle infinito, el cliente pide al usuario que ingrese un comando usando `fgets`.
- Si el comando ingresado es "exit", el bucle se rompe y el cliente cierra la conexión.
- Si no es "exit", el comando se envía al servidor usando `send`.

### 3. Recepción de Datos del Servidor
- Después de enviar el comando, el cliente entra en un bucle para recibir la respuesta del servidor usando `recv`.
- La respuesta se lee en partes y se imprime en la pantalla.
- El bucle de recepción continúa hasta que se encuentra la secuencia especial "\nEND\n", que indica que el servidor ha terminado de enviar la respuesta.
- Una vez que se recibe la secuencia especial, el bucle de recepción se rompe y el cliente vuelve a solicitar otro comando al usuario.

## Explicación Detallada de los Cambios

### Servidor
- Después de ejecutar el comando con `system(buf)`, el servidor envía la secuencia especial "\nEND\n" al cliente.
- Esto le permite al cliente saber cuándo ha terminado de recibir la salida completa del comando.

### Cliente
- El cliente sigue leyendo datos del servidor hasta encontrar la secuencia especial "\nEND\n".
- Una vez que encuentra la secuencia especial, el cliente sabe que ha recibido toda la respuesta y puede volver a solicitar otro comando al usuario.

## Ejemplo de Flujo Completo

1. El cliente se conecta al servidor.
2. El cliente ingresa un comando, por ejemplo, `cal`.
3. El cliente envía el comando `cal` al servidor.
4. El servidor recibe el comando `cal`, lo ejecuta y envía la salida al cliente, seguida de la secuencia "\nEND\n".
5. El cliente recibe la salida del comando `cal` y sigue leyendo hasta encontrar "\nEND\n".
6. Una vez que el cliente encuentra "\nEND\n", imprime la salida completa del comando y vuelve a solicitar otro comando al usuario.
7. El usuario ingresa otro comando, y el proceso se repite hasta que el usuario ingresa `exit`.

Este flujo asegura que el cliente pueda manejar múltiples comandos y recibir la salida completa de cada comando antes de solicitar un nuevo comando, evitando problemas de truncado o espera indefinida.
