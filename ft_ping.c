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

u_int16_t ft_checksum(void *buf, int len)
{
    u_int16_t *ptr = (u_int16_t *)buf;
    u_int32_t total;

    total = 0;

    while (len > 1)
    {
        total += *ptr;
        ptr++;
        len -= 2;
    }

    while (total >> 16)
    {
        total = (total & 0xFFFF) + (total >> 16);
    }

    return ((u_int16_t)~total);
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
        freeaddrinfo(adresse);
        return(1);
    }

    size_t packet_len = sizeof(struct icmphdr) + sizeof(struct timeval);
    char *packet = malloc(packet_len);
    if (!packet)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        close(sockfd);
        freeaddrinfo(adresse);
        return (1);
    }
    struct icmphdr *hdr = (struct icmphdr *)packet;

    hdr->type = 8;
    hdr->code = 0;
    hdr->checksum = 0;
    hdr->un.echo.id = (u_int16_t)getpid();
    hdr->un.echo.sequence = 0;

    struct timeval ts;
    gettimeofday(&ts, NULL);

    memcpy(packet + sizeof(struct icmphdr), &ts, sizeof(struct timeval));

    hdr->checksum = ft_checksum(packet, packet_len);

    ssize_t sent = sendto(sockfd, packet, packet_len, 0, adresse->ai_addr, adresse->ai_addrlen);
    if (sent < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
    }

    if (packet)
        free(packet);
    freeaddrinfo(adresse);
    close(sockfd);
    return (0);
}