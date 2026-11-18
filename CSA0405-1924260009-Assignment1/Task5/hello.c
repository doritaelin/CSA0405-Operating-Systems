#include <fcntl.h>
#include <unistd.h>
int main() {
    char buf[100];
    int fd = open("test.txt", O_RDONLY);
    int n = read(fd, buf, 100);
    write(1, buf, n);
    close(fd);
    return 0;
}
