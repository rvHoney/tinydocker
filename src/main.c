#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

typedef struct
{
    char *hostname;
    char *rootfs;
    char **command;
} ContainerArgs;

int init_container(void *arg)
{
    ContainerArgs *args = (ContainerArgs *)arg;

    printf("🔧 Setting hostname to '%s'\n", args->hostname);
    if (sethostname(args->hostname, strlen(args->hostname)) == -1)
    {
        perror("sethostname failed");
        return 1;
    }

    printf("🔒 Setting up namespaces...\n");
    if (chroot(args->rootfs) == -1)
    {
        perror("chroot failed");
        return 1;
    }

    printf("📂 Changing root directory to '/'...\n");
    if (chdir("/") == -1)
    {
        perror("chdir failed");
        return 1;
    }

    printf("🔗 Mounting proc filesystem...\n");
    if (mount("proc", "/proc", "proc", 0, NULL) == -1)
    {
        perror("mount proc failed");
        return 1;
    }

    pid_t shell_pid = fork();
    if (shell_pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (shell_pid == 0)
    {
        printf("🖥️ Starting shell in the container...\n");
        execvp(args->command[0], args->command);
        perror("execvp failed");
        exit(1);
    }
    else
    {
        waitpid(shell_pid, NULL, 0);
        printf("🧹 Unmounting /proc...\n");
        if (umount("/proc") == -1)
        {
            perror("umount /proc");
        }
        return 0;
    }
}

int main(int argc, char *argv[])
{
    ContainerArgs args = { .hostname = "tinydocker",
                           .rootfs = "./rootfs",
                           .command = (char *[]){ "/bin/sh", NULL } };

    int i = 1;
    int cmd_start = 0;
    while (i < argc)
    {
        if (strcmp(argv[i], "--") == 0)
        {
            cmd_start = i + 1;
            break;
        }
        else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc)
        {
            args.hostname = argv[++i];
        }
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc)
        {
            args.rootfs = argv[++i];
        }
        else
        {
            fprintf(
                stderr,
                "Usage: %s [-h hostname] [-r rootfs] -- command [args...]\n",
                argv[0]);
            exit(EXIT_FAILURE);
        }
        i++;
    }

    if (cmd_start > 0)
    {
        int cmd_argc = argc - cmd_start;
        args.command = malloc((cmd_argc + 1) * sizeof(char *));
        if (!args.command)
        {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < cmd_argc; j++)
        {
            args.command[j] = argv[cmd_start + j];
        }
        args.command[cmd_argc] = NULL;
    }

    printf("🐟 Welcome to TinyDocker!\n");

    pid_t pid = clone(init_container, child_stack + STACK_SIZE,
                      CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC
                          | CLONE_NEWNET | SIGCHLD,
                      &args);

    if (pid == -1)
    {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    printf("📦 Container created with PID: %ld\n", (long)pid);

    if (waitpid(pid, NULL, 0) == -1)
    {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}