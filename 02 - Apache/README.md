**Trabajo Práctico de Sockets**

**Mini Cliente Apache**

Aquí tenemos un Mini Cliente Apache ejecutándose en un SO Linux Debian que hace conexión con un Servidor Apache corriendo en un SO Windows XP.

Es muy simple, el Cliente le pide un archivo al Servidor Apache, y este le envía el archivo al Cliente, y el Cliente lo muestra por pantalla.

El alumno tiene que modificar el Cliente para que decodifique el archivo recibido quitando todo lo que corresponde al código html.

Para implementar y probar este ejercicio se deberá instalar y configurar un servidor  web Apache, la recomendación es instalar el Apache en un sistema operativo Windows con el objetivo de que el alumno perciba el funcionamiento de un sistema distribuido cliente servidor.

**Cliente que se ejecuta en un SO Linux Debian**

clienteApache.c

### Explicación

#### Función `eliminarEtiquetasHTML`:
Esta función recorre el contenido recibido y copia solo los caracteres que no están dentro de las etiquetas HTML (delimitadas por `<` y `>`).

#### Modificación en el `main`:

- Se añade una nueva variable `plain_text` para almacenar el texto sin etiquetas HTML.
- Después de recibir la respuesta del servidor, se llama a `eliminarEtiquetasHTML(buf, plain_text)` para procesar el contenido recibido y eliminar las etiquetas HTML.
- Finalmente, se imprime el contenido sin etiquetas HTML.
