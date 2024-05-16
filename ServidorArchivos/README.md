1\
Sistemas distribuidos de archivos\
\
2\
Sistemas distribuidos de archivos\
El sistema de archivos en los sistemas distribuidos  almacena los programas y los datos y los tiene disponibles cuando sea necesario.\
\
Los aspectos de los sistemas distribuidos de archivos son similares a los de los sistemas convencionales.\
\
3\
Es importante distinguir entre los conceptos de\
servicio de archivos\
y\
servidor de archivos.\
El servicio de archivos es la especificación de los servicios que el sistema de archivos ofrece a sus clientes. Describe las primitivas disponibles, los parámetros que utilizan y las acciones que llevan a cabo.\
\
OPEN, READ, WRITE,  CLOSE, etc.\
\
Especifica la interfaz del sistema de archivos con los clientes.\
EJEMPLIFICAR\
\
4\
Un servidor de archivos\
\
Es un proceso que se ejecuta en alguna máquina y ayuda a implantar el servicio de archivos. Un sistema puede tener uno o varios servidores de archivos, pero los clientes no deben conocer el número de servidores de archivos ni su posición.\
Los clientes saben que al llamar a los servicios de archivos, el trabajo se lleva a cabo de alguna manera y se obtienen los resultados pedidos.\
Los clientes no deben saber que el servicio de archivos es distribuido.\
Lo ideal es que se vea como un sistema de archivos convencional de un procesador.\
\
5\
Un servidor de archivos es por lo general un proceso del usuario\
que se ejecuta en una máquina, un sistema puede contener varios servidores de archivos, cada uno de los cuales ofrece un servicio de archivos distinto.\
Un sistema distribuido podría tener dos servidores de archivos que ofrezcan el servicio de archivos en UNlX y el servicio de archivos en MS-DOS, donde cada proceso de usuario utilizaría el servidor apropiado.\
Un servidor de archivos\
\
6\
DISEÑO DE LOS SISTEMAS DISTRIBUIDOS DE ARCHIVOS\
Un sistema distribuido de archivos tiene dos componentes\
\
Servicio de archivos\
\
Servicio de directorios\
\
7\
Interfaz del servicio de archivos\
¿Qué es un archivo?\
Un archivo es una secuencia de bytes sin interpretación alguna. El significado y estructura de la información en los archivos queda a cargo de los programas de aplicación, esto no le interesa al sistema operativo.\
\
8\
Los servicios de archivos se dividen en dos tipos\
\
\
Modelo carga y descarga\
\
y\
\
Modelo de acceso remoto\
\
9\
\
Modelo carga y descarga\
\
El servicio de archivos proporciona dos operaciones\
\
La lectura de un archivo completo.\
\
La escritura de un archivo completo.\
\
La primera operación transfiere todo un archivo de uno de los servidores de archivos al cliente solicitante. La segunda operación transfiere todo un archivo en sentido contrario, del cliente al servidor.\
Los programas de aplicación buscan los archivos que necesitan y después los utilizan de manera local.\
La ventaja es la sencillez.\
EJEMPLIFICAR\
\
10\
La desventaja es que el cliente debe disponer de un espacio suficiente de almacenamiento para todos los archivos necesarios.\
\
Además, si sólo se necesita una pequeña parte de un archivo, el traslado del archivo completo es un desperdicio.\
Modelo carga y descarga\
\
11\
\
12\
Modelo de acceso remoto\
El servicio de archivos proporciona gran número de operaciones para abrir, cerrar archivos, leer y escribir partes de archivos, moverse a través de un archivo (LSEEK), examinar y modificar los atributos de archivo.\
Su ventaja es que no necesita mucho espacio por parte de los clientes, a la vez que elimina la necesidad de transferir archivos completos cuando sólo se necesita una pequeña parte de ellos.\
EJEMPLIFICAR\
\
13\
La interfaz del servidor de directorios\
La naturaleza del servicio de directorios no depende del hecho de que los archivos individuales se transfieran en su totalidad o que se tenga un acceso remoto a ellos.\
\
14\
Servidor de directorios\
\
Transparencia con respecto a la posición\
\
Un sistema donde los archivos se pueden desplazar sin que cambien sus nombres tiene independencia con respecto a la posición.\
\
Un sistema distribuido que incluya los nombres de la máquina o el servidor en los nombres de las rutas de acceso no es independiente con respecto a la posición.\
\
La independencia con respecto a la posición no es fácil de lograr, pero es una propiedad deseable en un sistema distribuido.\
\
15\
Existen tres métodos usuales para nombrar los archivos y directorios en un sistema distribuido:\
\
Nombre máquina + ruta de acceso, como /máquina/ruta o máquina:ruta.\
\
2\. Montaje de sistemas de archivos remotos en la jerarquía local de archivos.\
\
Nombrar archivos en un sistema distribuido\
\
16\
En un sistema distribuido, existe la posibilidad de buscar un nombre en ASCII y obtener no uno, sino varios nombres (nodos-i). Por lo general, éstos representan al archivo original y todos sus respaldos. Con varios nombres, es posible entonces intentar la localización de uno de los archivos correspondientes, si éste no está disponible por alguna razón, se intenta con los otros. Este método proporciona cierto grado de tolerancia.\
Tolerancia de fallas por medio de la redundancia\
\
17\
Semántica de los archivos compartidos\
Si dos o más usuarios comparten el mismo archivo, es necesario definir con precisión la semántica de la lectura y escritura para evitar problemas. En los sistemas con un procesador que permiten a los procesos compartir archivos, como UNIX la semántica establece por lo general que si una operación READ sigue después de una operación WRITE, entonces READ regresa el valor recién escrito.\
De manera análoga, cuando dos WRITE se realizan en serie y después se ejecuta un READ, el valor que se lee es el almacenado en la última escritura. De hecho, el sistema impone en todas las operaciones un orden absoluto con respecto del tiempo y siempre regresa el valor más reciente. Nos referiremos a este modelo como la semántica de UNIX. Este modelo es fácil de comprender y tiene una implantación directa.\
\
18\
En un sistema distribuido,  la semántica de UNIX se puede lograr fácilmente, mientras sólo exista un servidor de archivos.\
Todas las instrucciones READ y WRlTE pasan en forma directa al servidor de archivos, que los procesa en forma secuencial, este método proporciona la semántica de UNIX (excepto por un problema menor: los retrasos en la red pueden hacer que un READ ocurrido un microsegundo después de un WRlTE llegue primero al servidor y que obtenga el valor anterior).\
Semántica de UNIX\
18\
\
19\
Tener una regla que diga: "los cambios a un archivo abierto sólo pueden ser vistos en un principio por el proceso que modificó el archivo. Los cambios serán visibles a los demás procesos sólo cuando se cierre el archivo".\
\
Cuando un proceso cierra el archivo, envía una copia al servidor, de modo que los siguientes READ obtienen el nuevo valor.\
Semántica de sesión.\
\
20\
Archivos inmutables (No se pueden modificar)\
\
Un método por completo distinto a la semántica de los archivos compartidos en un sistema distribuido es que todos los archivos sean inmutables. Así, no existe forma de abrir un archivo para escribir en él. Las únicas operaciones en los archivos son CREATE y READ. Lo que es posible es crear un archivo por completo nuevo e introducirlo en el sistema de directorios. Así, se vuelve imposible modificar el archivo, pero es posible remplazarlo en forma atómica por un archivo nuevo. Una vez que hemos decidido que los archivos no se pueden modificar, desaparece el problema.\
\
21\
Transacciones atómicas (Concepto de Transaction)\
\
Otro método para evitar usar archivos compartidos en un sistema distribuido es usar las transacciones atómicas.  Para tener acceso a un archivo,  un proceso ejecuta una primitiva BEGIN TRANSACTION para señalar que lo que sigue debe ejecutarse de manera indivisible. Después vienen las llamadas al sistema para leer o escribir, al terminar el trabajo, se ejecuta una primitiva END TRANSACTION.\
Lo fundamental de este método es que el sistema garantiza que todas las llamadas contenidas dentro de la transacción se llevarán a cabo en orden, sin interferencias de otras transacciones concurrentes.  \
\
22\
Cuatro métodos para utilizar archivos compartidos en un sistema distribuido.\
22\
\
23\
Uso de archivos\
IMPLANTACIÓN DE UN SISTEMA DISTRIBUIDO DE ARCHIVOS\
23\
\
24\
IMPLANTACIÓN DE UN SISTEMA DISTRIBUIDO DE ARCHIVOS\
Punto de vista del Sistema Operativo\
¿Son distintos los clientes de los servidores?\
\
Es sorprendente el hecho de que no exista acuerdo en este tema.\
\
25\
No existe distinción alguna entre un cliente y un servidor. Todas las máquinas ejecutan el mismo software básico, de modo que una máquina que desee dar servicio de archivos es libre de hacerlo. Este ofrecimiento del servicio de archivos consiste sólo en exportar los nombres de los directorios seleccionados, de modo que otras máquinas puedan tener acceso a ellos.\
\
El servidor de archivos y el de directorios son programas del usuario, por lo que se puede configurar un sistema para que ejecute o no el software de cliente o servidor en la misma máquina, como se desee.\
\
Los clientes y los servidores son máquinas distintas, ya sea en hardware o software. Aunque la separación de funciones es un poco más transparente, no existe razón fundamental para preferir un método por encima de los demás.\
\
\
26\
Forma de estructurar el servicio de archivos y directorios.\
Combinar ambos en un servidor, que maneje todas las llamadas directorios y archivos.\
\
2\) Separarlos, la apertura de un archivo exige ir hasta el servidor de directorios para asociar su nombre simbólico con el nombre (máquina + nodo-i) y después ir hasta el servidor de archivos con el nombre para llevar a cabo la lectura o escritura real del archivo.\
\
Por ejemplo, se puede implantar un servidor de directorios en MS-DOS y otro servidor de directorios en UNlX donde ambos utilicen el mismo servidor de archivos para el almacenamiento físico.\
\
Desventaja: El hecho de contar con dos servidores requiere de mayor comunicación.\
\
27\
Consideremos el caso de servidores de archivos y directorios independientes. En el caso normal, el cliente envía un nombre simbólico al servidor de directorios, que a su vez regresa el nombre que se utiliza para acceder al servidor de archivos.\
Servidores de archivos y directorios independientes\
\
28\
Los servidores de archivos o directorios deben contener la información de estado de los clientes.\
Una escuela piensa que los servidores no deben contener los estados, es decir, servidor sin estado. En otras palabras, cuando un cliente envía una solicitud a un servidor, éste la lleva a cabo, envía la respuesta y elimina de sus tablas internas toda la información relativa a dicha solicitud. El servidor no guarda información alguna relativa a los clientes entre las solicitudes.\
\
La otra escuela de pensamiento sostiene que es correcto que los servidores conserven información de estado de los clientes entre las solicitudes. Después de todo, los sistemas operativos centralizados mantienen la información de estado de los procesos activos.\
\
Ejemplificar\
\
\
29\
Ocultamiento\
En un sistema cliente-servidor, cada uno con su memoria principal y un disco, existen cuatro lugares donde se pueden almacenar los archivos o partes de archivos: el disco del servidor, la memoria principal del servidor, el disco del cliente o la memoria principal del cliente. EJEMPLIFICAR\
\
30\
Réplica\
Con frecuencia, los sistemas distribuidos de archivos proporcionan la réplica de archivos como servicio a sus clientes.  Se dispone de varias copias de algunos archivos, donde cada copia está en un servidor de archivos independiente.\
Las razones principales son:\
EJEMPLIFICAR\
1\. Aumentar la confiabilidad al disponer de respaldos independientes de cada archivo. Si un servidor falla o se pierde en forma permanente, no se pierden los datos. Mejoramiento de la confiabilidad y la disponibilidad\
\
2\. Permitir el acceso al archivo aunque falle un servidor de archivos. El tema aquí  es: El espectáculo debe continuar. Mejoramiento de la confiabilidad y la disponibilidad\
\
3\. Repartir la carga de trabajo entre varios servidores. Al crecer el tamaño del sistema, el hecho de tener todos los archivos en un procesador se puede convertir en un cuello de botella. Con varios archivos duplicados en dos o más servidores, se puede utilizar el que tenga menor carga. Mejora el desempeño.\
\
31\
El programador controla todo el proceso. Cuando un proceso crea un archivo, lo hace en un servidor específico. Entonces puede crear copias adicionales en otros servidores. Si el servidor de directorios permite varias copias de un archivo, las direcciones en la red de todas las copias se pueden asociar con el nombre del archivo de modo que cuando se busque el nombre, se encuentren todas las copias. Cuando el archivo se abre de nuevo, se pueden buscar las copias de manera secuencial en cierto orden, hasta encontrar una disponible.\
\
Ejemplificar\
Replica explicita\
\
32\
En este caso, sólo se crea una copia de cada archivo en un servidor. Más tarde, el propio servidor crea réplicas en otros servidores en forma automática, sin el conocimiento del programador. El sistema debe ser lo bastante hábil como para recuperar alguna de estas copias en caso necesario. Al crear copias secundarias de esta manera, es importante poner atención en el hecho de que el archivo podría cambiar antes de que se hagan las copias.\
\
Ejemplificar\
Replica retrasada\
\
33\
Todas las llamadas WRlTE al sistema se transmiten en forma simultánea a todos los servidores a la vez, por lo que las copias adicionales se hacen al mismo tiempo que el original. Existen dos diferencias fundamentales entre la réplica retrasada y el uso de un grupo. En primer lugar, con la réplica retrasada, se direcciona un servidor y no un grupo. En segundo lugar, la réplica retrasada ocurre en un plano secundario, cuando el servidor tiene cierto tiempo libre, mientras que con el uso de la comunicación en grupo, todas las copias se crean al mismo tiempo.\
\
Ejemplificar\
Replica en grupos\
\
34\
Lecciones aprendidas\
1\) Si se tiene la opción de hacer algo en una estación de trabajo o en un servidor, hay que elegir la estación de trabajo, puesto que los ciclos del servidor son costosos y los ciclos de la estación de trabajo no lo son.\
2\) Deben utilizarse los caches. Con frecuencia ahorran gran cantidad de tiempo de cómputo y ancho de banda de la red.\
3\) En un sistema UNIX cerca de un tercio de todas las referencias a archivos son a archivos temporales, los cuales tienen tiempos de vida cortos y nunca son compartidos. Si se tratan estos archivos de manera especial se puede mejorar en mucho el desempeño.\
\
Existe otra escuela de pensamiento que opina: "Establecé un mecanismo y apégate a él. No tengas cinco vías para hacer lo mismo". El punto de vista que uno adopte depende de si se prefiere la eficiencia o la sencillez.
