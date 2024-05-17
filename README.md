### Assignment 2 for CS410 
## Group: Qing Liu and Irene Mitsiades 

The shell can support IO redirection to and from files and pipes. You can also set a command to run in the background by using & like this: "(name of command) &"
Inputs to the shell can be either space or tab seperated. 

"make" on the command line will make all of the object files and executables needed. Then "./myshell" will start up the shell.

For the matrix executables, matmult_t can only support a matrix multiplication if the return matrix has less than 1000 entries (so a multiplication ending in a 30 x 30 matrix is ok since it has 900 entries).

matmult_p uses an array so there isn't a size limitation there.

External references: \
isatty reference - https://man7.org/linux/man-pages/man3/isatty.3.html \
dup2 reference - https://man7.org/linux/man-pages/man2/dup.2.html \
strtok reference - https://www.cplusplus.com/reference/cstring/strtok/ \
feof reference - https://man7.org/linux/man-pages/man3/feof.3.html \
strcpy reference - https://man7.org/linux/man-pages/man3/strcpy.3.html \
fgets reference - https://man7.org/linux/man-pages/man3/fgets.3.html \
strcmp reference - https://www.cplusplus.com/reference/cstring/strcmp/ \
execvpe reference - https://linux.die.net/man/3/execvpe \
itoa reference - https://www.geeksforgeeks.org/implement-itoa/ \
shmat/shmctl/shmget reference - https://man7.org/linux/man-pages//man2/shmat.2.html \
swapcontext reference - https://linux.die.net/man/3/swapcontext \
makecontext reference - https://www.man7.org/linux/man-pages/man3/makecontext.3.html \
additional reference for the makecontext and swapcontext functions - https://www.gnu.org/software/libc/manual/html_node/System-V-contexts.html \

And thank you to Professor Rich and Sasan Golchin for helping!
