#define _GNU_SOURCE
#include "cli.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../container/container.h"

#define DEFAULT_HOSTNAME "container"
#define DEFAULT_ROOTFS "./rootfs"
#define DEFAULT_CPUS 1
#define DEFAULT_MEMORY (512 * 1024 * 1024) // 512MB in bytes

static void print_usage(const char *program_name)
{
    printf("Usage: %s [OPTIONS] -- COMMAND [ARGS...]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --hostname NAME   Set container hostname (default: %s)\n",
           DEFAULT_HOSTNAME);
    printf("  -r, --rootfs PATH     Set root filesystem path (default: %s)\n",
           DEFAULT_ROOTFS);
    printf("  -c, --cpus N          Set maximum number of CPUs (default: %d)\n",
           DEFAULT_CPUS);
    printf("  -m, --memory SIZE     Set maximum memory in MB (default: %d)\n",
           (int)(DEFAULT_MEMORY / (1024 * 1024)));
    printf("  --help                Display this help message\n\n");
    printf("Examples:\n");
    printf("  # Run a basic container\n");
    printf("  sudo %s -- /bin/bash\n\n", program_name);
    printf("  # Run with custom hostname and resource limits\n");
    printf("  sudo %s -h myapp -c 2 -m 1024 -- /bin/bash\n", program_name);
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
        case 'c':
            args->max_cpus = strtol(optarg, NULL, 10);
            if (args->max_cpus <= 0)
            {
                fprintf(stderr, "Error: CPU count must be positive\n");
                return EXIT_FAILURE;
            }
            break;
        case 'm':
            args->max_memory =
                atol(optarg) * 1024 * 1024; // Convert MB to bytes
            if (args->max_memory <= 0)
            {
                fprintf(stderr, "Error: Memory size must be positive\n");
                return EXIT_FAILURE;
            }
            break;
        case '?':
            print_usage(argv[0]);
            return EXIT_FAILURE;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Check if we have a command after --
    if (optind >= argc)
    {
        // No command specified, use default
        static char *default_cmd[] = { "/bin/sh", NULL };
        args->process = default_cmd;
    }
    else
    {
        args->process = &argv[optind];
    }

    return EXIT_SUCCESS;
}