#ifndef TCPFLAGS
#define TCPFLAGS

/* Data structures */
typedef struct {
  char flag[5];
} __attribute__((packed)) tcpflag;

#endif
