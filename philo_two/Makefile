NAME = philo_two
CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(NAME)
	
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c $(INCS)
		$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
