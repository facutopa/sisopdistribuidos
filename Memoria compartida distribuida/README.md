# Tipos de Memoria en Sistemas Distribuidos

## Memoria centralizada y compartida → multiprocesadores
## Memoria distribuida y privada → sistemas distribuidos
## Memoria distribuida y compartida → Distributed Shared Memory (DSM)

### Distributed Shared Memory (DSM)

#### Mediante hardware:
- **Multiprocesadores NUMA (acceso a memoria no uniforme)**
  - Permite el acceso a la memoria remota.
  - El acceso a la memoria remota es más lento que el de la memoria local.

#### Mediante software:
- Incluir software en SD para ofrecer DSM.
- **Memoria Compartida Distribuida**

#### Multiprocesadores NUMA (acceso a memoria no uniforme)

### SHM Distribuida

La principal problemática que se presenta entre dos o más procesos, sean locales o distribuidos, al compartir recursos es que cada proceso tiene su propio espacio de direcciones.

Cuando se trata de procesos locales, al estar físicamente en el mismo hardware el espacio de direcciones, es sencillo compartir memoria. Esto no es sencillo en procesos distribuidos.

En un Sistema Operativo Distribuido, un computador ejecuta los procesos en su propia memoria, pero en caso de necesitar más memoria utilizará los recursos disponibles de otra computadora.

La memoria compartida distribuida ayuda a que no se formen cuellos de botella, facilita el diseño y construcción de sistemas distribuidos.

## Multiprocesadores

### Multiprocesadores con memoria compartida:
- HW más complejo y difícilmente ampliable.
- SW más sencillo.

### Modelo de programación en sistemas con memoria compartida:
- Llamadas a procedimiento (o invocación de métodos).
- Comunicación mediante datos compartidos.
- Sincronización mediante semáforos.

### Modelo de programación tradicional en sistemas distribuidos:
- Paso de mensajes para comunicación y sincronización.

### Modelo de programación en sistemas distribuidos:
- RPC (o RMI) + DSM.
- Multicomputadoras.

### SHM Distribuida

#### Memoria compartida distribuida (DSM):
- Memoria compartida implementada por software.
- Simulada mediante paso de mensajes.
- Comunicación con datos compartidos.

**Objetivo:**
- Sistemas fáciles de construir y programar.
- Especialmente adecuada para aplicaciones paralelas.

**Problema:**
- Para un rendimiento aceptable se requieren múltiples copias en distintos nodos.

### Visión general de la MCD

#### Sistemas con Memoria Compartida Distribuida (DSM)

[Li, 1986] y [Li and Hudak, 1989] fueron los primeros en proponer el paradigma de Memoria Compartida Distribuida. 

Colección de estaciones de trabajo conectadas por una LAN que comparten un solo espacio de direcciones paginado. 

Una referencia a una página causa una falla de página que atrapa el SO. Este último envía un mensaje a la máquina remota, para solicitar la página. 

La comunicación y sincronización se realiza a través de la memoria, la comunicación es transparente para los procesos.

### Estrategias de implementación

#### Memoria basada en circuitos:
- Existe una única área de memoria y cada procesador tiene su propio bus de datos y direcciones (en caso de no tenerlo se vuelve un esquema centralizado).

#### Memoria basada en bus:
- Los procesadores comparten un bus de datos y direcciones, lo que es más barato de implementar.
- Se necesita tener una memoria caché grande y sumamente rápida.

#### Memoria basada en anillos:
- Es más tolerante a fallos.
- No hay coordinador central y se privilegia el uso de la memoria más cercana.

### Modelos de consistencia

La principal problemática de la compartición de memoria en ambientes distribuidos consiste en el manejo de la consistencia, ya que varios procesos distribuidos pueden estar escribiendo en la zona de memoria compartida, pudiendo invalidar el contenido de las lecturas que acaban de hacer algunos procesos.

Para prevenir esta problemática se han planteado muchos mecanismos que permiten evitar la inconsistencia de los datos.

Una forma básica pero costosa es el manejo de replicación.

Se deben considerar el tipo de datos en memoria que se están compartiendo, por ejemplo, páginas, variables, objetos.

#### Modelos de consistencia

Un modelo de consistencia de memoria [Mosberger 1993] especifica las garantías de consistencia que un sistema otorga sobre los valores que los procesos leen de los objetos, dado que en realidad acceden a una réplica de cada objeto y que múltiples procesos pueden actualizar los objetos. Los requisitos de consistencia pueden ser muy variados y dependen de la aplicación.

El principal interrogante que se plantea al caracterizar un modelo de consistencia de memoria es:

