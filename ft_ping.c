#include "ft_ping.h"

void	print_bits(u_int32_t octet)
{
	int				i;
	unsigned char	bit;

	i = sizeof(octet) * 8;
	while (i--)
	{
		bit = (octet >> i & 1) + '0';
		write(1, &bit, 1);
	}
    write(1, "\n", 1);
}

void ft_checksum(struct icmphdr *tmp)
{
    u_int16_t *ptr = (u_int16_t *)tmp;
    int len = sizeof(struct icmphdr);
    u_int32_t total;

    total = 0;

    if (tmp->checksum != 0)
        tmp->checksum = 0;

    print_bits(total);
    while (len > 1)
    {
        total += *ptr;
        ptr++;
        len -= 2;
    }

    print_bits(total);
    while (total >> 16)
    {
        total = (total & 0xFFFF) + (total >> 16);
    }

    tmp->checksum = (u_int16_t)~total;
}

void ft_print_struct(struct icmphdr *tmp)
{
    printf("struct->type : %d\n", tmp->type);
    printf("struct->code : %d\n", tmp->code);
    printf("struct->checksum : %d\n", tmp->checksum);
    printf("struct->un.echo.id : %d\n", tmp->un.echo.id);
    printf("struct->un.echo.sequence : %d\n", tmp->un.echo.sequence);
}

struct addrinfo *ft_get_addr(char *destination)
{
    struct addrinfo *adresse;
    struct addrinfo hints;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    status = getaddrinfo(destination, NULL, &hints, &adresse);
    if (status != 0)
    {
        write(2, gai_strerror(status), strlen(gai_strerror(status)));
        write(2, "\n", 1);
        return (NULL);
    }
    return (adresse);
}

bool ft_ping(char *destination)
{
    int sockfd;
    struct addrinfo *adresse = ft_get_addr(destination);

    if (!adresse)
        return (false);
    printf("PING %s\n", destination);

    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return(1);
    }
    
    struct icmphdr *tmp;

    tmp = malloc(sizeof(struct icmphdr));
    tmp->type = 8;
    tmp->code = 0;
    tmp->checksum = 0;
    tmp->un.echo.id = (u_int16_t)getpid();
    tmp->un.echo.sequence = 0;

    ft_print_struct(tmp);

    ft_checksum(tmp);

    ssize_t sent = sendto(sockfd, tmp, sizeof(struct icmphdr), 0, adresse->ai_addr, adresse->ai_addrlen);
    if (sent < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
    }
    if (tmp)
        free(tmp);
    if (adresse)
        freeaddrinfo(adresse);
    close(sockfd);
    return true;
}