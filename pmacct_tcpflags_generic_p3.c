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
avro_value_t v_type_string, v_type_array, v_type_record;
avro_value_iface_t *if_type_record, *if_type_array, *if_type_string;

//
// --- AVRO prototypes ---
//
void compose_tcpflags_avro_schema(void);
int compose_tcpflags_avro_data(char tcpflags[6][5]);
int print_tcpflags_avro_data();
void free_tcpflags_avro_data_memory(void);

/* Function prototypes */
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
    compose_tcpflags_avro_data(tcpflags);
    print_tcpflags_avro_data();
    free_tcpflags_avro_data_memory();
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
  avro_schema_record_field_append(sc_type_record, "tcp_flags", sc_type_array);

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;
  avro_schema_fp = fopen("avro_schema.dump", "w");
  avro_schema_writer = avro_writer_file(avro_schema_fp);
  avro_schema_to_json(sc_type_record, avro_schema_writer);
  fclose(avro_schema_fp);
}


int
compose_tcpflags_avro_data(char tcpflags[6][5])
{
  if_type_record = avro_generic_class_from_schema(sc_type_record);
  if_type_array = avro_generic_class_from_schema(sc_type_array);
  if_type_string = avro_generic_class_from_schema(sc_type_string);

  avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_array, &v_type_array);
  avro_generic_value_new(if_type_string, &v_type_string);

  avro_file_writer_t db_w;
  const char *dbname = "avro_record.db";

  remove(dbname);
  int rval_w = avro_file_writer_create(dbname, sc_type_record, &db_w);
  if (rval_w)
  {
    fprintf(stderr, "Error: %s\n", avro_strerror());
    return -1;
  }

  size_t array_size;
  avro_value_get_size(&v_type_array, &array_size);
  printf("before: %lu\n", array_size);

  int idx_1;
  for (idx_1 = 0; idx_1 < 6; idx_1++)
  {
    if (avro_value_get_by_name(&v_type_record, "tcp_flags", &v_type_array, NULL) == 0)
    {
      if (avro_value_append(&v_type_array, &v_type_string, NULL) == 0)
      {
        //if (strcmp(tcpflags[idx_1], "NULL") != 0)
        {
          avro_value_set_string(&v_type_string, tcpflags[idx_1]);
        }
      }
    }
  }
  avro_file_writer_append_value(db_w, &v_type_record);

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  avro_value_get_size(&v_type_array, &array_size);
  printf("after: %lu\n", array_size);

  return 0;
}


int
print_tcpflags_avro_data()
{
  const char *dbname = "avro_record.db";
  avro_file_reader_t db_r;
  int rval_r = avro_file_reader(dbname, &db_r);
  if (rval_r)
  {
    fprintf(stderr, "Error: %s\n", avro_strerror());
    return -1;
  }

  avro_file_reader_read_value(db_r, &v_type_record);
  size_t value_size;
  const char *p = NULL;

  size_t array_size;
  avro_value_get_size(&v_type_array, &array_size);
  printf("print_array_size: %lu\n", array_size);

  int idx_0;
  for (idx_0 = 0; idx_0 < array_size; idx_0++)
  {
    if (avro_value_get_by_name(&v_type_record, "tcp_flags", &v_type_array, NULL) == 0)
    {
      if (avro_value_get_by_index(&v_type_array, idx_0, &v_type_string, NULL) == 0)
      {
        avro_value_get_string(&v_type_string, &p, &value_size);
      }
    }
    //fprintf(stdout, "%lu\n", value_size);
    fprintf(stdout, "%s ", p);
  }
  printf("\n");

  avro_file_reader_close(db_r);

  return 0;
}


void
free_tcpflags_avro_data_memory(void)
{
  avro_value_iface_decref(if_type_record); //no need to decref the associated value
  avro_value_iface_decref(if_type_array); //no need to decref the associated value
  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_schema_decref(sc_type_record);
  avro_schema_decref(sc_type_array);
  avro_schema_decref(sc_type_string);
  //avro_value_decref(&v_type_record);
  //avro_value_decref(&v_type_map);
  //avro_value_decref(&v_type_string);
}
