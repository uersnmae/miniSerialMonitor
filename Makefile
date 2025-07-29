CC = cc
CFLAGS = -Wall -Wextra -Werror -Iincludes

# VPATH =

# INCLUDE = 
SRC = main.c \

PRG_NAME = miniSerialMonitor
NAME = $(PRG_NAME)

OBJ_DIR = obj

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

all: $(PRG_NAME)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(PRG_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(PRG_NAME)

re: fclean all

compile:
	@echo "[" > compile_commands.json; \
	SEP=""; \
	for SRC in $$(find srcs -name "*.c"); do \
		echo "$$SEP{" >> compile_commands.json; \
		echo "  \"directory\": \"$$PWD\"," >> compile_commands.json; \
		echo "  \"command\": \"cc -Wall -Wextra -Werror -Isrc -Iinclude -c $$SRC -o /dev/null\"," >> compile_commands.json; \
		echo "  \"file\": \"$$SRC\"" >> compile_commands.json; \
		echo "}" >> compile_commands.json; \
		SEP=","; \
	done; \
	echo "]" >> compile_commands.json

del_compile:
	rm -f compile_commands.json

.PHONY: all clean fclean re compile del_compile
