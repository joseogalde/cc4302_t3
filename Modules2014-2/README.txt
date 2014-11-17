Ejemplos de modulos para el kernel de Linux

+ Hello: modulo trivial con rutinas init_module y cleanup_module
+ Mem: modulo que implementa el driver para un trozo de memoria
  de 8192 bytes.  Implementa la exclusion mutua de los escritores.
+ Syncread: solucion de la tarea 3 del semestre Otoño de 2013.
  Ademas de implementar la exclusion mutua de los escritores, hace que
  si hay un escritor activo (hizo open, pero todavia no invoca close),
  los lectores que llegaron al final del archivo, esperan a que
  el escritor (i) escriba mas datos, o (ii) cierre el disposivo.
  Se incluye el enunciado.
+ Multicast: solucion de la tarea 3 del semestre 2009.
  Se incluye el enunciado.
+ BadMonitor: solucion incorrecta de la tarea 3 del semestre primavera 2014.
  No implementa la exclusion mutua como se pide en el enunciado.

Directorio con los 3 tests para la tarea

+ TestMon

El siguiente no es un modulo, pero se requiere para compilar los modulos de mas
arriba (excepto Hello y Mem).

+ KMutex: implementacion de un mutex mas condiciones (tipos KMutex y KCondition)
  al estilo de pthread_mutex_t y pthread_cond_t.  La semantica es la misma
  aunque los nombres de las funciones de la API fueron abreviados.
  Casi todos los directorios anteriores contienen un link simbolico a los
  archivos kmutex.c y kmutex.h de este directorio.  Nunca use zip para
  empaquetar Modules2014-2 porque unzip convierte los links simbolicos en
  copias de los archivos.

Se incluye:
- tutorial de device drivers (archivo device-drivers-tutorial.pdf).
- una clase auxiliar con un tutorial de modulos y drivers de
  Javier Bustos (archivo "modulos-jbustos.pdf").

---

Cada directorio incluye un README.txt que indica como crear e instalar el
modulo y device.
