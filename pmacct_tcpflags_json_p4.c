/*
 * fourth prototype - linked-list logic with libcdada (JSON version)
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
 *    gcc -Wall pmacct_tcpflags_json_p4.c autogen_cdada_tcpflag.o -o bin/pmacct_tcpflags_json_p4 -lcdada -lstdc++ -ljansson
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <jansson.h>
#include <cdada/list.h>
#include "cdada_types/tcpflag.h"

CDADA_LIST_CUSTOM_TYPE_DECL(tcpflag);


/* Function prototypes */
size_t generate_rnd(void);
cdada_list_t *tcpflags_to_linked_list(size_t);
//
// --- JSON prototypes ---
//
json_t *compose_tcpflags_json_data(cdada_list_t *, size_t);


int
main(void)
{
  while(1)
  {
    /* rnd unsigned int (0 - 63) generation */
    size_t rnd = generate_rnd();

    /* linked-list creation */
    cdada_list_t *tcpflag_ll = tcpflags_to_linked_list(rnd);
    int ll_size = cdada_list_size(tcpflag_ll);

    json_t *root_l0 = json_object();
    json_t *root_l1 = compose_tcpflags_json_data(tcpflag_ll, ll_size);

    json_object_set_new(root_l0, "tcp_flags", root_l1);

    char *j_dump_final  = json_dumps(root_l0, JSON_INDENT(2));
    printf("%s\n", j_dump_final);
    free(j_dump_final);

    json_decref(root_l0);
    cdada_list_destroy(tcpflag_ll);

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
// --- JSON functions ---
//
json_t *
compose_tcpflags_json_data(cdada_list_t *ll, size_t ll_size)
{
  tcpflag tcpstate;

  printf("\nstart -> linked-list:\n");
  size_t idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    cdada_list_get(ll, idx_0, &tcpstate);
    printf("tcpflag: %s\n", tcpstate.flag);
  }
  printf("\n\n---\n");

  json_t *root = json_array();
  json_t *j_str_tmp = NULL;

  int idx_1;
  for (idx_1 = 0; idx_1 < ll_size; idx_1++)
  {
    cdada_list_get(ll, idx_1, &tcpstate);
    if (strcmp(tcpstate.flag, "NULL") != 0)
    {
      j_str_tmp = json_string(tcpstate.flag);
      json_array_append(root, j_str_tmp);
    }
  }

  return root;
}
