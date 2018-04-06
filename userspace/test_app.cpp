#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

#define DEVICE "/dev/test_device"

void print_buffer(char * buffer, ssize_t size) {
  for (int index = 0; index < size; index++){
    printf("%c", buffer[index]);
  }
}

int main() {
  int i, fd;
  char ch;
  char read_buffer[100] = "";
  //std::string buffer;

  fd = open(DEVICE, O_RDWR); // Open for reading and writing
  if (fd == -1) {
    printf("File %s either does not exist or has been opened by another process\n", DEVICE);
    exit(-1);
  }

    // Prompt for data
    // printf("r = read from device\nw = write to device\nenter command: ");
    // scanf("%c", &ch);

    // switch(ch) {
    //   case 'w':
    //     printf("enter data: ");
    //     std::cin >> buffer;
    //     write(fd, buffer.c_str(), buffer.length());
    //     break;
    //   case 'r':
    //     read(fd, read_buffer, sizeof(read_buffer));
    //     printf("device: %s\n", read_buffer);
    //     break;
    //   default:
    //     printf("command not recognized\n");
    //     break;
    // }
  
  int size;

  write(fd, "Hello", 5);
  write(fd, " World!", 7);

  size = read(fd, read_buffer, sizeof(read_buffer));
  print_buffer(read_buffer, size);
  printf("\n");

  size = write(fd, "Goodbye", 7);

  size = read(fd, read_buffer, sizeof(read_buffer));
  print_buffer(read_buffer, size);
  printf("\n");

  close(fd);

  return 0;
}