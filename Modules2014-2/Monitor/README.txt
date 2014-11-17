Este ejemplo es una adaptacion del tutorial incluido
(archivo "device drivers tutorial.pdf") y bajado de:
http://www.freesoftwaremagazine.com/articles/drivers_linux

---

Guia rapida:

Lo siguiente se debe realizar parados en
el directorio en donde se encuentra este README.txt

+ Compilacion (puede ser en modo usuario):
  % make
  ...
  % ls
  ... badmonitor.ko ...

+ Instalacion (en modo root)

  # mknod /dev/badmonitor c 62 0
  # chmod a+rw /dev/badmonitor
  # insmod badmonitor.ko
  # dmesg | tail
  ...
  [...........] Inserting badmonitor module
  #

+ Testing

  El ejemplo del enunciado de la tarea de 2014/2.  Los scripts para put,
  get y makebuf estan en el directorio TestMon.  Funciona porque uno
  no es lo suficientemente rapido como para gatillar un datarace cuando
  invoca put y get interactivamente.

  Los siguientes tests deben fallar.  Por eso es un *bad* monitor.

  % cd .../TestMon
  % make
  % ./testmon /dev/badmonitor
  
  % ./prodcons /dev/badmonitor 1000 10

+ Desinstalar el modulo

  # rmmod badmonitor.ko
  #
