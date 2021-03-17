#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;
    int selector;
    fd_set reader;
    char buf[1024];
    for (;;) {
        if (std::filesystem::exists("./fifo")) {
            break;
        } else {
            std::cout << "wait for fifo\n";
            sleep(1);
        }
    }
    fd = open("./fifo", O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        std::cout << "nope";
    }
    FD_ZERO(&reader);
    FD_SET(fd, &reader);
    for (;;) {
        selector = select(fd + 1, &reader, 0, 0, 0);
        if (!selector)
            continue;
        if (selector == -1) {
            perror("select error");
            return EXIT_FAILURE;
        }
        if (FD_ISSET(fd, &reader)) {
            std::cout << "Ready state! \n";
            read(fd, buf, sizeof(buf));
            if (strcmp(buf, "end") == 0) {
                std::cout << "End!\n";
                return EXIT_SUCCESS;
            }
            std::cout << buf << "\n";
        }
    }
}
