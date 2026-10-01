#include "ft_ping.h"

volatile sig_atomic_t g_stop = 0;

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
        write(2, "ping: ", 6);
        write(2, destination, strlen(destination));
        write(2, ": ", 2);
        write(2, gai_strerror(status), strlen(gai_strerror(status)));
        write(2, "\n", 1);
        return (NULL);
    }
    return (adresse);
}


int ft_create_socket(void)
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return(-1);
    }
    return (sockfd);
}

char *ft_create_packet()
{
    size_t packet_len = sizeof(struct icmphdr) + ICMP_DATA_SIZE;
    char *packet = malloc(packet_len);
    if (!packet)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return (NULL);
    }

    struct icmphdr *hdr = (struct icmphdr *)packet;
    hdr->type = 8;
    hdr->code = 0;
    hdr->checksum = 0;
    hdr->un.echo.id = (u_int16_t)getpid();
    hdr->un.echo.sequence = -1;

    struct timeval ts;
    gettimeofday(&ts, NULL);

    memcpy(packet + sizeof(struct icmphdr), &ts, sizeof(struct timeval));

    u_int8_t *data = (u_int8_t *)(packet + sizeof(struct icmphdr));
    for (size_t i = sizeof(struct timeval); i < ICMP_DATA_SIZE; i++)
        data[i] = (u_int8_t)i;
    return (packet);
}

void ft_setup_send(void *packet, size_t packet_len)
{
    struct icmphdr *hdr = (struct icmphdr *)packet;
    hdr->un.echo.sequence++;

    struct timeval ts;
    gettimeofday(&ts, NULL);
    memcpy(packet + sizeof(struct icmphdr), &ts, sizeof(struct timeval));

    hdr->checksum = 0;
    hdr->checksum = ft_checksum((struct icmphdr *)packet, packet_len);
}

void handle_sigint(int signal)
{
    (void)signal;
    g_stop = 1;
}

bool ft_ping(char *destination)
{

    signal(SIGINT, handle_sigint);

    struct addrinfo *adresse;
    adresse = ft_get_addr(destination);
    if (!adresse)
        return (false);


    int sockfd;
    sockfd = ft_create_socket();
    if (sockfd == -1)
    {
        freeaddrinfo(adresse);
        return false;
    }


    size_t packet_len = sizeof(struct icmphdr) + ICMP_DATA_SIZE;
    char *packet;
    packet = ft_create_packet();
    if (!packet)
    {
        freeaddrinfo(adresse);
        close(sockfd);
        return false;
    }
    struct icmphdr *hdr = (struct icmphdr *)packet;
    int count_receive = 0;
    int count_send = 0;
    struct timeval start_ts;
    gettimeofday(&start_ts, NULL);

    while (!g_stop)
    {
        ft_setup_send(packet, packet_len);
        ssize_t sent = sendto(sockfd, packet, packet_len, 0, adresse->ai_addr, adresse->ai_addrlen);
        if (sent < 0)
        {
            write(2, strerror(errno), strlen(strerror(errno)));
            write(2, "\n", 1);
            break;
        }
        count_send++;

        while (!g_stop)
        {
            struct sockaddr_in receive_addr;
            socklen_t receive_addr_len = sizeof(receive_addr);
            ssize_t size_receive;
            u_int8_t receive_buff[1024];

            size_receive = recvfrom(sockfd, receive_buff, sizeof(receive_buff), 0,
                (struct sockaddr *)&receive_addr, &receive_addr_len);
            if (size_receive < 0)
            {
                if (errno != EINTR)
                {
                    write(2, strerror(errno), strlen(strerror(errno)));
                    write(2, "\n", 1);
                }
                continue;
            }

            u_int8_t ip_header_len = (receive_buff[0] & 0x0F) * 4;
            struct icmphdr *receive_icmp = (struct icmphdr *)(receive_buff + ip_header_len);
            if (receive_icmp->un.echo.id != hdr->un.echo.id)
                continue;
            else
            {
                count_receive++;
                struct timeval receive_ts;
                struct timeval now;
                long rtt_usec;
                struct ip *iph;
                iph = (struct ip *)receive_buff;

                memcpy(&receive_ts, receive_buff + ip_header_len + sizeof(struct icmphdr),
                    sizeof(struct timeval));
                gettimeofday(&now, NULL);
                rtt_usec = (now.tv_sec - receive_ts.tv_sec) * 1000000
                    + (now.tv_usec - receive_ts.tv_usec);
                char *ip_str = inet_ntoa(iph->ip_src);
                printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%ld ms\n",
                    size_receive - ip_header_len, ip_str, ip_str,
                    hdr->un.echo.sequence, iph->ip_ttl, rtt_usec / 1000);
                break;
            }
        }
        sleep(1);
    }

    struct timeval end_ts;
    gettimeofday(&end_ts, NULL);
    int total_time = ((end_ts.tv_sec - start_ts.tv_sec) * 1000000 + (end_ts.tv_usec - start_ts.tv_usec)) / 1000;
    printf("--- %s ping statisctics ---\n", destination);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %d ms\n", count_send, count_receive, ((count_send - count_receive) * 100 / count_send), total_time);
    if (packet)
        free(packet);
    freeaddrinfo(adresse);
    close(sockfd);
    return (0);
}