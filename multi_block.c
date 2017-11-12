#include "multi_block.h"

struct hashtable *array = NULL;
int flag = 1;

struct hashindex {
   char url[30];
   struct hashindex *next;
};

struct hashtable {
  int count;
  struct hashindex *head;
};

struct hashindex *create (char *url) {
  struct hashindex *temp;
  temp = (struct hashindex *)malloc(sizeof(struct hashindex));
  strncpy(temp->url, url, strlen(url));
  temp->next = NULL;

  return temp;
}

int myhash(const char *string) {
   int i;
   unsigned int num = 0;

   unsigned char result[MD5_DIGEST_LENGTH];



   MD5(string, strlen(string), result);

   for (i=0; i<16; i++) {
      printf("%02x", result[i]);
      num += (int)(result[i]) * (int)(pow((double)(16), (double)(4-2*(i))));
   }
   printf("\n");

   return num;
}

void insert (char *url) {
  int index = myhash(url);
  struct hashindex *node = create(url);

  if (!array[index].head) {
    array[index].head = node;
    array[index].count = 1;
    return;
  }

  node->next = array[index].head;
  array[index].head = node;
  array[index].count++;
  return;
}
int search (char *url) {
  int index = myhash(url);
  int flag = 0;
  struct hashindex *node;

  node = array[index].head;
  if (!node) {
    return 0;
  }
  while (node != NULL) {
    if (!strncmp(node->url, url, strlen(url))) {
      flag = 1;
      return 1;
    }
    node = node->next;
  }
  if (!flag) {
    return 0;
  }
}

void syscom() {
    system("iptables -F");
    system("iptables -A OUTPUT -j NFQUEUE --queue-num 0");
    system("iptables -A INPUT -j NFQUEUE --queue-num 0");
}
int IsHttp(unsigned char *data) {
    if (!strncmp(data, "GET", 3)) {
        return 1;
    }
    else if (!strncmp(data, "POST", 4)) {
        return 1;
    }
    else if (!strncmp(data, "HEAD", 4)) {
        return 1;
    }
    else if (!strncmp(data, "PUT", 3)) {
        return 1;
    }
    else if (!strncmp(data, "DELETE", 6)) {
        return 1;
    }
    else if (!strncmp(data, "OPTIONS", 7)) {
        return 1;
    }
    else return 0;
}
void filter (unsigned char* buf) {
    struct ip *ipH;
    struct tcphdr *tcpH;
    unsigned char *data;
    char *addr;
    char *addr2;
    char *url;

    ipH = (struct ip *)buf;
    if (ipH->ip_p == IPPROTO_TCP) {
        tcpH = (unsigned char *)(struct tcphdr *)buf  + 4*(ipH->ip_hl);
        data = (unsigned char *)tcpH + 4*(tcpH->th_off);
        //printf("%s",data);
        if (IsHttp(data)) {
          printf("1\n");
            if (strstr(data, "Host: ")) {
              printf("2\n");
                addr = strstr(data, "www.");
                addr2 = strstr(addr, "\r");
                printf("3\n");
                //printf("%d\n", strlen(addr)-strlen(addr2));
                url = (char *)malloc(strlen(addr)-strlen(addr2));
                strncpy(url, addr+4, strlen(addr)-strlen(addr2)-4);
                printf("%s\n", url);
                if(search(url)) {
                    flag = 0;
                }
            }
        }
    }
    //free(url);
}
static u_int32_t print_pkt (struct nfq_data *tb)
{
    int id = 0;
    struct nfqnl_msg_packet_hdr *ph;
    struct nfqnl_msg_packet_hw *hwph;
    u_int32_t mark,ifi; 
    int ret;
    unsigned char *data;

    ph = nfq_get_msg_packet_hdr(tb);
    if (ph) {
        id = ntohl(ph->packet_id);
    }

    hwph = nfq_get_packet_hw(tb);
    if (hwph) {
        int i, hlen = ntohs(hwph->hw_addrlen);
    }
    mark = nfq_get_nfmark(tb);
    ifi = nfq_get_indev(tb);
    ifi = nfq_get_outdev(tb);
    ifi = nfq_get_physindev(tb);
    ifi = nfq_get_physoutdev(tb);
    ret = nfq_get_payload(tb, &data);

    filter(data);

    //fputc('\n', stdout);

    return id;
}
static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
          struct nfq_data *nfa, void *data)
{
    u_int32_t id = print_pkt(nfa);
    //printf("entering callback\n");
    if (flag == 0) {
        flag = 1;
        printf("filtered!\n");
        return nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
    }
    return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
}

void main (int argc, char *argv[]) {
   char *filename;
   FILE *fp;
   char buffer[BUFSIZE];
   char *string;
   char *line;
   int count;
   int i;
   struct nfq_handle *h;
   struct nfq_q_handle *qh;
   struct nfnl_handle *nh;
   int fd;
   int rv;
   char buf[4096] __attribute__ ((aligned));

   filename = argv[1];
   fp = fopen (filename, "r");
   if (fp == NULL)   printf("No %s in directory.\n", filename);

   array = (struct hashtable *)calloc(ARRAYSIZE, sizeof(struct hashtable));

   while ((line = fgets(buffer, BUFSIZE, fp)) != NULL) {
      string = strtok(line, ",");
      count = 0;
      while (string) {
         count++;
         if (count == 2) {
            string[strlen(string)-2] = 0;
            printf("%s\n", string);
            insert(string);
         }
         string = strtok(NULL, ",");
      }
    }

    syscom();

    //printf("opening library handle\n");
    h = nfq_open();
    if (!h) {
        fprintf(stderr, "error during nfq_open()\n");
        exit(1);
    }

    printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
    if (nfq_unbind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "error during nfq_unbind_pf()\n");
        exit(1);
    }

    printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
    if (nfq_bind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "error during nfq_bind_pf()\n");
        exit(1);
    }

    printf("binding this socket to queue '0'\n");
    qh = nfq_create_queue(h,  0, &cb, NULL);
    if (!qh) {
        fprintf(stderr, "error during nfq_create_queue()\n");
        exit(1);
    }

    printf("setting copy_packet mode\n");
    if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "can't set packet_copy mode\n");
        exit(1);
    }

    fd = nfq_fd(h);

    for (;;) {
        if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
            //printf("pkt received\n");
            nfq_handle_packet(h, buf, rv);
            continue;
        }
        /* if your application is too slow to digest the packets that
         * are sent from kernel-space, the socket buffer that we use
         * to enqueue packets may fill up returning ENOBUFS. Depending
         * on your application, this error may be ignored. nfq_nlmsg_verdict_putPlease, see
         * the doxygen documentation of this library on how to improve
         * this situation.
         */
        if (rv < 0 && errno == ENOBUFS) {
            printf("losing packets!\n");
            continue;
        }
        perror("recv failed");
        break;
    }

    //printf("unbinding from queue 0\n");
    nfq_destroy_queue(qh);

#ifdef INSANE
    /* normally, applications SHOULD NOT issue this command, since
     * it detaches other programs/sockets from AF_INET, too ! */
    //printf("unbinding from AF_INET\n");
    nfq_unbind_pf(h, AF_INET);
#endif
    //printf("closing library handle\n");
    nfq_close(h);

    exit(0);
}
