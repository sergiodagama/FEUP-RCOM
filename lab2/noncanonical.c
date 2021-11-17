/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"
#include "utils.c"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

volatile int STOP = FALSE;

unsigned char UA[SU_TRAMA_SIZE] = {FLAG, A_ER, C_UA, BCC(A_ER, C_UA), FLAG};

int main(int argc, char **argv)
{
  int fd, c, res;
  struct termios oldtio, newtio;

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
       (strcmp("/dev/ttyS1", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(argv[1]);
    exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1)
  { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;  /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  unsigned char rSET[SU_TRAMA_SIZE];
  int idx = 0;
  int CONNECTED = FALSE;

  //while - open connection
  while (!CONNECTED)
  {
    //Receção do SET
    if (readData(fd, 1, rSET, SU_TRAMA_SIZE) < 0)
      perror("Error reading SET\n");

    if (checkTramaReceived(rSET, SU_TRAMA_SIZE, TM_SET) == FALSE){
      perror("SET NOT RIGHT!\n"); //ver o que fazer nesta situação
      continue;
    }
      
    printf("All OK on receiver!\n");

    //Envio de UA
    if (writeData(fd, UA, SU_TRAMA_SIZE) < 0)
      perror("Error writing UA\n");
    else 
      STOP=TRUE; //está a parar quando envia bem o UA - para teste

    sleep(2);
    printf("UA Sent!\n");

    //While para receber o I
    printf("Loop to receive the first I and Stop missing!\n");
    CONNECTED = TRUE;
  }

  //while data receive + acknoledge

  //     sleep(1);

  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
