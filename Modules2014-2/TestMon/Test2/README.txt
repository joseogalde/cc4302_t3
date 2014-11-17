Este directorio contiene el 2do. test para su tarea.

Invoque:

% ./testmon2 /dev/monitor 1000 20

La salida de este test es mas o menos la siguiente:

==============================================
monitor=/dev/monitor, itemes por productor=1000, numero de productores=20
inicializando el monitor con: ./makebuf /dev/monitor 2
Creando 20 procesos productores: 3450 3451 3452 3453 3454 3455 3456 3457 3458 3459 3460 3461 3462 3463 3464 3465 3466 3467 3468 3469 
20 procesos creados
Numero de itemes que se produciran: 20000
Recomendacion: lance en otro terminal el siguiente comando para
ver algunos estados intermedios del buffer:

   % cat /dev/monitor

Si el buffer se queda estancado y este script no termina,
probablemente deba terminarlo con control-C.  Pero si su monitor no
implementa correctamente la interrumpibilidad de las operaciones,
entonces tendra que rebootear la maquina.  :-(
Proceso consumidor 3470 lanzado
1000 itemes recibidos
2000 itemes recibidos
3000 itemes recibidos
4000 itemes recibidos
5000 itemes recibidos
6000 itemes recibidos
7000 itemes recibidos
8000 itemes recibidos
9000 itemes recibidos
Productor 3450 terminado
10000 itemes recibidos
11000 itemes recibidos
12000 itemes recibidos
13000 itemes recibidos
Productor 3467 terminado
14000 itemes recibidos
15000 itemes recibidos
Productor 3460 terminado
Productor 3451 terminado
16000 itemes recibidos
Productor 3454 terminado
Productor 3466 terminado
17000 itemes recibidos
Productor 3458 terminado
18000 itemes recibidos
Productor 3453 terminado
Productor 3463 terminado
Productor 3461 terminado
Productor 3459 terminado
Productor 3465 terminado
Productor 3452 terminado
19000 itemes recibidos
Productor 3457 terminado
Productor 3468 terminado
Productor 3462 terminado
Productor 3469 terminado
Productor 3464 terminado
Productor 3456 terminado
Productor 3455 terminado
Todos los itemes fueron correctamente recibidos
El estado final del monitor es el esperado
=== Bien: su modulo paso este test ===========
Todos los productores terminaron
Esperando 5 segundos a que termine el consumidor 3470
==============================================

Scripts que usan perl

+ buffer.pl: contiene las funciones put y get
+ testmon2: el script que debe usar para probar su tarea
+ get y put: los scripts que se usan en el enunciado

Scripts que usan /bin/sh

+ makebuf: inicializa un buffer
