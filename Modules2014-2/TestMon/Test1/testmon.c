#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>

#define MAXSIZE 80
#define DEVMON devmon

typedef struct {
  char *name;
  int delay1, delay2;
} Param;

int busy= 0;
int bad= 0;
char *devmon;

void error(char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);

  bad= 1;
  pthread_exit(NULL);
}

void *test1(void *ptr) {
  Param *prm= ptr;
  int fd= open(DEVMON, O_RDWR);
  if (fd<0) {
    perror("No se pudo abrir el dispositivo\n");
    bad= 1;
    pthread_exit(NULL);
  }

  if (busy) error("No se respeta la exclusion mutua.");
  busy= 1;
  printf("%s: open entrega %d\n", prm->name, fd);
  sleep(prm->delay1);
  busy= 0;
  close(fd);
  return NULL;
}

void *test2_read(void *ptr) {
  Param *prm= ptr;
  int fd= open(DEVMON, O_RDWR);
  if (fd<0) {
    perror("No se pudo abrir dispositivo\n");
    bad= 1;
    pthread_exit(NULL);
  }

  if (busy) error("No se respeta la exclusion mutua.");
  busy= 1;
  printf("%s: open entrega %d\n", prm->name, fd);

  for (;;) {
    sleep(prm->delay1);
    char buf[MAXSIZE];
    busy= 0;
    int cnt= read(fd, buf, MAXSIZE);
    if (busy) error("No se respeta la exclusion mutua.");
    busy= 1;
    printf("%s: read entrega %d bytes: %s\n", prm->name, cnt, buf);
    if (strcmp(buf, "chao")==0)
      break;
  }

  sleep(prm->delay2);
  busy= 0;
  close(fd);
  return NULL;
}

void *test2_write(void *ptr) {
  Param *prm= ptr;
  char *msgs[4]= { "vacio", "hola", "que tal", "chao"};
  int i;

  for (i= 0; i<4; i++) {

    int fd= open(DEVMON, O_RDWR);
    if (fd<0) {
      perror("No se pudo abrir dispositivo\n");
      bad= 1;
      pthread_exit(NULL);
    }
    if (busy) error("No se respeta la exclusion mutua.");
    busy= 1;
    printf("%s: open entrega %d\n", prm->name, fd);

    sleep(prm->delay1);

    char *msg= msgs[i];
    int len= strlen(msg)+1;
    if (write(fd, msg, len)!= len)
      error("No se logro escribir hello");
    printf("%s: write deposito %s (%d bytes)\n", prm->name, msg, len);

    sleep(prm->delay2);

    busy= 0;
    close(fd);

    sleep(prm->delay2);
  }
  return NULL;
}

int main(int argc, char **argv) {
  if (argc!=2) {
    fprintf(stderr, "uso: %s <dispositivo-monitor>\n", argv[0]);
    exit(1);
  }

  devmon= argv[1];

  { /* Se verifica la exclusion mutua */
    pthread_t t1, t2, t3;
    Param prm1= { "T1", 1, 0 };
    Param prm2= { "T2", 1, 0 };
    Param prm3= { "T3", 1, 0 };
    pthread_create(&t1, 0, test1, &prm1);
    pthread_create(&t2, 0, test1, &prm2);
    pthread_create(&t3, 0, test1, &prm3);
    pthread_join(t1, 0);
    pthread_join(t2, 0);
    pthread_join(t3, 0);
    if (!bad)
      printf("Bien: se cumple la exclusion mutua en open ... close\n");
  }

  { /* Se verifica el funcionamiento de read como wait y write como broadcast */
    pthread_t t1, t2, t3;
    Param prm1= { "T1", 1, 1 };
    Param prm2= { "T2", 1, 1 };
    Param prm3= { "T3", 1, 1 };
    pthread_create(&t3, 0, test2_write, &prm3);
    sleep(1);
    pthread_create(&t1, 0, test2_read, &prm1);
    pthread_create(&t2, 0, test2_read, &prm2);
    pthread_join(t1, 0);
    pthread_join(t2, 0);
    pthread_join(t3, 0);
    if (!bad)
      printf("Bien: read espera el proximo write\n");
  }

  if (bad)
    fprintf(stderr, "Lo siento, su modulo monitor no funciona correctamente\n");
  else
    printf("\nSu modulo paso este test\n");

  return 0;
}
