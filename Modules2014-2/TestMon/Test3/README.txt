Este directorio contiene el 3er. test para su tarea.

Invoque:

% ./testmon3 /dev/monitor

La salida de este test es mas o menos la siguiente:
=====================================================
Probando que un proceso que espera obtener el monitor puede ser
interrumpido
Abrimos el monitor, pero no lo cerramos intencionalmente para que
el hijo espere indefinidamente
El open en el hijo se debe quedar esperando indefinidamente
Interrumpimos el hijo
El hijo debe terminar de inmediato.  Si no ocurre, la operacion
open no es interrumpible.  Mate el proceso con control-C.
correcto: el hijo fue interrumpido
-------------------------------------------------------
Probando que un proceso que espera en un read puede ser
interrumpido
El hijo debe terminar de inmediato.  Si no ocurre, la operacion
read no es interrumpible.  Mate el proceso con control-C.
correcto: el hijo fue interrumpido en un read
----------------------------------------------------------------
Probando que un proceso que espera obtener el monitor despues de
un read puede ser interrumpido
El hijo debe terminar de inmediato.  Si no ocurre, la operacion
read no es interrumpible.  Mate el proceso con control-C.
correcto: el hijo fue interrumpido mientras espera reobtener el
monitor despues en la operacion read
=== Felicitaciones: su tarea funciona correctamente ===
=====================================================

Advertencia:
Si su tarea no implementa la interrumpibilidad de las operaciones,
un subproceso de testmon3 no morira jamas (incluso si lo mata con
kill -9).  Tendra que rebootear la maquina para poder utilizar
nuevamente /dev/monitor.
