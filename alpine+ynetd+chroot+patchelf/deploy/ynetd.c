/*
 *
 * Copyright (c) 2015-2017 Lorenz Panny
 *
 * This is ynetd version 0.1.2 (24 May 2017).
 * Check for newer versions at https://yx7.cc/code.
 * Please report bugs to y@yx7.cc.
 *
 * This program is released under the MIT license; see license.txt.
 *
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/resource.h>

__attribute__((noreturn)) void version()
{
    printf("This is ynetd version 0.1.2 (24 May 2017).\n");
    exit(0);
}

__attribute__((noreturn)) void help(int st)
{
    bool tty = isatty(fileno(stdout));

    printf("\n");
    printf("    %synetd: a minimalistic inetd%s\n",
            tty ? "\x1b[32m" : "", tty ? "\x1b[0m" : "");
    printf("    ---------------------------\n\n");
    printf("    %sinvocation:%s ynetd [$opts] $cmd\n\n",
            tty ? "\x1b[33m" : "", tty ? "\x1b[0m" : "");

    printf("    %sflags:%s\n",
            tty ? "\x1b[33m" : "", tty ? "\x1b[0m" : "");
    printf("-h              "
            "this help text\n");
    printf("-a $addr        "
            "IP address to bind to (default :: and 0.0.0.0)\n");
    printf("-p $port        "
            "TCP port to bind to (default 1024)\n");
    printf("-u $user        "
            "username (default current)\n");
    printf("-d $dir         "
            "working directory (default user's home if -u else current)\n");
    printf("-sh [yn]        "
            "invoke /bin/sh to execute $cmd? (default y)\n");
    printf("-si [yn]        "
            "use socket as stdin? (default y)\n");
    printf("-so [yn]        "
            "use socket as stdout? (default y)\n");
    printf("-se [yn]        "
            "use socket as stderr? (default n)\n");
    printf("-lt $lim        "
            "limit cpu time in seconds (default unchanged)\n");
    printf("-lm $lim        "
            "limit amount of memory in bytes (default unchanged)\n");
    printf("-lp $lim        "
            "limit number of processes (default unchanged)\n");
    printf("-rn $val        "
            "set process priority (default unchanged)\n");
    printf("$cmd            "
            "command\n");
    printf("                NOTE: $cmd is executed relative to $dir!\n");
    printf("                      if in doubt, use absolute paths only.\n");
    printf("\n");
    exit(st);
}

#define die(S) do { perror(S); exit(-1); } while (0)

struct config {
    struct {
        bool set;
        uid_t uid;
        gid_t gid;
    } ids;

    int family;
    union {
        struct in6_addr ipv6;
        struct in_addr ipv4;
    } addr;
    in_port_t port;

    char *cmd;
    char *dir;
    bool shell;
    bool in, out, err;
    struct {
        bool set;
        rlim_t lim;
    } cpu, mem, proc;
    struct {
        bool set;
        int val;
    } nice;
};

void parse_args(size_t argc, char **argv, struct config *cfg)
{
    struct passwd spw, *pw;
    char pwbuf[0x100];

    /* note: to avoid copying all the strings from argv[] to cfg,
     * we only write pointers to the arguments into cfg. since only
     * main() calls this function, these references are guaranteed
     * to stay valid for the lifetime of the program. */

#define ARG_YESNO(S, L, V) \
    else if (!strcmp(argv[i], (S)) || !strcmp(argv[i], (L))) { \
        if (++i >= argc) \
            help(1); \
        if (argv[i][1] || (*argv[i] != 'y' && *argv[i] != 'n')) \
            help(1); \
        (V) = *argv[i++] == 'y'; \
    }

#define ARG_NUM(S, L, V, P) \
    else if (!strcmp(argv[i], (S)) || !strcmp(argv[i], (L))) { \
        if (++i >= argc) \
            help(1); \
        (V) = strtol(argv[i++], NULL, 10); \
        if (P) \
            * (bool *) (P) = true; \
    }

    for (size_t i = 1; i < argc; ) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help(0);
        }
        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            version();
        }
        ARG_YESNO("-sh", "--shell", cfg->shell)
        ARG_YESNO("-si", "--stdin", cfg->in)
        ARG_YESNO("-so", "--stdout", cfg->out)
        ARG_YESNO("-se", "--stderr", cfg->err)
        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--addr")) {
            if (++i >= argc)
                help(1);
            if (1 == inet_pton(AF_INET6, argv[i], &cfg->addr.ipv6))
                cfg->family = AF_INET6;
            else if (1 == inet_pton(AF_INET, argv[i], &cfg->addr.ipv4))
                cfg->family = AF_INET;
            else
                die("inet_pton");
            ++i;
        }
        ARG_NUM("-p", "--port", cfg->port, NULL)
        else if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--user")) {
            if (++i >= argc)
                help(1);
            if (getpwnam_r(argv[i++], &spw, pwbuf, sizeof(pwbuf), &pw) || !pw)
                die("getpwnam_r");
            cfg->ids.uid = pw->pw_uid;
            cfg->ids.gid = pw->pw_gid;
            cfg->ids.set = true;
            if (!cfg->dir) {
                /* note: pw->pw_dir is local, so we need to copy it. */
                /* note: ideally we should free() this, but it will
                 * exist until the parent dies anyway. */
                cfg->dir = strdup(pw->pw_dir);
            }
        }
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir")) {
            if (++i >= argc)
                help(1);
            cfg->dir = argv[i++];
        }
        ARG_NUM("-lt", "--limit-time", cfg->cpu.lim, &cfg->cpu.set)
        ARG_NUM("-lm", "--limit-memory", cfg->mem.lim, &cfg->mem.set)
        ARG_NUM("-lp", "--limit-processes", cfg->proc.lim, &cfg->proc.set)
        ARG_NUM("-rn", "--renice", cfg->nice.val, &cfg->nice.set)
        else if (!cfg->cmd) {
            cfg->cmd = argv[i++];
        }
        else {
            help(1);
        }
    }

