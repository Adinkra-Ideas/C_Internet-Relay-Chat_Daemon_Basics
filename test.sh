#!/bin/sh
gcc -Wall -Wextra -Werror -pedantic -g3 -fsanitize=address irc_daemon.c -o irc_daemon
