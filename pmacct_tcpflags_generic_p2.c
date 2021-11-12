/* pmacct_tcpflags_generic_p1.c - gcc pmacct_tcpflags_generic_p2.c -o bin/pmacct_tcpflags_generic_p2 -lavro */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <avro.h>


char tcpflags[6][5] = {"NULL", "NULL", "NULL", "NULL", "NULL", "NULL"};

//
// --- AVRO global variables ---
//
avro_schema_t sc_type_record, sc_type_array, sc_type_string;

//
// --- AVRO prototypes ---
//
void compose_tcpflags_avro_schema(void);

size_t generate_rnd(void);
void generate_tcpflags_array(size_t);


int
main(void)
{
  compose_tcpflags_avro_schema();

  while(1)
  {
    size_t rnd = generate_rnd();

    generate_tcpflags_array(rnd);

    size_t idx_0;
    for (idx_0 = 0; idx_0 < 6; idx_0++)
    {
      printf("%s ", tcpflags[idx_0]);
    }
    printf("\n");
    sleep(1);
  }

  return (0);
}


size_t
generate_rnd()
{
  size_t rnd;

  srand((size_t) time(0));
  rnd = rand() % 64;

  printf("%lu\n", rnd);

  return rnd;
}


void generate_tcpflags_array(size_t tcpflags_decimal)
{
  size_t tcpflags_binary[6] = {0, 0, 0, 0, 0, 0};
  const char tcpflags_mask[6][5] = {"URG", "ACK", "PSH", "RST", "SYN", "FIN"};

  size_t idx_0;
  if ((tcpflags_decimal > 0) && (tcpflags_decimal) < 64)
  {
    for (idx_0 = 5; tcpflags_decimal > 0 && idx_0 >= 0; idx_0--)
    {
      tcpflags_binary[idx_0] = (tcpflags_decimal % 2);
      tcpflags_decimal /= 2;
    }
  }

  size_t idx_1;
  for (idx_1 = 0; idx_1 < 6; idx_1++)
  {
    if (!tcpflags_binary[idx_1])
    {
      printf("%lu ", tcpflags_binary[idx_1]);
      strcpy(tcpflags[idx_1], "NULL");
    }
    else
    {
      printf("%lu ", tcpflags_binary[idx_1]);
      strcpy(tcpflags[idx_1], tcpflags_mask[idx_1]);
    }
  }
  printf("\n");
}
//
// --- AVRO functions ---
//
void
compose_tcpflags_avro_schema(void)
{
  sc_type_string = avro_schema_string();
  sc_type_array = avro_schema_array(sc_type_string);
  sc_type_record = avro_schema_record("acct_data", NULL);
  avro_schema_record_field_append(sc_type_record, "label", sc_type_array);

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;
  avro_schema_fp = fopen("avro_schema.dump", "w");
  avro_schema_writer = avro_writer_file(avro_schema_fp);
  avro_schema_to_json(sc_type_record, avro_schema_writer);
  fclose(avro_schema_fp);
}
