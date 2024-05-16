1\
Procesos y procesadores en sistemas distribuidos\
\
Capítulo 4\
\
Sistemas Operativos Distribuidos\
\
Andrew Tanenbaum\
\
2\
Los procesos son un concepto importante en los sistemas con un procesador, hablaremos de los aspectos del manejo de procesos que a  menudo no se estudian en el contexto de los sistemas operativos clásicos, y  cómo se enfrenta la existencia de muchos procesadores.\
Procesos y procesadores en sistemas distribuidos\
\
3\
En muchos sistemas distribuidos, es posible tener muchos hilos de  control dentro de un proceso. Esto tiene ventajas importantes, pero también tiene desventajas.\
Procesos y procesadores en sistemas distribuidos\
\
4\
Definición de un proceso en los sistemas operativos tradicionales\
\
“Cada proceso tiene un espacio de direcciones y un hilo de control.”\
Sin embargo, con frecuencia existen situaciones en donde se desea tener varios hilos de control que compartan un espacio de direcciones, pero que se ejecutan de manera cuasi paralela, como si fuesen de hecho procesos independientes (excepto por el espacio de direcciones compartido)\
\
5\
Introducción a los hilos\
Un servidor de archivos que debe bloquearse en forma ocasional, en espera de acceso al disco. Si el servidor tiene varios hilos de control, se podría ejecutar un segundo hilo mientras el primero duerme. El resultado sería mejor rendimiento y desempeño.\
Ejemplo\
\
6\
Un proceso que tiene varios hilos de control, los cuales por lo general se llaman sólo hilos o a veces procesos ligeros. En muchos sentidos, los hilos son como pequeños mini procesos. Cada hilo tiene su contador de programa y una pila para llevar un registro de su posición.\
\
7\
Los hilos comparten el CPU, de la misma forma que lo hacen los procesos  (tiempo compartido).\
\
Sólo en un multiprocesador se pueden ejecutar en paralelo.\
\
Los hilos pueden crear hilos hijos.\
\
Los hilos se pueden bloquear en espera de que terminen sus llamadas al sistema.\
\
Mientras un hilo está bloqueado, se puede ejecutar otro hilo del mismo proceso.\
\
8\
Los distintos hilos de un proceso no son tan independientes como los procesos\
\
Todos los hilos tienen el mismo espacio de direcciones.\
\
comparten también las mismas variables globales.\
\
No existe protección entre los hilos debido a que es imposible y no debe ser necesaria.  A diferencia de los procesos, que pueden ser de diversos usuarios y que pueden ser hostiles entre sí. Un proceso siempre le pertenece a un usuario, quien supuestamente ha creado varios hilos para que éstos cooperen y no luchen entre si.\
\
Además de compartir un espacio de direcciones, todos los hilos comparten el mismo conjunto de archivos abiertos, cronómetros y señales.\
\
9\
\
10\
Como los procesos tradicionales, los hilos pueden tener uno de los siguientes estados: en ejecución, bloqueado, listo o terminado. Un hilo en ejecución posee CPU y está activo. Un hilo bloqueado espera que otro elimine el bloqueo (por ejemplo, en un semáforo). Un hilo listo está programado para su ejecución, la cual se llevará a cabo tan pronto le llegue su turno. Por último, un hilo terminado es aquel que ha hecho su salida, pero que todavía no es recogido por su padre (en términos de UNIX, el hilo padre no ha realizado un WAIT).\
ESTADOS DE LOS HILOS\
\
11\
Uso de hilos\
\
Los hilos se inventaron para permitir la combinación del paralelismo con la ejecución secuencial  y el bloqueo de las llamadas al sistema.\
\
Servidor de archivos con hilos.\
\
El servidor, lee las solicitudes de trabajo del buzón del sistema. Después de examinar la solicitud elige a un hilo trabajador inactivo (bloqueado) y le envía la solicitud, lo cual se puede realizar al escribir un apuntador al mensaje en una palabra especial asociada a cada hilo. El servidor despierta entonces al trabajador dormido (por ejemplo, lleva a cabo un UP en el semáforo en donde duerme). Cuando el trabajador despierta, verifica si puede satisfacer la solicitud, envía un mensaje al disco para obtener el bloque necesario (si se trata de un READ) y se duerme en espera de la conclusión de la operación del disco.\
\
12\
EJEMPLIFICAR\
\
13\
El ciclo principal del servidor de archivos obtiene una solicitud, y la resuelve mientras las otras solicitudes esperan tu turno.\
\
Mientras espera al disco, el servidor está inactivo y no procesa otras solicitudes. Si el servidor de archivos se ejecuta en una máquina exclusiva para él, como es lo común, el CPU está inactivo mientras el servidor espera al disco. El resultado neto es que se pueden procesar menos solicitudes por segundo.\
Servidor de archivos sin hilos\
\
14\
Con frecuencia, los hilos también son útiles para los clientes. Por ejemplo, si un cliente desea copiar un archivo en varios servidores, puede hacer que cada hilo se comunique con cada servidor. Otro uso de los hilos clientes es el manejo de señales, como las interrupciones desde el teclado (DEL o BREAK). En vez de dejar que la señal interrumpa al proceso, se dedica un hilo de tiempo completo para la espera de señales. Por lo general, éste se bloquea; pero cuando llega una señal, despierta y la procesa.\
HILOS EN LOS PROCESOS CLIENTES\
EJEMPLIFICAR\
\
15\
Ciertas aplicaciones son más fáciles de programar mediante procesos paralelos; por ejemplo, el problema de los productores y los consumidores. El hecho de que el productor y el consumidor se ejecuten en paralelo es secundario. Se les programa de esa manera debido a que esto hace más simple el diseño del software. Como comparten un buffer común, los hilos se ajustan perfectamente a este caso.\
Fácil diseño de software con hilos\
EJEMPLIFICAR\
\
16\
En un sistema multiprocesador es posible que los hilos de un espacio de direcciones se ejecuten en realidad en paralelo, en varios CPU. Un programa diseñado de manera adecuada y que utilice hilos debe funcionar bien, tanto en un CPU con hilos compartidos, como en un verdadero multiprocesador, por lo que los aspectos del software son casi iguales de cualquier forma.\
Ventajas de Hilos en multiprocesador\
EJEMPLIFICAR\
\
17\
Aspectos del diseño de paquetes de hilos\
Un conjunto de primitivas relacionadas con los hilos (por ejemplo, llamadas a biblioteca) disponibles para los usuarios se llama un paquete de hilos.\
\
18\
HILOS ESTATICOS Y DINAMICOS\
En un diseño estático, se elige el número de hilos al escribir el programa o durante su compilación. Este método es simple, pero inflexible.\
\
En un diseño dinámico se permite la creación y destrucción de los hilos durante la ejecución. La llamada para la creación de hilos determina el  programa principal del hilo (como un apuntador a un procedimiento) la llamada regresa un identificador de hilo, para utilizarlo en las llamadas posteriores relacionadas con el hilo. En este modelo, un proceso se inicia (de manera implícita) con un hilo, pero puede crear el número necesario de ellos y éstos pueden expirar al terminar su tarea.\
EJEMPLIFICAR\
\
19\
Los hilos comparten una memoria común y la utilizan para guardar datos que comparten los distintos hilos, como los buffers en un sistema de productores y consumidores. El acceso a los datos compartidos se programan por lo general mediante regiones críticas, para evitar que varios hilos intenten tener acceso a los mismos datos al mismo tiempo.\
Datos compartidos\
EJEMPLIFICAR\
\
20\
La implantación de las regiones críticas es más fácil si se utilizan los semáforos, monitores u otras construcciones similares. Una técnica de uso común en los paquetes de hilos es el mutex, que es un cierto tipo de semáforo moderado. Un mutex sólo tiene dos estados, cerrado y no cerrado. Se definen dos operaciones de mutex. La primera, LOCK, intenta cerrar el mutex. Si el mutex no está cerrado, LOCK tiene éxito y el mutex se cierra en una acción atómica. Si dos hilos intentan cerrar al mismo mutex en el mismo instante uno de ellos gana y el otro pierde. Si un hilo intenta cerrar un mutex ya cerrado, se bloquea.\
MUTEX\
EJEMPLIFICAR\
\
21\
Otra característica de sincronización que a veces está disponible en los paquetes de hilos es la variable de condición, La diferencia entre el mutex y la variable de condición es que el primero se utiliza para una cerradura a corto plazo, principalmente para proteger la entrada a las regiones criticas. Las variables de condición se utilizan para una espera a largo plazo hasta que un recurso esté disponible.\
\
https\://www\.youtube.com/watch?v=AtB57UpIIos\
\
VARIABLE DE CONDICIÓN\
EJEMPLIFICAR\
\
22\
Nuestro último aspecto del diseño relativo a los hilos es la planificación. Los hilos se pueden planificar mediante distintos algoritmos, entre los que se encuentran por prioridad, round robin y FCFS. Los paquetes de hilos proporcionan a menudo ciertas llamadas para que el usuario pueda especificar el algoritmo de planificación y establecer las prioridades, en su caso.\
PLANIFICACION DE HILOS\
EJEMPLIFICAR\
\
23\
Implementación de un paquete de hilos\
Existen dos métodos principales para implantar un paquete de hilos: en el espacio del usuario y en el espacio del núcleo. También existe una implantación hibrida.\
\
24\
Implantación de los hilos en el espacio del usuario\
El primer método consiste en colocar todo el paquete de hilos en el espacio del usuario. El núcleo no sabe de su existencia. La primera ventaja, es que un paquete de hilos implantado en el espacio del usuario se puede implantar en un sistema operativo que no tiene soporte de hilos. Por ejemplo, UNIX no soporta hilos, pero para él se han escrito distintos paquetes de hilos en el espacio del usuario.\
\
25\
Los hilos a nivel de usuario también tienen otras ventajas. Permiten que cada proceso tenga su algoritmo de planificación adaptado. Por ejemplo, aquellas que cuentan con un hilo recolector de basura, es muy bueno no tener que preocuparse por el hecho de que un hilo se aborte. Tienen mejor escalabilidad, puesto que los hilos del núcleo requieren de manera invariable algún espacio para sus tablas y su pila en el núcleo, lo cual puede ser un problema si existe un número muy grande de hilos.\
Ventajas de los hilos en el espacio del usuario\
\
26\
Implantación de hilos en el núcleo\
Ahora consideremos que el núcleo sabe de la existencia de los hilos y cómo manejarlos. Cuando un hilo desea crear un nuevo hilo o destruir uno existente, hace una llamada al núcleo, el que realiza entonces la creación o la destrucción.\
\
Para controlar todos los hilos, el núcleo tiene una tabla con una entrada por cada hilo, con los registros, estado, prioridades y demás información relativa al hilo. La información es la misma que en el caso de los hilos a nivel usuario, sólo que ahora se encuentra en el espacio del núcleo y no en el espacio del usuario. Esta información también es la misma que los núcleos tradicionales conservan acerca de sus procesos con un hilo, es decir, el estado del proceso.\
\
27\
Todas las llamadas que pueden bloquear un hilo, como la sincronización entre hilos mediante semáforos, se implementan como llamadas al sistema, con un costo considerable. Cuando un hilo se bloquea, el núcleo puede optar por ejecutar otro hilo del mismo proceso (si alguno está listo) o un hilo de un proceso distinto. Con los hilos a nivel de usuario, se mantienen en ejecución los hilos de un proceso hasta que el núcleo le retira el CPU al proceso.\
PLANIFICACIÓN A NIVEL DE NÚCLEO\
\
28\
RECICLAJE DE HILOS A NIVEL DE NUCLEO\
\
Debido a un gran costo de creación y destrucción de hilos en el núcleo, algunos sistemas adoptan el método de reciclar sus hilos.\
\
Al destruir un hilo, se marca como no ejecutable, pero las estructuras de datos de su núcleo no se ven afectadas.\
\
Posteriormente, cuando se debe crear un nuevo hilo, se reactiva un hilo anterior, lo que ahorra ciertos costos.\
\
29\
Hilos y RPC\
\
Es común que los sistemas distribuidos utilicen tanto las RPC como los hilos. Puesto que los hilos se idearon como alternativa menos costosa a los procesos estándar (pesados), es natural que los investigadores analicen las RPC más de cerca en este contexto, para ver si éstas también se pueden aligerar.\
\
\
\
30\
MODELOS DE SISTEMAS\
\
Los procesadores de un sistema distribuido se pueden organizar de varias formas\
\
31\
Uso de estaciones de trabajo inactivas\
\
El primer intento por permitir el uso de las estaciones de trabajo inactivas fue el programa rsh proporcionado por UNIX de Berkeley. Este programa se invoca con rsh machine command.\
\
El primer problema, es que el usuario debe indicar la máquina que desea utilizar, lo que coloca al usuario en el problema de mantener un registro de las máquinas inactivas.\
\
El segundo problema, es que el programa se ejecuta en el ambiente de la máquina remota, el cual es por lo general distinto del ambiente local.\
\
Tercer problema, si alguien debe entrar a una máquina inactiva que ejecuta un proceso remoto, el proceso continúa su ejecución y el nuevo usuario debe aceptar el desempeño menor o encontrar otra máquina.\
\
32\
Uso de estaciones de trabajo inactivas\
\
La investigación en el tema de las estaciones de trabajo inactivas se ha centrado en la solución de estos problemas. Los aspectos claves son:\
\
¿Cómo encontrar una estación de trabajo inactiva?\
\
2\. ¿Cómo lograr que un proceso remoto se ejecute en forma transparente?\
\
3\. ¿Qué ocurre si regresa el poseedor de la máquina?\
\
33\
Primer pregunta\
¿Cómo encontrar una estación de trabajo inactiva?\
\
Los algoritmos que se utilizan para localizar las estaciones de trabajo inactivas se pueden dividir en dos categorías:\
\
\
controlados por el servidor\
\
\
controlados por el cliente.\
\
34\
Cuando una estación de trabajo está inactiva y se convierte en un servidor potencial y anuncia su disponibilidad.  Puede hacer esto al proporcionar su nombre, dirección en la red y propiedades en un archivo de registros (o base de datos).\
\
1\) Cuando un usuario desee ejecutar un comando en una estación de trabajo inactiva, puede escribir remote command y el programa remote busca en el registro una estación de trabajo adecuada.\
\
2\) Otra alternativa consiste en que la estación inactiva anuncie el hecho de que no tiene trabajo, al colocar un mensaje que se transmite en toda la red.\
Todas las demás estaciones registran esto.\
Cada máquina tiene su copia del registro.\
La ventaja es: Menor costo en la búsqueda de una estación de trabajo.\
La desventaja es: Todas las máquinas se encargan de mantener el registro de máquinas inactivas.\
Controlados por el servidor\
\
35\
\
36\
Controlados por el cliente\
\
Al llamar a remote, el cliente transmite una solicitud donde indica el programa que desea ejecutar, la cantidad de memoria necesaria, si requiere o no un circuito coprocesador de punto flotante, etc.\
\
Estos detalles no son necesarios si todas las estaciones de trabajo son idénticas, pero son esenciales si el sistema es heterogéneo y no todos los programas se pueden ejecutar en todas las estaciones. Al regresar la respuesta, remote elige una de ellas. Un cambio interesante es que las estaciones "inactivas" retrasen un poco sus respuestas, con un retraso proporcional a la carga actual. De esta forma, la respuesta de la máquina con menor carga llega primero y se selecciona.\
\
37\
DESPLAZAMIENTO DEL CÓDIGO\
\
La búsqueda de la estación de trabajo es sólo el primer paso. Ahora hay que ejecutar el proceso ahí. El desplazamiento del código es fácil. El truco consiste en configurar el proceso remoto de modo que vea el mismo ambiente que tendría en forma local en la estación de trabajo de origen y llevar a cabo el cómputo de la misma forma que de forma local.\
\
Necesita la misma visión del sistema de archivos, el mismo directorio de trabajo y las mismas variables de ambiente del shell.\
\
Ejemplificar\
\
38\
Algunas de las Llamadas al sistema se deben regresar a la máquina de origen sin hacer distinción alguna.\
\
Las Llamadas al sistema relacionadas con el tiempo son un problema, puesto que los relojes de las diversas máquinas no estan sincronizados. El uso del tiempo de la máquina remota provoca que los programas dependientes del mismo, como make, proporcionen resultados incorrectos.\
Sin embargo, si todas las llamadas relacionadas con el tiempo hacen referencia al tiempo de la máquina de origen, se introduce un retraso, lo que también provoca problemas con el tiempo.\
Segunda Pregunta\
¿Qué sucede con las Llamadas al sistema que hace el proceso?\
\
39\
Tercer Pregunta\
¿Qué ocurre si regresa el poseedor de la máquina?\
\
La última pregunta de nuestra lista original es ¿Qué hacer si regresa el poseedor de la máquina (es decir, alguien entra al sistema o un usuario previamente inactivo toca el teclado o el ratón)?\
Lo más fácil es no hacer nada, pero esto tiende a destruir la idea de las estaciones de trabajo "personales".\
Si otras personas intentan ejecutar programas en la estación de trabajo al mismo tiempo en que usted desea utilizarla, no hay respuesta garantizada.\
\
40\
Otra posibilidad es\
\
Eliminar el proceso de  manera abrupta y sin previo aviso. La desventaja de esta estrategia es que se perderá todo el trabajo y el sistema de archivos puede quedar en estado inconsistente. Es mejor darle al proceso una advertencia y escribir los buffers editados en un disco, cerrar archivos, y si el proceso no retorna después de unos segundos, hay que terminarlo. Por supuesto, hay que escribir el programa de modo que pueda manejar una señal de terminación, algo que no hacen la mayoría de los programas existentes.\
\
Ejemplificar señales\
\
41\
Hacer que el proceso emigre a otra máquina, ya sea a la máquina de origen o a alguna otra estación de trabajo inactiva. La migración se realiza poco en la práctica, puesto que el mecanismo real es complejo. La parte difícil no es mover el código y los datos del usuario, sino encontrar y recolectar todas las estructuras de datos del núcleo relativas al proceso de salida. Por ejemplo, podría tener archivos abiertos, ejecutar cronómetros, tener una cola de mensajes recibidos, y otras partes de información diseminadas en el núcleo. Todo esto debe eliminarse con cuidado de la máquina fuente e instalarse con éxito en la máquina destino. No existen problemas teóricos en este aspecto,\
pero las dificultades prácticas de ingeniería  de software son sustanciales.\
Otra posibilidad es\
\
42\
En todos los casos, al irse el proceso, debe dejar la máquina en el mismo estado en que la encontró, para evitar molestar al poseedor. Entre otros aspectos, esto significa que el proceso no sólo debe irse, sino también sus hijos y los hijos de éstos. Además, hay que eliminar los buzones de correo, conexiones en la red y otras estructuras de datos relativas a todo el sistema. Tomar todas las previsiones necesarias para ignorar las respuestas y mensajes que lleguen al proceso después de haberse ido. Si existe un disco local, hay que eliminar los archivos temporales y, de ser posible, restaurar los archivos que hayan sido eliminados de su caché.\
Dejar la máquina como estaba\
\
43\
El modelo de la pila de procesadores\
Otro método consiste en construir una pila de procesadores, repleta de CPU, los cuales se pueden asignar de manera dinámica a los usuarios según la demanda.\
Los usuarios obtienen tantos CPU como sea necesario, durante períodos cortos, después de lo cual regresan a la pila, de modo que otros usuarios puedan disponer de ellos. En este caso no existe el concepto de propiedad, todos los procesadores pertenecen por igual a todos.\
\
44\
Un modelo híbrido\
Se puede establecer una indicación al proporcionar a cada usuario una estación de trabajo personal y además tener una pila de procesadores. Aunque esta solución es más cara que cualquiera de los dos modelos puros, combina las ventajas de ambos.\
\
El trabajo interactivo se puede llevar a cabo en las estaciones de trabajo, con una respuesta garantizada. Sin embargo, en las estaciones inactivas no se utiliza el trabajo interactivo, lo cual hace más sencillo el diseño del sistema.\
Todos los procesos no interactivos se ejecutan en la pila de procesadores, así como todo el cómputo pesado en general. Este modelo proporciona una respuesta interactiva más rápida, un uso eficiente de los recursos y un diseño sencillo.\
\
45\
 ASIGNACIÓN  DE PROCESADORES\
