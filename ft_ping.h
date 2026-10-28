#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <sys/socket.h>
# include <string.h>
# include <errno.h>
# include <unistd.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <stdlib.h>
# include <stdbool.h>
# include <netdb.h>
# include <sys/time.h>
# include <signal.h>
# include <arpa/inet.h>
# include <math.h>

extern volatile sig_atomic_t g_stop;

# define ICMP_DATA_SIZE 56

//flag_c N = Stop after sending (and receiving) N ECHO_RESPONSE packets.
//flag_q = Quiet output.  Nothing is displayed except the summary lines at startup time and when finished.
//flag_w N = Stop after N seconds of sending packets.
//flag_i N = Wait N seconds between sending each packet.  The default is to wait for one second between each packet.
//flag_W N = Number of seconds N to wait for response
typedef struct s_parsing
{
    char *destination;
    bool flag_v;
    bool flag_c;
    int flag_c_count;
    bool flag_q;
    bool flag_w;
    int flag_w_count;
    bool flag_i;
    int flag_i_count;
    bool flag_W;
    int flag_W_count;
    bool flag_question_mark;

}t_parsing;

typedef struct s_ping_stats
{
    int count_send;
    int count_receive;
    long rtt_min;
    long rtt_max;
    long rtt_sum;
    double rtt_sum_sq;
    struct timeval start_ts;
    struct timeval end_ts;
    char *destination;

}t_ping_stats;

u_int16_t ft_checksum(void *buf, int len);
bool ft_ping(char *destination);
struct addrinfo *ft_get_addr(char *destination);
int ft_create_socket(void);
void handle_sigint(int sig);
void ft_final_print(struct s_ping_stats *stats);
struct s_ping_stats *ft_create_stats(char *destination);

#endif