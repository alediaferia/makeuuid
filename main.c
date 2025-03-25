#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Unable to open urandom block device for reading. Exiting.\n");
        perror(0);
        return -1;
    }

    uint32_t uuid_chunks[4];
    ssize_t random_read = read(fd, &uuid_chunks, sizeof(uuid_chunks));
    if (random_read == -1) {
        fprintf(stderr, "Unable to read from urandom. Exiting.\n");
        perror(0);
        return -1;
    } else if ((unsigned long)random_read < sizeof(uuid_chunks)) {
        fprintf(stderr, "Unable to read enough bytes from urandom. Exiting.\n");
        return -1;
    }
    close(fd);

    /* set version bit (4) */
    uuid_chunks[1] = (uuid_chunks[1] & 0xffff0fff) | 0x00004000;
    /* set the var field */
    uuid_chunks[2] = (uuid_chunks[2] & 0x3fffffff) | 0x80000000;

    char output[37];

    int written = snprintf(output, sizeof(output),
        "%08x-%04x-%04x-%04x-%04x%08x",
        uuid_chunks[0],
        (uint16_t)(uuid_chunks[1] >> 16),
        (uint16_t)(uuid_chunks[1]),
        (uint16_t)(uuid_chunks[2] >> 16),
        (uint16_t)(uuid_chunks[2]),
        uuid_chunks[3]);

    if (written != 36) {
        fprintf(stderr, "Error writing UUID to output buffer. Exiting.\n");
        return -1;
    }

    printf("%s\n", output);
    
    return 0;
}
