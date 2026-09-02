CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -D_REENTRANT
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm
SRCS    = main.c vec2.c camera.c scaler.c scene.c render.c
OBJS    = $(SRCS:.c=.o)
NAME    = scaler

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
