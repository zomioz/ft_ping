#include "ft_ping.h"

void ft_init_pars(struct s_parsing *pars)
{
    pars->destination = NULL;
    pars->flag_v = false;
    pars->flag_c = false;
    pars->flag_c_count = 0;
    pars->flag_q = false;
    pars->flag_w = false;
    pars->flag_w_count = 0;
    pars->flag_i = false;
    pars->flag_i_count = 0;
    pars->flag_W = false;
    pars->flag_W_count = 0;    
    pars->flag_question_mark = false;
}

void ft_print_pars(struct s_parsing *pars)
{
    printf("Struct pars\n");
    printf("flag_v = %d\n", pars->flag_v);
    printf("flag_c = %d\tflag_c_count = %d\n", pars->flag_c, pars->flag_c_count);
    printf("flag_q = %d\n", pars->flag_q);
    printf("flag_w = %d\tflag_w_count = %d\n", pars->flag_w, pars->flag_w_count);
    printf("flag_i = %d\tflag_i_count = %d\n", pars->flag_i, pars->flag_i_count);
    printf("flag_W = %d\tflag_W_count = %d\n", pars->flag_W, pars->flag_W_count);
}

bool ft_handle_getopt_error(char *optarg, bool handle_atoi_error)
{
    if (!optarg)
        printf("ping : option requires an argument -- 'c'\n");
    if (optarg && optarg[0] == '-')
        printf("ping : invalid value (\'%s\' near \'%s\')\n", optarg, optarg);
    if (atoi(optarg) <= 0)
        print("ping : option value %d", atoi(optarg))
}

int main(int argc, char **argv)
{
    int opt;
    struct s_parsing *pars;
    bool ret;

    (void)argc;
    (void)argv;
    (void)opt;
    ret = false;
    pars = malloc(sizeof(struct s_parsing));
    ft_init_pars(pars);
    if (!pars)
    {
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return (1);
    }

    while ((opt = getopt(argc, argv, ":vc:qw:i:W:")) != -1)
    {
        switch(opt)
        {
            case 'v':
                pars->flag_v = true;
                break;
            case 'c':
                pars->flag_c = true;
                pars->flag_c_count = atoi(optarg);
                break;
            case 'q':
                pars->flag_q = true;
                break;
            case 'w':
                pars->flag_w = true;
                pars->flag_w_count = atoi(optarg);
                break;
            case 'i':
                pars->flag_i = true;
                pars->flag_i_count = atoi(optarg);
                break;
            case 'W':
                pars->flag_W = true;
                pars->flag_W_count = atoi(optarg);
                break;
            case ':':
                fprintf(stderr, "ping: option requires an argument -- '%c'\n", opt);
                return (-1); // Erreur d'argument manquant
            case '?':
                fprintf(stderr, "ping: invalid option -- '%c'\n", opt);
                return (-1); // Option inconnue
            default:
                return (-1);
        }
    }

    ft_print_pars(pars);
    return (ret);
}