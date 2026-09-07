NAME = ft_ping

SRCS = main.c

OBJTS = $(SRCS:.c=.o)

HEADER = -I includes
CFLAGS = -Wall -Wextra -Werror -g

$(NAME): $(OBJTS)
	cc -o $(NAME) $(OBJTS)

RM	= rm -f

all:	${NAME}

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	${RM} ${OBJTS}

fclean:	clean
	${RM} ${NAME}

re:	fclean all

.PHONY: all clean fclean re