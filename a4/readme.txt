

0. The code in main.c file works as a small conatainer for simple programs.

run `make` to build the binary,
you might be prompted to enter password  
as the binary requires CPA_SYS_ADMIN for clone and namepsace operations.

To create a container,
***************************** I have skipped -b option ********************************************************
1. Run ./os_isolate `binary` file

2. A new process namespace is created by passing the CLONE_NEWPID flag to clone() system call.

This makes the child process cutoff form rest of the process world.

If you do `ps` in the new bash running as a child you can see all the process listed.
This is because ps looks in to the /proc directory for process information.


But since child process is cutoff from rest of them (as they lie in different namespace)
trying to `kill <pid>` will result `Process <pid> not found`

3. `ip link` command gives the list of network devices on PC, 
	afer the clone () a new process namespace is created but 
	the PC resources / devices are still shared. 
	
	As a result the child process still have a complete view of the system.
	
	By adding the CLONE_NEWNET flag to clone() call a new netqork namespace is created 
	this restricts the vire of network devices as seen by the process.
	
	
4. Even with PID namespace and NEt namespace, the child process can still see and travrse all filesystem in the system and can potentially modify mounts used by ther process.
	A mount namespace helps in islating the mount points seen by the process.
	To achieve complete isolation, I have used the pivot_root() Sys call to chaneg the root mount of the process.
	Thus the process is isolated from looking at directories beyod the `container_rrotfs` directory in the folder.
	I initially I used the alpine's rootfs as a container root fs but swithced to ubuntu as it prvides more utilities.
	
	5. Even with these isolation, a process can escape by 
		1. making itself root (as no usernamespace isolation is done) and then pivoting back to oldroot 	fs.
		
		seteuid(0);
		setcap(CAP_SYS_ADMIN);
		pivot_root(oldrootfs,newrootfs); //reversing the argument to get the system view
		
		// subsequent setns() calls to skip namespace isolation.
				
		
		2. Thats why I unmount the oldrootfs before spinning the binary, and have blocked some syscalls 
		like setcap, seteuid, Using Seccmp ebpf filters.
		
		
		
		3. Although usernamespace isolation is also a good solution but so that the root in this namespace 
		
		
		is always non root in the actual system. 
		

A sample image is also shared from my program.
