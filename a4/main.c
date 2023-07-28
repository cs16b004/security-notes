#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ftw.h>
#include <sys/syscall.h>

#include <limits.h>
#include <sys/mman.h>

#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/prctl.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_INFO print_green

#define LOG_ERROR print_red

void print_green(const char *fmt, ...) {

    char str[1024] ;
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
        va_end(args);

        printf("%s[%s] %s%s\n", ANSI_COLOR_YELLOW,__DATE__,str,ANSI_COLOR_RESET);

}

void print_red(const char *fmt, ...) {
    char str[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
        va_end(args);

        printf("%s[%s] %s%s\n", ANSI_COLOR_RED,__DATE__,str,ANSI_COLOR_RESET);

	exit(1);
}


// add flags here
#define CLONE_FLAGS (SIGCHLD|CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWNET)
#define UNSHARE_FLAGS (CLONE_NEWNS)






int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}


static int add_filter(){
	
	
	struct sock_filter filter[] = {
    		// Load architecture 
    		BPF_STMT(BPF_LD|BPF_W|BPF_ABS, offsetof(struct seccomp_data, arch)),
    		// Jump to syscall filtering 
    		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, AUDIT_ARCH_I386, 1, 0),
    		BPF_STMT(BPF_RET|BPF_K, SECCOMP_RET_ALLOW),

    		//Load syscall number 
    		BPF_STMT(BPF_LD|BPF_W|BPF_ABS, offsetof(struct seccomp_data, nr)),

    		// Block only seteuid, pivot_root,setcap
    		//
    		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 113, 0, 1),
    		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, __NR_capset, 0, 1),
    		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, __NR_pivot_root, 0, 1),
    		BPF_STMT(BPF_RET|BPF_K, SECCOMP_RET_KILL),

    		// Allow all other syscalls 
		BPF_STMT(BPF_RET|BPF_K, SECCOMP_RET_ALLOW),
	};

	struct sock_fprog prog = {
    		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
    		.filter = filter,
	};


	
	LOG_INFO("Adding SECCOMP filter ");
    	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) != 0) {
        	perror("prctl");
        	return 1;
    	}

	return 0;
	
}




static int pivot_root(const char *new_root, const char *put_old){
           return syscall(SYS_pivot_root, new_root, put_old);
}
int change_fs(){

	   char *new_root = "./container_rootfs";
           const char *put_old = "oldrootfs";
           char path[PATH_MAX];

           /* Ensure that 'new_root' and its parent mount don't have
              shared propagation (which would cause pivot_root() to
              return an error), and prevent propagation of mount
              events to the initial mount namespace. */

           if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
               LOG_ERROR("mount-MS_PRIVATE");

           /* Ensure that 'new_root' is a mount point. */

           if (mount(new_root, new_root, NULL, MS_BIND, NULL) == -1)
               LOG_ERROR("mount-MS_BIND failed");
	   
           /* Create directory to which old root will be pivoted. */

           snprintf(path, sizeof(path), "%s/%s", new_root, put_old);
           LOG_INFO("Creating path for old FS at %s",path);
           if (mkdir(path, 0777) == -1)
               LOG_ERROR("mkdir failed");

           /* And pivot the root filesystem. */

           if (pivot_root(new_root, path) == -1)
               LOG_ERROR("pivot_root");

           /* Switch the current working directory to "/". */

           if (chdir("/") == -1)
               LOG_ERROR("chdir");

           /* Unmount old root and remove mount point. 
           This adds another layer of security as the old rootfs has all the information on processes' 
           ids their file descriptors etc.
           
           If privelege escalation happens, then the process can make a call to pivot root back 
           to the host rootfs
           
           */

           if (umount2(put_old, MNT_DETACH) == -1)
               perror("umount2");
           if (rmdir(put_old) == -1)
               perror("rmdir");
 		// Mount proc for ps to work
 	   if(mount("proc","/proc","proc",0,"") !=0){
 	   	LOG_ERROR("Mounting Proc FS failed !");
 	   	
 	   }          
           return 0;

}
static char child_stack[1048576];

static int create_container(char *arg) {
		
  	
	LOG_INFO("Container process created pid: %d",getpid());
	
	
	// Pivot root chroot like operation
	change_fs();
	// Add seccomp-bpf filter
	add_filter();
	
	char *cmd[] = {arg, NULL};
	LOG_INFO("Executing command %s PID: %ld\n",cmd[0],getpid());
	
	seteuid(0);
	
	execv(arg, cmd);
	
	perror("exec");
	exit(EXIT_FAILURE);
}

static int child(void *args) {
	create_container((char*)args);
	return (0);
}
int copy_libs_and_bin(char * contained_process){
	
	char* cp = "/bin/cp";
	char* arg_bin[] = {cp,contained_process, "./container_rootfs/",NULL};
	char  buf[256];
	char * str = buf; 
	sprintf(str,"%s %s %s",cp,contained_process,"./container_rootfs/");
	
	FILE* p;
	
	LOG_INFO("Copying the binary in the container");
	p=popen(str,"r");
		
	
	return 0;
}
int main(int argc, char **argv) {
  char *cmd = "/bin/bash";
  if ( argc == 2 )
	cmd = argv[1];
 
  
  copy_libs_and_bin(cmd);
  
  
  
  LOG_INFO("Clone a new process from: %ld\n", getpid());

  pid_t child_pid = clone(child, child_stack+1048576, CLONE_FLAGS, cmd);
  
  LOG_INFO("clone() pid = %ld\n", (long)child_pid);
  if (child_pid > 0 )
  	LOG_INFO("Cloning successfull at : %ld\n", getpid());
  else{ 
  	LOG_ERROR("Child process creation failed");
  }
  waitpid(child_pid, NULL, 0);
  rmrf("./container_rootfs/oldrootfs");
  LOG_INFO("Removed old root fs directory");
  LOG_ERROR("Exiting");
  
  return 0;
}

