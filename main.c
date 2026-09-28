#include "ft_ping.h"

int main(int argc, char **argv)
{
    bool ret;

    ret = 0;
    
    if (argc != 2)
    {
        write(2, "Usage: ./ft_ping <destination>\n", 32);
        return (1);
    }
    ret = ft_ping(argv[1]);
    return (ret);
}