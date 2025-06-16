#include "cli.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HOSTNAME "sample-container"
#define DEFAULT_ROOTFS "./rootfs"
#define DEFAULT_CPUS 1
#define DEFAULT_MEMORY (1024 * 1024 * 1024) // 1 GB

void print_usage(const char *program_name)
{
    printf("Usage: %s [OPTIONS] -- COMMAND [ARGS...]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --hostname NAME    Set container hostname (default: %s)\n",
           DEFAULT_HOSTNAME);
    printf("  -r, --rootfs PATH      Set root filesystem path (default: %s)\n",
           DEFAULT_ROOTFS);
    printf(
        "  -c, --cpus NUM         Set maximum number of CPUs (default: %d)\n",
        DEFAULT_CPUS);
    printf("  -m, --memory SIZE      Set maximum memory in MB (default: %d)\n",
           (int)(DEFAULT_MEMORY / (1024 * 1024)));
    printf("  --help                 Display this help message\n");
    printf("\nExamples:\n");
    printf("  %s -- /bin/sh\n", program_name);
    printf("  %s -h mycontainer -c 2 -m 512 -- /bin/bash\n", program_name);
}

int parse_args(int argc, char *argv[], ContainerArgs *args)
{
    static struct option long_options[] = {
        { "hostname", required_argument, 0, 'h' },
        { "rootfs", required_argument, 0, 'r' },
        { "cpus", required_argument, 0, 'c' },
        { "memory", required_argument, 0, 'm' },
        { "help", no_argument, 0, '?' },
        { 0, 0, 0, 0 }
    };

    // Set default values
    args->hostname = DEFAULT_HOSTNAME;
    args->rootfs = DEFAULT_ROOTFS;
    args->max_cpus = DEFAULT_CPUS;
    args->max_memory = DEFAULT_MEMORY;
    args->process = NULL;

    int opt;
    int option_index = 0;
    while (
        (opt = getopt_long(argc, argv, "h:r:c:m:", long_options, &option_index))
        != -1)
    {
        switch (opt)
        {
        case 'h':
            args->hostname = optarg;
            break;
        case 'r':
            args->rootfs = optarg;
            break;
        case 'c': {
            char *endptr;
            errno = 0;
            long cpus = strtol(optarg, &endptr, 10);
            if (errno != 0 || *endptr != '\0' || cpus <= 0)
            {
                fprintf(stderr, "Invalid CPU count: %s\n", optarg);
                return EXIT_FAILURE;
            }
            args->max_cpus = (int)cpus;
            break;
        }
        case 'm': {
            char *endptr;
            errno = 0;
            long memory = strtol(optarg, &endptr, 10);
            if (errno != 0 || *endptr != '\0' || memory <= 0)
            {
                fprintf(stderr, "Invalid memory size: %s\n", optarg);
                return EXIT_FAILURE;
            }
            args->max_memory = memory * 1024 * 1024; // Convert MB to bytes
            break;
        }
        case '?':
            print_usage(argv[0]);
            return EXIT_FAILURE;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Check if there are any arguments after --
    if (optind >= argc)
    {
        fprintf(stderr, "Error: No command specified\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // All remaining arguments form the command
    args->process = &argv[optind];

    return EXIT_SUCCESS;
}