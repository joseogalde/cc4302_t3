Contiene el primer test para su tarea.

Para compilar invoque:

% make

Para ejecutar:

% ./testmon /dev/monitor

La salida de este test es mas o menos la siguiente:
=============================================
T1: open entrega 3
T2: open entrega 4
T3: open entrega 5
Bien: se cumple la exclusion mutua en open ... close
T3: open entrega 3
T3: write deposito vacio (6 bytes)
T1: open entrega 4
T1: read entrega 6 bytes: vacio
T2: open entrega 5
T2: read entrega 6 bytes: vacio
T3: open entrega 3
T3: write deposito hola (5 bytes)
T1: read entrega 5 bytes: hola
T2: read entrega 5 bytes: hola
T3: open entrega 3
T3: write deposito que tal (8 bytes)
T1: read entrega 8 bytes: que tal
T2: read entrega 8 bytes: que tal
T3: open entrega 3
T3: write deposito chao (5 bytes)
T1: read entrega 5 bytes: chao
T2: read entrega 5 bytes: chao
Bien: read espera el proximo write

Su modulo paso este test
=============================================

Archivos:

+ testmon.c: verifica que se cumpla la exclusion mutua.
+ Makefile: para compilar testmon.c
