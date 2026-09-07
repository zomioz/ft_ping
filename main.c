#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>

void ft_print_struct(struct icmphdr *tmp)
{
    printf("struct->type : %d\n", tmp->type);
    printf("struct->code : %d\n", tmp->code);
    printf("struct->checksum : %d\n", tmp->checksum);
    printf("struct->un.echo.id : %d\n", tmp->un.echo.id);
    printf("struct->un.echo.sequence : %d\n", tmp->un.echo.sequence);
}

int main(void)
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return(1);
    }
    
    struct icmphdr *tmp;

    tmp = malloc(sizeof(struct icmphdr));
    tmp->type = 0;
    tmp->code = 0;
    tmp->checksum = 0;
    tmp->un.echo.id = getpid();
    tmp->un.echo.sequence = 0;

    ft_print_struct(tmp);

    free(tmp);
    close(sockfd);
    return 0;
}