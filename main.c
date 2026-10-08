#include "ft_ping.h"

void ft_init_pars(struct s_parsing *pars)
{
    pars->destination = NULL;
    pars->flag_v = false;
    pars->flag_T = false;
    pars->flag_n = false;
}

void ft_is_valid_flag(char c, struct s_parsing *pars)
{
    if (c == 'v')
        pars->flag_v = true;
    else if (c == 'T')
        pars->flag_T = true;
    else if (c == 'n')
        pars->flag_n = true;
    return ;
}

void ft_handle_flags(char *tmp, struct s_parsing *pars)
{
    int len = strlen(tmp);
    if (!len)
    {
        printf("NULL protection\n");
        return ;
    }


    if (tmp[0] == '-')
    {
        int x = 0;
        while (x < len)
        {
            ft_is_valid_flag(tmp[x], pars);
            x++;
        }
    }
    return ;
}

int main(int argc, char **argv)
{
    struct s_parsing *pars;
    bool ret;

    ret = false;
    pars = malloc(sizeof(struct s_parsing));
    ft_init_pars(pars);
    if (!pars)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return (1);
    }

    int x = 1;
    while(x < argc - 1)
    {
        ft_handle_flags(argv[x], pars);
        x++;
    }
    printf("struct :\nflag_v = %d\nflag_T = %d\nflag_n = %d\n", pars->flag_v, pars->flag_T, pars->flag_n);
    //if (argc != 2)
    //{
    //    write(2, "ft_ping: usage error: Destination address required\n", 51);
    //    return (1);
    //}
    //ret = ft_ping(argv[1]);
    return (ret);
}