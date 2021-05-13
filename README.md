# CrackMe_MyDog5_Walkthrough

Link to the crackme: https://crackmes.one/crackme/6043d8b833c5d42c3d016d32

### Solution description

Trampoline hooking a string comparison function to extract the Dog's name hashed using MD5, then decrypting it to obtain the Dog's name in plaintext.

### Thought process

Running the program will tell us that our input is being compared to something, and will produce one of two outputs. Either we get the name right, or we don't. So, it is 
immediately obvious that our goal would be try to extract the string being compared to. Using IDA's decompiler, we can see that our input (Str1) is being hashed using a 
str2md5 hashing function, which suggets that the other string is probably in the same format (as evident by qmemcpy and the subsequent for loop). Just below that, we can see a 
simple strcmp (string compare) function with Str1 and Str2 as its arguments. So, if we can somehow intercept those two arguments being passed in, we can extract the hashed values
and attempt to decrypt it. 

Now normally the program will either provide the decryption function (in which case you just have to copy it and run it), or we have to reverse the function. In this case, we don't
have a decryption function and reversing a MD5 is way too hard. Luckily, there are plenty MD5 decryptors online. So after we extracted the two hashed values, we easily obtained
the plaintext value and thus got our answer. 

### Other possible solutions

You could attempt to directly read the value Str2 from memory (I couldn't). 

You could also patch the je (jump if equal) instructions to force the program to display the "winning" message, but where is the fun in that?

C++ Source code attached. It is a DLL that will hook the strcmp function upon being injected into the target process. 
