# Sistemas distribuidos de archivos

## Sistemas distribuidos de archivos

El sistema de archivos en los sistemas distribuidos almacena los programas y los datos y los tiene disponibles cuando sea necesario.

Los aspectos de los sistemas distribuidos de archivos son similares a los de los sistemas convencionales.

### Servicio de archivos y servidor de archivos

Es importante distinguir entre los conceptos de **servicio de archivos** y **servidor de archivos**. 

El **servicio de archivos** es la especificación de los servicios que el sistema de archivos ofrece a sus clientes. Describe las primitivas disponibles, los parámetros que utilizan y las acciones que llevan a cabo.
- OPEN, READ, WRITE, CLOSE, etc.

Especifica la interfaz del sistema de archivos con los clientes.
_Ejemplificar_

#### Un servidor de archivos

Es un proceso que se ejecuta en alguna máquina y ayuda a implantar el servicio de archivos. Un sistema puede tener uno o varios servidores de archivos, pero los clientes no deben conocer el número de servidores de archivos ni su posición. Los clientes saben que al llamar a los servicios de archivos, el trabajo se lleva a cabo de alguna manera y se obtienen los resultados pedidos. Los clientes no deben saber que el servicio de archivos es distribuido. Lo ideal es que se vea como un sistema de archivos convencional de un procesador.

Un servidor de archivos es por lo general un proceso del usuario que se ejecuta en una máquina. Un sistema puede contener varios servidores de archivos, cada uno de los cuales ofrece un servicio de archivos distinto. Un sistema distribuido podría tener dos servidores de archivos que ofrezcan el servicio de archivos en UNIX y el servicio de archivos en MS-DOS, donde cada proceso de usuario utilizaría el servidor apropiado.

### Diseño de los sistemas distribuidos de archivos

Un sistema distribuido de archivos tiene dos componentes:
- Servicio de archivos
- Servicio de directorios

#### Interfaz del servicio de archivos

**¿Qué es un archivo?**
Un archivo es una secuencia de bytes sin interpretación alguna. El significado y estructura de la información en los archivos queda a cargo de los programas de aplicación, esto no le interesa al sistema operativo.

Los servicios de archivos se dividen en dos tipos:
- Modelo carga y descarga
- Modelo de acceso remoto

##### Modelo carga y descarga

El servicio de archivos proporciona dos operaciones:
1. La lectura de un archivo completo.
2. La escritura de un archivo completo.

La primera operación transfiere todo un archivo de uno de los servidores de archivos al cliente solicitante. La segunda operación transfiere todo un archivo en sentido contrario, del cliente al servidor. Los programas de aplicación buscan los archivos que necesitan y después los utilizan de manera local. La ventaja es la sencillez.
_Ejemplificar_

La desventaja es que el cliente debe disponer de un espacio suficiente de almacenamiento para todos los archivos necesarios. Además, si sólo se necesita una pequeña parte de un archivo, el traslado del archivo completo es un desperdicio.

##### Modelo de acceso remoto

El servicio de archivos proporciona gran número de operaciones para abrir, cerrar archivos, leer y escribir partes de archivos, moverse a través de un archivo (LSEEK), examinar y modificar los atributos de archivo. Su ventaja es que no necesita mucho espacio por parte de los clientes, a la vez que elimina la necesidad de transferir archivos completos cuando sólo se necesita una pequeña parte de ellos.
_Ejemplificar_

#### La interfaz del servidor de directorios

La naturaleza del servicio de directorios no depende del hecho de que los archivos individuales se transfieran en su totalidad o que se tenga un acceso remoto a ellos.

**Transparencia con respecto a la posición**

Un sistema donde los archivos se pueden desplazar sin que cambien sus nombres tiene independencia con respecto a la posición. Un sistema distribuido que incluya los nombres de la máquina o el servidor en los nombres de las rutas de acceso no es independiente con respecto a la posición. La independencia con respecto a la posición no es fácil de lograr, pero es una propiedad deseable en un sistema distribuido.

Existen tres métodos usuales para nombrar los archivos y directorios en un sistema distribuido:
1. Nombre máquina + ruta de acceso, como /máquina/ruta o máquina:ruta.
2. Montaje de sistemas de archivos remotos en la jerarquía local de archivos.
3. Nombrar archivos en un sistema distribuido

En un sistema distribuido, existe la posibilidad de buscar un nombre en ASCII y obtener no uno, sino varios nombres (nodos-i). Por lo general, éstos representan al archivo original y todos sus respaldos. Con varios nombres, es posible entonces intentar la localización de uno de los archivos correspondientes, si éste no está disponible por alguna razón, se intenta con los otros. Este método proporciona cierto grado de tolerancia.
- **Tolerancia de fallas por medio de la redundancia**

