#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

uint32_t uuid[] = {0, 0, 0, 0};

int main(int argc, char *argv[]) {
    int fd = open("/dev/urandom", O_RDONLY, O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Unable to open urandom block device for reading. Exiting.\n");
        return -1;
    }

    ssize_t r = read(fd, &uuid, sizeof(uuid));
    if (r < sizeof(uuid)) {
        fprintf(stderr, "Unable to read enough bytes from urandom. Exiting.\n");
        return -1;
    }

    close(fd);
    
    /* set version bit */
    uuid[1] = (uuid[1] & 0xffff0fff) | 0x00004000;

    /* set the var field */
    uuid[2] = (uuid[2] & 0x3fffffff) | 0x80000000;

    /* Print as per ABNF in RFC 9562 */
    printf("%08x-", uuid[0]);
    printf("%04x-", (uint16_t)(uuid[1] >> 16));
    printf("%04x-", (uint16_t)(uuid[1]));
    printf("%04x-", (uint16_t)(uuid[2] >> 16));
    printf("%04x", (uint16_t)(uuid[2]));
    printf("%08x\n", uuid[3]);

    return 0;
}
