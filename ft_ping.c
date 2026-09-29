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


    //starting of recvfrom
    u_int8_t receive_buff[1024];
    struct sockaddr_in receive_addr;
    socklen_t receive_addr_len = sizeof(receive_addr);
    ssize_t size_receive;

    size_receive = recvfrom(sockfd, receive_buff, sizeof(receive_buff), 0,
        (struct sockaddr *)&receive_addr, &receive_addr_len);
    if (size_receive < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
    }
    else
    {
        u_int8_t ip_header_len = (receive_buff[0] & 0x0F) * 4;
        struct icmphdr *receive_icmp = (struct icmphdr *)(receive_buff + ip_header_len);

        if (receive_icmp->type != 0)
        {
            write(2, "Error: Receive type isn't 0\n", 28);
            if (packet)
                free(packet);
            freeaddrinfo(adresse);
            close(sockfd);
            return (0);
        }
        if (receive_icmp->un.echo.id != hdr->un.echo.id)
        {
            write(2, "Error: Receive id isn't the same as id sent\n", 44);
            if (packet)
                free(packet);
            freeaddrinfo(adresse);
            close(sockfd);
            return (0);
        }
        struct timeval receive_ts;
        struct timeval now;
        long rtt_usec;

        memcpy(&receive_ts, receive_buff + ip_header_len + sizeof(struct icmphdr),
            sizeof(struct timeval));
        gettimeofday(&now, NULL);
        rtt_usec = (now.tv_sec - receive_ts.tv_sec) * 1000000
            + (now.tv_usec - receive_ts.tv_usec);
        printf("Time of response = %ld ms\n", rtt_usec / 1000);
    }

    if (packet)
        free(packet);
    freeaddrinfo(adresse);
    close(sockfd);
    return (0);
}