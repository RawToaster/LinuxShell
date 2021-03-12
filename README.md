# LinuxShell
### Description
A program that can take in and execute a linux command. It can be ran in an interactive mode to run one command at a time, or in batch mode to read a list of commands from a file and execute them all sequentially. The application also supports a few custom commands and features. Those being, directory change, command history, environment path, signal catching, pipelining, and file redirection. The program uses system calls such as fork() and execvp() to handle executing the commands on different processes. Application was developed in a Linux environment with a group of people.
### Execution
To compile the program, use command `gcc main.c processor.c -o [executable name]`. This will generate an executable of your desired name that can be used to run the program. 
When running, there are two different modes, batch and interactive. 
#### Batch Mode
To enter batch mode, add a file as a command line argument after the executable name. 
#### Interactive Mode
To run in interactive mode, simply run the executable and type commands as you go. 

### Use
The system can execute any Linux system command. Including commands that use file redirection and pipelining. Additionally, there are some custom commands implemented that allow the user to change directory, check their command history, and check, append, and remove directories to their environment path.
#### Batch Mode
Batch mode will take in a file as a command line argument and execute each command, line by line, in the file. The user can also place multiple commands on the same line as long as each command is seperated by a `;`. An example batch file can be seen in `batch.txt`.
#### Interactive Mode
Interactive mode is simply the command line. Type in a command, and see the result. 
#### Custom Commands
##### Change Directory
Use command `cd [directory name]` to enter a particular directory, or simply use `cd` to return to the home directory.
##### Check History
Command `myhistory` will show the user their last 20 commands in a numbered list. The user can use the listed number in command `myhistory -e [command number]` which will execute the command at the given location. Lastly, `myhistory -c` can be used to clear the history. 
##### Environment Path
The `path` command can be used to see the current environment path. To append a directory to the end of the path, use command `path + [directory name]` and it will be added to the end of the path after a `:`. To remove the first occurance of a directory, use command `path - [directory name]`.
