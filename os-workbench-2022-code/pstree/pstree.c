#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <stdbool.h>

/*
 * ******************************************************************
 * Read Pid and PPid
 *
 * ------------------------------------------------------------------
 * how to know the diretory with numbers?
 * #include <dirent.h> 
 *
 * write isnumber() function
 * if isnumber():
 *      FILE *fp = fopen(filename, "r");
 *      use fgets() to read lines from /proc/pid/stat
 *      use sscanf() to get Pid and PPid
 *      
 * ******************************************************************
 * Create pstree
 *
 * ------------------------------------------------------------------
 * solution 1: Disjoint set
 * Union-find algs to define relationships between pids and ppids.
 * Define an array whose is max pid_t to store all possible pids.
 *
 * Use  
 *      /proc/pid/status (verbose)
 *      or 
 *      /proc/pid/stat arg1, arg4 (clean)
 * to get Pid and PPid for each process and store them in UF array.
 *
 * Toplogy sort the UF array by counting degree of each process, 
 * may set the top process's id to -1 or itself to identify the 
 * head node,
 * ------------------------------------------------------------------
 * solution 2:
 * Create pstree use tree structure ds directly.
 * Read in a pid number (randomly), then 
 *      struct proc_node tmp = {getppid(), getpid()};
 * ...
 * meet the same problem in print pstree.
 *
 * ******************************************************************
 * Print pstree
 *
 * ------------------------------------------------------------------
 * solution 1:
 * hard to execute topological sort, because the adjoint verteces in
 * tree can not find once find the parent. Also, it's time-consuming 
 * to iterate all array to find descendants. Besides, large array 
 * with large amount empty pid numbers.
 *
 * force solution:
 *  - iterate UF array to count degree of processes (Degarr, negtive 
 *  degree for unexisted process)
 *  - find zero degrees 
 *  - use BFS
 *
 */
#define MAXPID_T 100000

struct proc_node {
    pid_t pid;
    pid_t ppid;
    char filename[20];
}; 

void printPsTree(struct proc_node *procs, pid_t pid, int level, int ops);
int listProc(struct proc_node *procs);
bool isNum(char *s);

int main(int argc, char *argv[]) {
    int ops = 0;
    //Specifying the expected options
    static struct option long_options[] = {
        {"show-pids",       no_argument,    NULL,  'p'},
        {"numeric-sort",    no_argument,    NULL,  'n'},
        {"version",         no_argument,    NULL,  'V'},
        {NULL,              0,              NULL,   0}
    };
    char opt;
    struct proc_node procs[MAXPID_T]; /* store ppid for each process, index indicates the pid. */
    for (int i = 0; i < argc; i++) {
        assert(argv[i]);
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    assert(!argv[argc]);
    /* looping over arguments */
    /* getopt_long() to deal with long option strings. */
    while(-1 != (opt = getopt_long(argc, argv, "pnV", long_options, NULL))){
        /* determine which argument it’s processing */
        switch(opt) {
            case 'p':
                ops = ops | 0x1;
                break;
            case 'n':
                ops = ops | 0x2;
                break;
            case 'V':
                ops = ops | 0x4;
                break;
            default:
                printf("wrong argument\n");
                break;
        }
    }
    /* read in all pids and ppids. */
    if (listProc(procs) == 0) {
        /* use DFS to print processes tree. */
        switch (ops) {
            case 0:
                /* use DFS to print processes tree. */
                printf("%s\n", procs[1].filename);
                printPsTree(procs, 1, 1, ops);
                break;
            case 2:
                printf("%s\n", procs[1].filename);
                /* sort */
                printPsTree(procs, 1, 1, ops);
                break;
            case 4:
                printf("pstree (PSmisc) UNKNOWN\nCopyright (C) 1993-2019 Werner Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
                break;
            case 6:
                printf("pstree (PSmisc) UNKNOWN\nCopyright (C) 1993-2019 Werner Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
                printf("%s\n", procs[1].filename);
                printPsTree(procs, 1, 1, ops);
                break;

            case 1:
                printf("%s(%d)\n", procs[1].filename, 1);
                printPsTree(procs, 1, 1, ops);
                break;
            case 3:
                printf("%s(%d)\n", procs[1].filename, 1);
                printPsTree(procs, 1, 1, ops);
                break;
            case 5:
                printf("pstree (PSmisc) UNKNOWN\nCopyright (C) 1993-2019 Werner Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
                printf("%s(%d)\n", procs[1].filename, 1);
                printPsTree(procs, 1, 1, ops);
                break;
            case 7:
                printf("pstree (PSmisc) UNKNOWN\nCopyright (C) 1993-2019 Werner Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
                printf("%s(%d)\n", procs[1].filename, 1);
                printPsTree(procs, 1, 1, ops);
                break;
            default:
                break;
        }
    } else {
        exit(1);
    }
    return 0;
}

void printPsTree(struct proc_node *procs, pid_t pid, int level, int ops) {
    /* MAXPID_T is too large. */
    for (int i = pid; i < MAXPID_T; i++) {
        if (procs[i].ppid == pid) {
            for (int j = 0; j < level; j++) {
                printf("\t");
            } 
            if (ops & 0x1) {
                printf("%s(%d)\n", procs[i].filename, i);
            } else {
                printf("%s\n", procs[i].filename);
            }
            printPsTree(procs, i, level + 1, ops);
        }
    }
    return;
}


int listProc(struct proc_node *procs) {
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc/");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // printf("%s\n", dir->d_name);
            if (isNum(dir->d_name)) {
                char filename[300];
                int pid, ppid;
                char exe[256];
                snprintf(filename, sizeof(filename), "/proc/%s/stat", dir->d_name);
                FILE *fp = fopen(filename, "r");
                if (fp) {
                    // 用fscanf函数读取
                    fscanf(fp, "%d %s %*c %d", &pid, exe, &ppid);
                    // create disjoint set
                    exe[0] = '{';
                    exe[strlen(exe)-1] = '}';
                    procs[pid].pid = pid;
                    procs[pid].ppid = ppid;
                    strcpy(procs[pid].filename, exe);
                    fclose(fp);
                } else {
                    return 1;
                }
            }
        }
        closedir(d);
    }
    return 0;
}

bool isNum(char *s) {
    if (s == NULL) {
        return false;
    }
    for (int i = 0; i < strlen(s); i++) {
        /* all pids are positive integer. */
        if (!(s[i] >= '0' && s[i] <= '9')) { 
                return false;
        }
    }
    return true;
}
