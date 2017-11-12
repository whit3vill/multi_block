#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>        /* for NF_ACCEPT */
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <math.h>
#include <openssl/md5.h>

#define BUFSIZE 100
#define ARRAYSIZE 20000000

struct hashindex *create (char *url);
int myhash(const char *string);
void insert (char *url);
int search (char *url);
void syscom();
int IsHttp(unsigned char *data);
void filter (unsigned char* buf);
static u_int32_t print_pkt (struct nfq_data *tb);
static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
          struct nfq_data *nfa, void *data);
int myhash(const char *string);
void indexsort (char *line);
