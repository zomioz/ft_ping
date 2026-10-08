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

typedef struct s_parsing
{
    char *destination;
    bool flag_v;
    bool flag_T;
    bool flag_n;

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