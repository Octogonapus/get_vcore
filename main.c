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
Based on https://github.com/ajaiantilal/i7z
GNU General Public License v2.0
*/
uint64_t get_msr_value(int32_t cpu, uint32_t reg, uint8_t highbit, uint8_t lowbit) {
    char msr_file_name[64];
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    const int fd = open(msr_file_name, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open CPU: %d\n", cpu);
        exit(1);
    }

    uint64_t data;
    if (pread(fd, &data, sizeof data, reg) != sizeof data) {
        perror("rdmsr:pread");
        exit(127);
    }

    close(fd);

    const int bits = highbit - lowbit + 1;
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

    return (data);
}

float Read_Voltage_CPU(int32_t cpu_num) {
    const uint64_t val = get_msr_value(cpu_num, MSR_PERF_STATUS, 47, 32);
    return (float)val / (float)(1 << 13);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *endptr;
        errno = 0;
        const long cpu = strtol(argv[i], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || cpu < 0 || cpu > INT32_MAX) {
            fprintf(stderr, "Invalid CPU number: %s\n", argv[i]);
            return 1;
        }

        printf("%1.2f\n", Read_Voltage_CPU((int32_t)cpu));
    }

    return 0;
}