**¿Cuándo se realiza un acceso de lectura sobre una posición de memoria, cuáles actualizaciones a la misma posición deben devolverse?**

El valor más reciente, el significado más reciente en SD pudiera no estar claro, no es posible determinar siempre de forma exacta si un evento ocurrió antes que otro.

En los DSM se tienen varias copias de las páginas para lectura y una copia de cada página de escritura.

Cuando una máquina remota llama a una página para escritura, se señala y la página es ocupada.

Si una página para escritura está demasiado compartida y si hay solo una copia, se forma un cuello de botella y baja el rendimiento.

Si se permiten varias copias de las páginas de escritura, aumenta el desempeño, pero se introduce el problema del mantenimiento de la consistencia.

La consistencia perfecta de todas las copias de las páginas de escritura es muy compleja si además las páginas están en distintas máquinas. Recordando que todas las máquinas se comunican por mensajes enviados a través de la red.

Se permite una consistencia menos perfecta, menos compleja pero más aceptable desde el punto de vista del desempeño.

El significado preciso de la consistencia y su relajación sin hacer insoportable la programación es un tema fundamental entre los investigadores de DSM.

#### Consistencia

Un modelo de consistencia es un contrato entre el software y la memoria.

Si el software obedece determinadas reglas, la memoria trabaja en forma correcta. Si el software viola estas reglas, no se garantiza que la operación de memoria sea la correcta.

Existen contratos con restricciones menores y contratos con restricciones que hacen imposible la programación.

Queda claro que los contratos con restricciones menores no obtienen los mismos resultados que los contratos con restricciones mayores.

#### Modelos de consistencia

##### Consistencia Estricta
Este modelo es el más robusto pero sumamente difícil de implementar.

Cualquier lectura a la localidad de memoria x retorna el valor almacenado por la última operación de escritura (antes de la lectura).
- Supone la existencia de un tiempo global. Determinar cuál fue la escritura más reciente no siempre es posible.
- Si hay un cambio en una zona de memoria, todas las lecturas observarán el nuevo valor sin importar en qué tiempo se está haciendo la lectura (con respecto a la escritura) o dónde están localizados los procesos que realizan las operaciones. 
- Modelo de consistencia estricto: cada actualización es vista en forma instantánea por todos los nodos en el sistema. Aunque este sería el ideal para cualquier programador, en la práctica es imposible de implementar y mucho menos en una red de computadores.
- Este modelo asume la existencia de un reloj físico global.
- Es el modelo de consistencia que se utiliza en sistemas centralizados.
- En sistemas distribuidos sin reloj global es imposible en la práctica.

##### Consistencia secuencial
Fue propuesta por Lamport en 1977 y basa su funcionamiento en que la consistencia estricta es prácticamente imposible de implementar en un sistema distribuido y que la experiencia demuestra que un programador puede usar modelos más débiles.

Cuando se ejecutan procesos en paralelo sobre diferentes máquinas, cualquier mezcla de ejecución es un resultado aceptable, no obstante todos los procesos deben ver la misma secuencia de referencias a memoria.

Dos ejecuciones del mismo programa podrían no arrojar el mismo resultado a menos que se utilicen operaciones explícitas de sincronización.

En ciencias de la computación, un algoritmo no determinista es un algoritmo que con la misma entrada ofrece muchos posibles resultados. No se puede saber de antemano cuál será el resultado de la ejecución de un algoritmo no determinista.

#### Consistencia secuencial

Un sistema de consistencia secuencial se puede implementar utilizando un único servidor que administra los datos compartidos.

Todos los procesos envían sus operaciones de lectura y escritura al servidor.
_Ejemplificar_

##### Consistencia Causal

Si un evento B es causado por un evento A, la causalidad requiere que todo el mundo vea primero el evento A y luego el B. Cuando encontramos una lectura seguida por una escritura, los dos eventos están potencialmente relacionados en forma causal. Un read está relacionado causalmente con la escritura que provee el dato que se ha leído. P1 escribe X, P2 lee X, P2 escribe Y (Y puede depender del valor leído de X)

La sincronización del tiempo es complicada en SOD, por este motivo se sugiere que la consistencia de los datos modificados sea de forma causal.

Si un proceso desea leer (read) un dato en memoria, existirá siempre un proceso que haya escrito (write) previamente en memoria. De esta forma siempre se tiene el último valor escrito.

Si dos procesos escriben simultáneamente una variable, estos accesos no están relacionados causalmente. Las operaciones que no están relacionadas causalmente se dice que son concurrentes.

