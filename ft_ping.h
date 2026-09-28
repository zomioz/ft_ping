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

void	print_bits(u_int32_t octet);
void ft_checksum(struct icmphdr *tmp);
void ft_print_struct(struct icmphdr *tmp);
bool ft_ping(char *destination);
struct addrinfo *ft_get_addr(char *destination);

#endif