### Semántica de los archivos compartidos

Si dos o más usuarios comparten el mismo archivo, es necesario definir con precisión la semántica de la lectura y escritura para evitar problemas. En los sistemas con un procesador que permiten a los procesos compartir archivos, como UNIX la semántica establece por lo general que si una operación READ sigue después de una operación WRITE, entonces READ regresa el valor recién escrito. De manera análoga, cuando dos WRITE se realizan en serie y después se ejecuta un READ, el valor que se lee es el almacenado en la última escritura. De hecho, el sistema impone en todas las operaciones un orden absoluto con respecto del tiempo y siempre regresa el valor más reciente. Nos referiremos a este modelo como la **semántica de UNIX**. Este modelo es fácil de comprender y tiene una implantación directa.

En un sistema distribuido, la semántica de UNIX se puede lograr fácilmente, mientras sólo exista un servidor de archivos. Todas las instrucciones READ y WRITE pasan en forma directa al servidor de archivos, que los procesa en forma secuencial, este método proporciona la semántica de UNIX (excepto por un problema menor: los retrasos en la red pueden hacer que un READ ocurrido un microsegundo después de un WRITE llegue primero al servidor y que obtenga el valor anterior).

Tener una regla que diga: "los cambios a un archivo abierto sólo pueden ser vistos en un principio por el proceso que modificó el archivo. Los cambios serán visibles a los demás procesos sólo cuando se cierre el archivo". Cuando un proceso cierra el archivo, envía una copia al servidor, de modo que los siguientes READ obtienen el nuevo valor.
- **Semántica de sesión.**

#### Archivos inmutables (No se pueden modificar)

Un método por completo distinto a la semántica de los archivos compartidos en un sistema distribuido es que todos los archivos sean inmutables. Así, no existe forma de abrir un archivo para escribir en él. Las únicas operaciones en los archivos son CREATE y READ. Lo que es posible es crear un archivo por completo nuevo e introducirlo en el sistema de directorios. Así, se vuelve imposible modificar el archivo, pero es posible remplazarlo en forma atómica por un archivo nuevo. Una vez que hemos decidido que los archivos no se pueden modificar, desaparece el problema.

#### Transacciones atómicas (Concepto de Transaction)

Otro método para evitar usar archivos compartidos en un sistema distribuido es usar las transacciones atómicas. Para tener acceso a un archivo, un proceso ejecuta una primitiva BEGIN TRANSACTION para señalar que lo que sigue debe ejecutarse de manera indivisible. Después vienen las llamadas al sistema para leer o escribir, al terminar el trabajo, se ejecuta una primitiva END TRANSACTION. Lo fundamental de este método es que el sistema garantiza que todas las llamadas contenidas dentro de la transacción se llevarán a cabo en orden, sin interferencias de otras transacciones concurrentes.

### Cuatro métodos para utilizar archivos compartidos en un sistema distribuido

### Uso de archivos

**Implantación de un sistema distribuido de archivos**

#### Implantación de un sistema distribuido de archivos

**Punto de vista del Sistema Operativo**
¿Son distintos los clientes de los servidores?

Es sorprendente el hecho de que no exista acuerdo en este tema.

No existe distinción alguna entre un cliente y un servidor. Todas las máquinas ejecutan el mismo software básico, de modo que una máquina que desee dar servicio de archivos es libre de hacerlo. Este ofrecimiento del servicio de archivos consiste sólo en exportar los nombres de los directorios seleccionados, de modo que otras máquinas puedan tener acceso a ellos.

El servidor de archivos y el de directorios son programas del usuario, por lo que se puede configurar un sistema para que ejecute o no el software de cliente o servidor en la misma máquina, como se desee.

Los clientes y los servidores son máquinas distintas, ya sea en hardware o software. Aunque la separación de funciones es un poco más transparente, no existe razón fundamental para preferir un método por encima de los demás.

#### Forma de estructurar el servicio de archivos y directorios

1. Combinar ambos en un servidor, que maneje todas las llamadas directorios y archivos.
2. Separarlos, la apertura de un archivo exige ir hasta el servidor de directorios para asociar su nombre simbólico con el nombre (máquina + nodo-i) y después ir hasta el servidor de archivos con el nombre para llevar a cabo la lectura o escritura real del archivo.

Por ejemplo, se puede implantar un servidor de directorios en MS-DOS y otro servidor de directorios en UNIX donde ambos utilicen el mismo servidor de archivos para el almacenamiento físico.
- **Desventaja**: El hecho de contar con dos servidores requiere de mayor comunicación.

Consideremos el caso de servidores de archivos y directorios independientes. En el caso normal, el cliente envía un nombre simbólico al servidor de directorios, que a su vez regresa el nombre que se utiliza para acceder al servidor
