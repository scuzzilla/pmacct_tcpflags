/* pmacct_tcpflags_generic_p1.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


char tcpflags[6][5] = {"NULL", "NULL", "NULL", "NULL", "NULL", "NULL"};

unsigned int generate_rnd(void);
void generate_tcpflags_array(unsigned int);


int
main(void)
{
  while(1)
  {
    unsigned int rnd = generate_rnd();

    generate_tcpflags_array(rnd);

    unsigned int idx_0;
    for (idx_0 = 0; idx_0 < 6; idx_0++)
    {
      printf("%s ", tcpflags[idx_0]);
    }
    printf("\n");
    sleep(1);
  }

  return (0);
}


unsigned int
generate_rnd()
{
  unsigned int rnd;

  srand((unsigned int) time(0));
  rnd = rand() % 64;

  printf("%u\n", rnd);

  return rnd;
}


void generate_tcpflags_array(unsigned int tcpflags_decimal)
{
  unsigned int tcpflags_binary[6] = {0, 0, 0, 0, 0, 0};
  const char tcpflags_mask[6][5] = {"URG", "ACK", "PSH", "RST", "SYN", "FIN"};

  unsigned int idx_0;
  if (64 > tcpflags_decimal > 0)
  {
    for (idx_0 = 5; tcpflags_decimal > 0 && idx_0 >= 0; idx_0--)
    {
      tcpflags_binary[idx_0] = (tcpflags_decimal % 2);
      tcpflags_decimal /= 2;
    }
  }

  unsigned int idx_1;
  for (idx_1 = 0; idx_1 < 6; idx_1++)
  {
    if (!tcpflags_binary[idx_1])
    {
      printf("%u ", tcpflags_binary[idx_1]);
      strcpy(tcpflags[idx_1], "NULL");
    }
    else
    {
      printf("%u ", tcpflags_binary[idx_1]);
      strcpy(tcpflags[idx_1], tcpflags_mask[idx_1]);
    }
  }
  printf("\n");
}
