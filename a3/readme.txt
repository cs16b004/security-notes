CRACKME 1

For this file I initally used strings and readelf to get information about 
the sections and differetn offset.

I tried running it and saw what input was required.


After giving some random input,I tried running it with 
strace and ltrace to see if any strncmp, atoi calls were made 
they didnt show up in the output.

Next I used gdb to locate the various strings used 
and their offset.

0x402040:       "Your number corresponds to %s, well done!\n" // Success Message

0x402015:       "Enter the right number: " //Input prompt
0x403015:
0x40202e:       "%32s" // Scanf specifier
0x402008:       "Wrong number"  // fail message.


I tried to find these strings in the code but wasn't able to. 

This binary has a lot of junk code and it became harder to navigate.

Then I switched to Ghidra, and decomplied,
I found the main function called by the __lib_c_main function,
copied it to crack1.c and made some changes /*Added libraries*/, assigned better types to variables and tried running it.

The while at first seemed confusing but slowly(once I tried running it again again )
I was able to break the encoding. Essentially the number is traversed with 3 digits at a time,
there value is recorded in decimal, converted to corresponding char value, and then an offset of 4 
is added, since there could be many such numbers I have written a c program sol1.c which puts out
 the valid number which correspond to 

905ESCKCAH | HACKCSE509 :D

NUmber : 053044049065079063071063061068

Also the trick used to take three numbers by using adjecent numbers was very clever :). 

I think atoi is very unsafe in that regard.

I have added comments in the crack1.c files to explain what section does in detail.

 ----------------------------------------------------------------------------------------------------
CRACKME2 
Here I used similar steps which I used, except the fact that I didn't use GDB but Ghidra directly.

Essentially I read all the string s used by using the strings command.

Apparently there were some encrypted/encoded strings presents.

Next thing I used ltrace to see any use of string.h functions

like strcmp, strlen, strcpy

I was able to able to catch a few strlen and strcpy code,

and a lot of calls to rand.

Then I loaded the code to GHidra and looked at the function referenced by
__lib_start_main() function.

Initial inspection showed that it was decoding the strings in the rodata section and comparing it with the input.

That was it.

I copied the strings, the functions to decode them and the main function in crack2.c compiled it and ran it to see the decrypted the string.

I have provided the comments in the crac2.c file to explain what each function is doing essentially.



The string is x509_syssec_2023 
