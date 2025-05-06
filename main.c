#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define print_errno(str)                                                                                               \
    fprintf(stderr, "%s:%d <%s> " str " %d %s\n", __FILE__, __LINE__, __func__, errno, strerror(errno))

#define MSR_PERF_STATUS 0x198

/*
Taken from https://github.com/ajaiantilal/i7z

GNU General Public License v2.0
*/
uint64_t get_msr_value(int cpu, uint32_t reg, unsigned int highbit, unsigned int lowbit, int *error_indx) {
    uint64_t data;
    int fd;
    //  char *pat;
    //  int width;
    char msr_file_name[64];
    int bits;
    *error_indx = 0;

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_RDONLY);
    if (fd < 0) {
        if (errno == ENXIO) {
            // fprintf (stderr, "rdmsr: No CPU %d\n", cpu);
            *error_indx = 1;
            return 1;
        } else if (errno == EIO) {
            // fprintf (stderr, "rdmsr: CPU %d doesn't support MSRs\n", cpu);
            *error_indx = 1;
            return 1;
        } else {
            // perror ("rdmsr:open");
            *error_indx = 1;
            return 1;
            // exit (127);
        }
    }

    if (pread(fd, &data, sizeof data, reg) != sizeof data) {
        perror("rdmsr:pread");
        exit(127);
    }

    close(fd);

    bits = highbit - lowbit + 1;
    if (bits < 64) {
        /* Show only part of register */
        data >>= lowbit;
        data &= (1ULL << bits) - 1;
    }

    /* Make sure we get sign correct */
    if (data & (1ULL << (bits - 1))) {
        data &= ~(1ULL << (bits - 1));
        data = -data;
    }

    *error_indx = 0;
    return (data);
}

float Read_Voltage_CPU(int cpu_num) {
    int error_indx;
    unsigned long val = get_msr_value(cpu_num, MSR_PERF_STATUS, 47, 32, &error_indx);
    return (float)val / (float)(1 << 13);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *endptr;
        errno = 0;
        long cpu = strtol(argv[i], &endptr, 10);

        if (errno != 0 || *endptr != '\0' || cpu < 0) {
            fprintf(stderr, "Invalid CPU number: %s\n", argv[i]);
            return 1;
        }

        printf("%1.2f\n", Read_Voltage_CPU(cpu));
    }

    return 0;
}
