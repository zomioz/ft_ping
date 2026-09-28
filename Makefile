NAME = ft_ping

SRCS = main.c \
		ft_ping.c

OBJDIR = objects
OBJTS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

HEADER = ft_ping.h
CFLAGS = -Wall -Wextra -Werror -g

$(NAME): $(OBJTS)
	cc -o $(NAME) $(OBJTS)

RM	= rm -f

all:	${NAME}

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c $(HEADER) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	${RM} -r ${OBJDIR}

fclean:	clean
	${RM} ${NAME}

re:	fclean all

.PHONY: all clean fclean re