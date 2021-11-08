/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const unsigned int TCPFLAGS_SIZE = 6;
const unsigned int TCPFLAGS_MAX_COMBINATIONS = 63;
unsigned int tcpflags_bin[6] = {0, 0, 0, 0, 0, 0};
char tcpflags_str[6][10] = {"URG", "ACK", "PSH", "RST", "SYN", "FIN"};


unsigned int generate_rnd(void);
unsigned int *generate_bin_from_rnd(unsigned int);
void generate_tcpflags_from_bin(unsigned int *);


int
main(void)
{
  unsigned int rnd = generate_rnd();
  printf("%u\n", rnd);

  unsigned int *tcpflags_ptr = generate_bin_from_rnd(rnd);

  int idx;
  for (idx = 0; idx < TCPFLAGS_SIZE; idx++)
  {
    printf("%u ", tcpflags_ptr[idx]);
  }
  printf("\n");

  generate_tcpflags_from_bin(tcpflags_ptr);

  unsigned int idx_0;
  for (idx_0 = 0; idx_0 < TCPFLAGS_SIZE; idx_0++)
  {
    printf("%s ", tcpflags_str[idx_0]);
  }
  printf("\n");

  return (0);
}


unsigned int
generate_rnd()
{
  unsigned int rnd;

  srand((unsigned int) time(0));
  rnd = rand() % TCPFLAGS_MAX_COMBINATIONS;

  return rnd;
}


unsigned int *generate_bin_from_rnd(unsigned int rnd)
{
  unsigned int idx;
  for (idx = (TCPFLAGS_SIZE - 1); rnd > 0 && idx >= 0; idx--)
  {
    tcpflags_bin[idx] = (rnd % 2);
    rnd /= 2;
  }

  return tcpflags_bin;
}


void generate_tcpflags_from_bin(unsigned int *tcpflags_bin_ptr)
{
  unsigned int idx;
  for (idx = 0; idx < TCPFLAGS_SIZE; idx++)
  {
    if (tcpflags_bin_ptr[idx] == 0)
    {
      strcpy(tcpflags_str[idx], "NULL");
    }
  }
}
