#!/usr/bin/perl

# use strict;
# package buffer;


## el put hace un open + read + write + release del/dev/monitor porque, primero extrae el contador y le suma 1, además tambien agrega un item mas un caracter ','
sub put
{
  # declaracion de variables
  my $item; my $dev; my $max; my $lin; my @arr; my $cnt;

  # @_ es la lista de parametros recibidos por put
  # En este caso es uno solo: el item que se deposita en el buffer
  $dev=shift(@_);
  $item=shift(@_);

  # Se abre el device $dev en lectura/escritura.  El handle para leer o
  # escribir es $BUF.  Al mismo tiempo solicita la propiedad exclusiva del
  # device.
  open(my $BUF, "+<", $dev) || die "error: fallo el device";

  # Lee el contenido del buffer hasta que no este lleno.
  # La primera lectura es siempre no bloqueante.  De la segunda en adelante
  # libera el device y se queda esperando hasta que el consumidor escriba
  # una nueva version del buffer.
  do {
    $lin=<$BUF>;             # Lee el buffer en el string $lin
    chop($lin);              # Le quita el \n
    # print "$lin | put($item)\n";
    @arr= split(/,/, $lin);  # Decodifica la linea en un arreglo
    $cnt=$arr[0];            # El elemento 0 es el numero de itemes: $cnt
    $max=$arr[1];            # El elemento 1 es el tamano del buffer: $max
  } while ($cnt == $max);    # Mientras el buffer esta lleno
  push(@arr, $item);         # Agrega $item al final del buffer
  $arr[0]= $cnt+1;           # El buffer tiene un elemento adicional
  $lin= join(",", @arr) . "\n";  # Convierte el arreglo nuevamente en un string
  # print "put($item) | $lin";
  print $BUF "$lin";         # Escribe la nueva version del buffer en el device
                             # Al mismo tiempo hace un broadcast para que se
                             # despierten los procesos esperando en un read
  close($BUF);               # Libera el device
}

## el get hace un open + read + write + release. NO es una lectura "incoente" porque quita (read) elementos del /dev/monitor y vuelve a poner otros valores (write)
sub get
{
  # declaracion de variables
  my $item; my $dev; my $max; my $lin; my @arr; my $cnt;

  $dev=shift(@_);

  # Este procedimiento no tiene parametros pero si retorna un item

  # Se abre el device $dev en lectura/escritura.  El handle para leer o
  # escribir es $BUF.  Al mismo tiempo solicita la propiedad exclusiva del
  # device.
  open(my $BUF, "+<", $dev) || die "error: fallo el device";

  # Lee el contenido del buffer hasta que no este vacio.
  # La primera lectura es siempre no bloqueante.  De la segunda en adelante
  # libera el device y se queda esperando hasta que un productor escriba
  # una nueva version del buffer.
  do {
    $lin=<$BUF>;             # Lee el buffer en el string $lin
    chop($lin);              # Le quita el \n
    # print "$lin | get\n";
    @arr= split(/,/, $lin);  # Decodifica la linea en un arreglo
    $cnt= shift(@arr);       # Extrae el primer elemento del arreglo en $cnt
  } while ($cnt == 0);       # Mientras el buffer esta vacio
  $max= shift(@arr);         # Extrae el siguiente elemento del arreglo en $max
  $item= shift(@arr);        # Extrae el siguiente elemento que es el $item
  $cnt= $cnt-1;              # Hay un item menos en el buffer
  unshift @arr, $max;        # Inserta $max al inicio del arreglo
  unshift @arr, $cnt;        # Inserta $cnt al inicio del arreglo
  $lin= join(",", @arr) . "\n";  # Convierte el arreglo nuevamente en un string
  # print "get -> $item | $lin";
  print $BUF "$lin";         # Escribe la nueva version del buffer en el device
                             # Al mismo tiempo hace un broadcast para que se
                             # despierten los procesos esperando en un read
  close ($BUF);              # Libera el device
  return ($item);            # Entrega el item extraido
}

1;