Las escrituras relacionadas causalmente deben ser vistas por todos los procesos en el mismo orden. Las escrituras concurrentes pueden ser vistas en un orden diferente, en diferentes máquinas.
_Ejemplificar_

##### Consistencia PRAM

El modelo Pipelined RAM basa su funcionamiento en que las escrituras realizadas por un proceso son recibidas por el resto en el orden en el cual éstas fueron ejecutadas, no obstante, las escrituras realizadas por diferentes procesos pueden ser vistas en órdenes diferentes por todos ellos.

Ejemplo: Tanto P1 como P2 escriben datos, al mismo tiempo P3 y P4 leen los valores, si P3 depende de P2 el primer valor es el último escrito aunque puede visualizar después el otro valor de P1. Si P4 depende de P1, por eso tiene el primer valor, pero puede acceder al del otro proceso.

P1:  W(x)1                           
P3:  R(x)2  R(x)1
P2:  R(x)1  W(x)2 
P4:  R(x)1  R(x)2

Una sucesión correcta de eventos con consistencia PRAM.
_Ejemplificar_

##### Consistencia Débil

Fue propuesta por Dubois en 1986 y basa su funcionamiento en que los modelos anteriores de consistencia se consideran aún restrictivos porque requieren que las escrituras de un proceso se vean en orden.

Esto no siempre es necesario. Por ejemplo, cuando se está en una región crítica no es necesario propagar valores intermedios sino los valores finales. Para poder implantar este modelo se necesita de una variable de sincronización.

Los accesos a las variables de sincronización son secuencialmente consistentes: todos los procesos ven todos los accesos a las variables de sincronización en el mismo orden.

No se permite el acceso a ninguna variable de sincronización hasta que todas las escrituras previas se hayan completado: hacer una sincronización después de operaciones de escritura obliga a que los nuevos valores se propaguen a todas las memorias.

El hacer una operación de sincronización antes de leer los datos, le garantiza a un proceso que leerá los últimos valores.

Además, la operación de sincronización garantiza que las escrituras locales sean propagadas a todas las otras máquinas y se actualiza la memoria actual con escrituras hechas remotamente.
_Ejemplificar_

### MCD basada en Páginas

La DSM o (MCD) “clásica”, que se construyó en multicomputadoras (procesadores conectados en red con transferencia de mensajes). En este caso, ninguna CPU tiene acceso directo a la memoria de otra CPU (NORMA) en contraste con los sistemas NUMA (acceso no uniforme a memoria).

La diferencia entre NUMA y NORMA es que en el primero cada CPU hace referencia de forma directa a cada palabra en el espacio global de direcciones, sólo leyendo o escribiendo. Las páginas están distribuidas al azar entre las memorias, sin afectar los resultados dados por los programas. Cuando una CPU hace referencia a una página remota, el sistema tiene opción de traerla o usarla de forma remota. La decisión afecta el desempeño pero sigue siendo correcto. En las máquinas NUMA el hardware permite que cada CPU use las palabras del espacio de direcciones sin intervención del software.

Las estaciones en una LAN (NORMA) son diferentes a un multiprocesador (NUMA). Las CPUs sólo pueden hacer uso de su propia memoria. No existe el concepto de memoria compartida global. El objetivo de la DSM es agregar software al sistema para que una multicomputadora ejecute programas de multiprocesador. Cuando una CPU referencia una página remota, debe ser traída. No hay opción.

### Diseño básico

La idea es sencilla: emular el caché de un multiprocesador mediante MMU y el SO. El espacio de direcciones se separa en pedazos dispersos en todos los procesadores. Cuando un CPU referencia a una dirección remota ocurre un señalamiento y el software DSM trae el pedazo que contiene la dirección y reinicia la instrucción suspendida, que puede concluir con éxito. 

Si el CPU 1 referencia los pedazos 0, 2, 5 o 9, lo hace localmente. Las referencias al resto provocan señalamientos. Ej.: si el CPU 1 referencia el pedazo 10 provocará un señalamiento al software DSM, que mueve el pedazo 10 de la computadora 2 a la 1.

#### Réplica

Una mejora es duplicar los pedazos exclusivos para lectura. El uso del pedazo 10 de la figura por parte del CPU1 provoca el envío de una copia sin perturbar el original del CPU2. Así, los CPUs 1 y 2 pueden usar el pedazo 10 con la frecuencia que sea sin causar señalamientos de memoria faltante. Otra posibilidad es duplicar los pedazos no sólo de lectura. Si un pedazo duplicado se modifica, hay que realizar algo para evitar la inconsistencia de las copias.
_Ejemplificar_