\
46\
Por definición, un sistema distribuido consta de varios procesadores. Éstos se pueden organizar como una colección de estaciones de trabajo personales, una pila pública de procesadores o alguna forma híbrida. En todos los casos, se necesitan ciertos algoritmos para decidir cuál proceso hay que ejecutar y en qué máquina.\
\
Para el modelo de estaciones de trabajo.\
La pregunta es cuándo ejecutar el proceso de manera local y cuándo buscar una estación inactiva.?\
\
Para el modelo de la pila de procesadores.\
Hay que tomar una decisión de asignación de procesador por cada nuevo proceso.\
 ASIGNACIÓN  DE PROCESADORES\
\
47\
Modelos de asignación\
Casi todo el trabajo en esta área supone que todas las máquinas son idénticas,\
y que difieren en la velocidad de procesamiento.\
\
Casi todos los modelos publicados suponen que el sistema está por completo interconectado, es decir, que cada procesador se puede comunicar con los demás.\
\
Se genera un nuevo trabajo cuando un proceso en ejecución crea un subproceso. En ciertos casos, el proceso que se bifurca es el intérprete de comandos (shell) que inicia un nuevo trabajo en respuesta a un comando del usuario. En otros, el propio proceso usuario crea uno o más hijos, por ejemplo, para tener mejor desempeño con la ejecución en paralelo de todos los hijos.\
\
48\
Estrategia de asignación de procesadores no migratoria\
Al crearse un proceso, se toma una decisión acerca de dónde colocarlo. Una vez colocado en una máquina, el proceso permanece ahí hasta que termina. No se puede mover, no importa lo sobrecargada de la máquina ni que existan muchas otras máquinas inactivas.\
\
49\
Estrategia de asignación de procesadores migratoria\
Un proceso se puede trasladar aunque haya iniciado su ejecución. Mientras que las estrategias migratorias permiten un mejor balance de la carga, son extremadamente complejas.\
\
50\
Aspectos del diseño de algoritmos de asignación de procesadores\
Algoritmos deterministas vs. heurísticos.\
\
Algoritmos centralizados vs. distribuidos.\
\
Algoritmos locales vs. globales.\
\
51\
Los algoritmos deterministas son adecuados cuando se sabe de antemano todo acerca del comportamiento de los procesos. Deberíamos tener una lista completa de todos los procesos, sus necesidades de cómputo, de archivos y de comunicación. Con esta información, es posible hacer una asignación perfecta. En teoría, uno podría intentar todas las posibles asignaciones y tomar la mejor.\
Algoritmos Deterministas\
\
52\
En el otro extremo están los sistemas donde la carga es por completo impredecible.  Las solicitudes de trabajo dependen de quién esté haciendo qué, y puede variar de manera drástica cada hora, e incluso cada minuto. La asignación de procesadores en tales sistemas no se puede hacer de manera determinista o matemática, sino que utiliza técnicas heurísticas.\
Algoritmos heurísticos\
\
53\
El segundo aspecto del diseño es centralizado vs. distribuido.\
\
\
La recolección de toda la información en un lugar permite tomar una mejor decisión, pero coloca una carga pesada en la máquina central.\
\
Son preferibles los algoritmos descentralizados, pero se han propuesto algunos algoritmos centralizados por la carencia de alternativas descentralizadas adecuadas.\
\
54\
Una escuela de pensamiento\
Prefiere un algoritmo (local) sencillo, si la carga de la máquina está por debajo de cierta marca, se conserva al nuevo proceso, en caso contrario, se deshace de él.\
\
Otra escuela\
Es mejor recolectar información (global) acerca de la carga antes de decidir si la máquina local está o no muy ocupada para otro proceso.\
\
Cada una de estas opciones\
Tiene sus puntos a favor. Los algoritmos locales son sencillos, pero están muy lejos de ser los óptimos, mientras que los globales sólo dan un resultado poco mejor a un costo mayor.\
\
Fin de la presentación de hilos\
Y asignación de estaciones de trabajo y\
Pila de procesadores.\
\
55
