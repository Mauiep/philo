# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: admaupie <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/23 15:30:28 by admaupie          #+#    #+#              #
#    Updated: 2022/10/25 22:00:29 by admaupie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo

CC = gcc -pthread -Wall -Werror -Wextra

INCLUDES = -I./

SRCS =	philo.c

OBJ = $(SRCS:.c=.o)

.PHONY : all clean fclean re

all : $(NAME)

$(NAME): $(OBJ)
	$(CC) $(INCLUDES) -o $(NAME) $(SRCS)

clean :
	/bin/rm -f $(OBJ)

fclean : clean
	/bin/rm -f $(NAME)

re : fclean all
