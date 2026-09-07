CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -D_REENTRANT
BUILD   = -g -O0
RELEASE = -O2
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm
SRCS    = main.c vec2.c camera.c scaler.c scene.c render.c
OBJS    = $(SRCS:.c=.o)
NAME    = scaler

.PHONY: all release clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(BUILD) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(BUILD) -c $< -o $@

release:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory BUILD="$(RELEASE)"
	strip $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
