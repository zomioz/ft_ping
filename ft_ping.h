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

extern volatile sig_atomic_t g_stop;

# define ICMP_DATA_SIZE 56

void	print_bits(u_int32_t octet);
u_int16_t ft_checksum(void *buf, int len);
bool ft_ping(char *destination);
struct addrinfo *ft_get_addr(char *destination);
int ft_create_socket(void);
void handle_sigint(int sig);

#endif