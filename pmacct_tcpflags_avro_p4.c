/*
 * fourth prototype - linked-list logic with libcdada (AVRO version)
 *
 * 1. Invoke generator with the types you want to support
 *    ~/Projects/libcdada/tools/cdada-gen list:tcpflag -o ~/Projects/pmacct_tcpflags/autogen_cdada_tcpflag.cc
 *
 * 2. Add header includes for types 'tcpflags' in the place holder
 *    vim ~/Projects/pmacct_tcpflags/autogen_cdada_tcpflag.cc
 *
 * 3. Add to your build system
 *    g++ -c ~/Projects/pmacct_tcpflags/autogen_cdada_tcpflag.cc
 *
 * 4. Link your application; make sure to link against -lcdada:
 *    cd ~/Projects/pmacct_tcpflags/
 *    gcc -Wall pmacct_tcpflags_avro_p4.c autogen_cdada_tcpflag.o -o bin/pmacct_tcpflags_avro_p4 -lcdada -lstdc++ -lavro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <avro.h>
#include <cdada/list.h>
#include "cdada_types/tcpflag.h"

CDADA_LIST_CUSTOM_TYPE_DECL(tcpflag);


//
// --- AVRO global variables ---
//
avro_schema_t sc_type_record, sc_type_array, sc_type_string;

avro_value_t v_type_record;
//avro_value_iface_t *if_type_record;
//
// --- AVRO prototypes ---
//
void compose_tcpflags_avro_schema(void);
int compose_tcpflags_avro_data(size_t, avro_value_t);
int print_tcpflags_avro_data(cdada_list_t *, size_t, avro_value_t);
void free_tcpflags_avro_data_memory(void);

/* Function prototypes */
size_t generate_rnd(void);
cdada_list_t *tcpflags_to_linked_list(size_t);


int
main(void)
{
  compose_tcpflags_avro_schema();

  while(1)
  {
    /* rnd unsigned int (0 - 63) generation */
    size_t rnd = generate_rnd();

    compose_tcpflags_avro_data(rnd, v_type_record);
    //print_tcpflags_avro_data(tcpflag_ll, ll_size, v_type_record);

    sleep(1);
  }

  free_tcpflags_avro_data_memory();

  return (0);
}


size_t
generate_rnd()
{
  size_t rnd;

  srand((size_t) time(0));
  rnd = rand() % 64;

  printf("\n%lu\n", rnd);

  return rnd;
}


