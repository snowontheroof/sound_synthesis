# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sojala <sojala@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/05 11:54:28 by jvarila           #+#    #+#              #
#    Updated: 2025/09/05 16:55:31 by sojala           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:= minisynth

CXX			:= c++
CXX_FLAGS	:= -Wall -Wextra -Werror -std=c++20
DEBUG_FLAGS	:= -g
# ---------------------------------------------------------------------------- #
INC_DIR		:= ./inc
INC_FLAGS	:= -I$(INC_DIR)
HEADERS		:= $(INC_DIR)/samples.hpp
# ---------------------------------------------------------------------------- #
SRC_DIR	:=	./src
OBJ_DIR	:=	./obj
# ---------------------------------------------------------------------------- #
SRC :=	$(SRC_DIR)/samples.cpp	\
		$(SRC_DIR)/parsing.cpp	\
		$(SRC_DIR)/main.cpp
# ---------------------------------------------------------------------------- #
OBJ :=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

.SECONDARY: $(OBJ)
# ---------------------------------------------------------------------------- #
PORTAUDIO_DIR	:= ./lib
PORTAUDIO		:= $(PORTAUDIO_DIR)/libportaudio.a
PORTAUDIO_FLAGS	:= -lrt -lm -lasound -lpulse -lpulse-simple -pthread -lsndio
# ---------------------------------------------------------------------------- #
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) $(PORTAUDIO) $(CXX_FLAGS) $(INC_FLAGS) $(PORTAUDIO_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(INC_FLAGS) -c $< -o $@
# ---------------------------------------------------------------------------- #
clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
# ---------------------------------------------------------------------------- #
debug: CXX_FLAGS += $(DEBUG_FLAGS)
debug: fclean all
# ---------------------------------------------------------------------------- #
.PHONY: all clean fclean re debug
# ---------------------------------------------------------------------------- #
