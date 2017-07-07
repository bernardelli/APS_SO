#pragma once

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

int send_position(int fd, uint16_t position);

int set_interface_attribs(int fd, int speed);

void set_mincount(int fd, int mcount);