/* tcpflags to linked-list*/
cdada_list_t *
tcpflags_to_linked_list(size_t tcpflags_decimal)
{
  /* Generate the flag's binary array */
  /* the flag's size is equal to the struct's tcpflag.flag size */
  const char tcpflags_mask[6][5] = {"URG", "ACK", "PSH", "RST", "SYN", "FIN"};
  size_t tcpflags_binary[6] = {0};

  /* tcpflags binary format - valid decimals between 0 & 63 */
  size_t idx_0;
  if ((tcpflags_decimal > 0) && (tcpflags_decimal) < 64)
  {
    for (idx_0 = 5; tcpflags_decimal > 0 && idx_0 >= 0; idx_0--)
    {
      tcpflags_binary[idx_0] = (tcpflags_decimal % 2);
      tcpflags_decimal /= 2;
    }
  }

  /* Generate the flag's linked-list */
  cdada_list_t *tcpflag_linked_list = cdada_list_create_custom(tcpflag);
  tcpflag tcpstate;

  size_t idx_1;
  for (idx_1 = 0; idx_1 < 6; idx_1++)
  {
    memset(&tcpstate, 0, sizeof(tcpstate));
    if (!tcpflags_binary[idx_1])
    {
      printf("%lu ", tcpflags_binary[idx_1]);
      strcpy(tcpstate.flag, "NULL");
    }
    else
    {
      printf("%lu ", tcpflags_binary[idx_1]);
      strcpy(tcpstate.flag, tcpflags_mask[idx_1]);
    }
    cdada_list_push_back(tcpflag_linked_list, &tcpstate);
  }

  return tcpflag_linked_list;
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
compose_tcpflags_avro_data(size_t rnd, avro_value_t v_type_record)
{
  /* linked-list creation */
  cdada_list_t *ll = tcpflags_to_linked_list(rnd);
  int ll_size = cdada_list_size(ll);

  /* v_type_record is global */
  avro_value_t v_type_string, v_type_array;

  avro_value_iface_t *if_type_record, *if_type_array, *if_type_string;
  if_type_record = avro_generic_class_from_schema(sc_type_record);
  if_type_array = avro_generic_class_from_schema(sc_type_array);
  if_type_string = avro_generic_class_from_schema(sc_type_string);

  avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_array, &v_type_array);
  avro_generic_value_new(if_type_string, &v_type_string);


  tcpflag tcpstate;

  printf("\nstart -> linked-list:\n");
  size_t idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    cdada_list_get(ll, idx_0, &tcpstate);
    printf("tcpflag: %s\n", tcpstate.flag);
  }

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

  size_t idx_1;
  for (idx_1 = 0; idx_1 < ll_size; idx_1++)
  {
    cdada_list_get(ll, idx_1, &tcpstate);
    if (avro_value_get_by_name(&v_type_record, "tcp_flags", &v_type_array, NULL) == 0)
    {
      /* Serialize only flags set to 1 */
      if ((avro_value_append(&v_type_array, &v_type_string, NULL) == 0) && (strcmp(tcpstate.flag, "NULL") != 0))
      {
        /* scalar values are overwritten - reset not mandatory */
        avro_value_reset(&v_type_string);
        avro_value_set_string(&v_type_string, tcpstate.flag);
      }
    }
  }
  avro_file_writer_append_value(db_w, &v_type_record);

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  avro_value_get_size(&v_type_array, &array_size);
  printf("after: %lu\n", array_size);

  cdada_list_destroy(ll);

  avro_value_decref(&v_type_string);
  avro_value_decref(&v_type_array);
  avro_value_decref(&v_type_record);

  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_value_iface_decref(if_type_array); //no need to decref the associated value
  avro_value_iface_decref(if_type_record); //no need to decref the associated value

  return 0;
}


/*
int
print_tcpflags_avro_data(cdada_list_t *ll, size_t ll_size, avro_value_t v_type_record)
{
  avro_value_t v_type_string, v_type_array;

  avro_value_iface_t *if_type_record, *if_type_array, *if_type_string;
  if_type_record = avro_generic_class_from_schema(sc_type_record);
  if_type_array = avro_generic_class_from_schema(sc_type_array);
  if_type_string = avro_generic_class_from_schema(sc_type_string);

  avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_array, &v_type_array);
  avro_generic_value_new(if_type_string, &v_type_string);

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
  printf("\nprint_array_size: %lu\n", array_size);

  size_t idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    if (avro_value_get_by_name(&v_type_record, "tcp_flags", &v_type_array, NULL) == 0)
    {
      if (avro_value_get_by_index(&v_type_array, idx_0, &v_type_string, NULL) == 0)
      {
        avro_value_get_string(&v_type_string, &p, &value_size);
        avro_value_decref(&v_type_string);
      }
    }
    //fprintf(stdout, "%lu\n", value_size);
    fprintf(stdout, "%s ", p);
  }
  printf("\n");

  avro_file_reader_close(db_r);

  avro_value_decref(&v_type_array);
  avro_value_decref(&v_type_record);

  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_value_iface_decref(if_type_array); //no need to decref the associated value
  avro_value_iface_decref(if_type_record); //no need to decref the associated value

  return 0;
}
*/


void
free_tcpflags_avro_data_memory(void)
{
  avro_schema_decref(sc_type_record);
  avro_schema_decref(sc_type_array);
  avro_schema_decref(sc_type_string);
  //avro_value_iface_decref(if_type_record); //no need to decref the associated value
  //avro_value_iface_decref(if_type_array); //no need to decref the associated value
  //avro_value_iface_decref(if_type_string); //no need to decref the associated value
  //avro_value_decref(&v_type_record);
  //avro_value_decref(&v_type_array);
  //avro_value_decref(&v_type_string);
}