#undef ARG_YESNO
#undef ARG_NUM

    if (!cfg->cmd)
        help(1);
}

int bind_listen(struct config const cfg)
{
    int const one = 1;
    int lsock;
    union {
       struct sockaddr_in6 ipv6;
       struct sockaddr_in ipv4;
    } addr;
    socklen_t addr_len;

    if (0 > (lsock = socket(cfg.family, SOCK_STREAM, 0)))
        die("socket");

    if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)))
        die("setsockopt");

    switch (cfg.family) {
    case AF_INET6:
        addr.ipv6.sin6_family = cfg.family;
        addr.ipv6.sin6_addr = cfg.addr.ipv6;
        addr.ipv6.sin6_port = htons(cfg.port);
        addr_len = sizeof(addr.ipv6);
        break;
    case AF_INET:
        addr.ipv4.sin_family = cfg.family;
        addr.ipv4.sin_addr = cfg.addr.ipv4;
        addr.ipv4.sin_port = htons(cfg.port);
        addr_len = sizeof(addr.ipv4);
        break;
    default:
        fprintf(stderr, "bad address family?!\n");
        exit(-1);
    }

    if (bind(lsock, (struct sockaddr *) &addr, addr_len))
        die("bind");

    if (listen(lsock, 16))
        die("listen");

    return lsock;
}

void handle_connection(struct config const cfg, int sock)
{
    struct rlimit rlim;

    /* set resource limits */
    if (cfg.cpu.set) {
        rlim.rlim_cur = rlim.rlim_max = cfg.cpu.lim;
        if (0 > setrlimit(RLIMIT_CPU, &rlim))
            die("setrlimit");
    }
    if (cfg.mem.set) {
        rlim.rlim_cur = rlim.rlim_max = cfg.mem.lim;
        if (0 > setrlimit(RLIMIT_AS, &rlim))
            die("setrlimit");
    }
    if (cfg.proc.set) {
        rlim.rlim_cur = rlim.rlim_max = cfg.proc.lim;
        if (0 > setrlimit(RLIMIT_NPROC, &rlim))
            die("setrlimit");
    }

    /* renice */
    if (cfg.nice.set && setpriority(PRIO_PROCESS, 0, cfg.nice.val))
        die("setpriority");

    /* drop privileges */
    if (cfg.ids.set) {
        if (setgroups(0, NULL))
            die("setgroups");
        if (setgid(cfg.ids.gid))
            die("setgid");
        if (setuid(cfg.ids.uid))
            die("setuid");
    }

    /* change working directory */
    if (cfg.dir && chdir(cfg.dir))
        die("chdir");

    /* duplicate socket to stdio */
    if (cfg.in && fileno(stdin) != dup2(sock, fileno(stdin)))
        die("dup2");
    if (cfg.out && fileno(stdout) != dup2(sock, fileno(stdout)))
        die("dup2");
    if (cfg.err && fileno(stderr) != dup2(sock, fileno(stderr)))
        die("dup2");
    if (close(sock))
        die("close");

    /* FIXME does nobody care about the environment? */

    /* execute command */
    if (cfg.shell) {
        execle("/bin/sh", "sh", "-c", cfg.cmd, NULL, NULL);
        die("execle");
    }
    else {
        /* FIXME support more arguments? */
        execle(cfg.cmd, cfg.cmd, NULL, NULL);
        die("execle");
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    int lsock, sock;
    struct sigaction sigact;

    /* configuration options */
    struct config cfg = {
        .ids = {.set = false},

        .family = AF_INET6,
        .addr = {.ipv6 = in6addr_any},
        .port = 1024,

        .cmd = NULL,
        .dir = NULL,
        .shell = true,
        .in = true, .out = true, .err = false,
        .cpu = {.set = false}, .mem = {.set = false}, .proc = {.set = false},
        .nice = {.set = false},
    };

    /* "parse" arguments */
    parse_args(argc, argv, &cfg);

    /* do not turn dead children into zombies */
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sigact, 0))
        die("sigaction");

    /* set up listening socket */
    lsock = bind_listen(cfg);

    /* accept loop */
    while (1) {

        if (0 > (sock = accept(lsock, NULL, NULL)))
            continue;

        if ((pid = fork())) {
            /* parent */
            /* note: if the fork failed, we just drop the connection
             * and continue as usual, so we don't catch that case. */
            if (close(sock))
                die("close");
            continue;
        }

        /* child */
        if (close(lsock))
            die("close");

        /* detach from terminal */
        if (0 > setsid())
            die("setsid");

        handle_connection(cfg, sock);

    }
}
