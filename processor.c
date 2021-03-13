/*
*
* Group 5
* Major 2
*
*/

#include "processor.h"

void processor(char* input, int *historySize, char* myhistory[20], int *nextHistory){

	
	struct sigaction act;										// signal struct
	
	char *temp = malloc(8 * sizeof(char));						// temp string	(stores " exit ")
	int   exitCheck = 0;										// bool variable (set to 1 in exit code)

	// checking for exit and removing it from input
	while((temp = strstr(input, "exit")))
	{
		memmove(temp, temp +4, strlen(temp + 4) +1);
		exitCheck = 1;		// setting bool to true for check @ end of prog
	}

	char **line = malloc(512 * sizeof(char*)); 					// array of strings to store each command line
	char *delim = ";\n";										// field seperator
	char *parsedInput = strtok(input, delim); 					// holds input parsed at semi-colons 
	int   i=0;													// index variable
	
	// parsing input by semi-colons
	while(parsedInput != NULL)
	{
		line[i] = parsedInput;                          		// putting parsed input into the cmd
		i++;

		parsedInput = strtok(NULL, delim);
	}
	line[i] = NULL;
	
	i = 0;
	while(line[i] != NULL){
		
		//tcsetpgrp(fileno(stdin), getpgrp());
		act.sa_handler = SIG_IGN;	

		for(int n = 1; n <= 64; n++)
		{
			// catching all catch-able signals
			if(n ==9)
				continue;
			else if(n == 19)
				continue;
			else if(n == 32)
				continue;
			else if(n == 33)
				continue;
		
			// setting how to handle recieved signals
			assert(sigaction(n, &act, NULL) == 0);	// ERROR CHECK
		}

		pid_t pid;												// process id variable
		int   status;											// integer used for status passed into waitpid()
		
		int j = 0;												// index variable

		char **cmd = malloc(512 * sizeof(char*));				// command array of strings allocated with 64 chars of space
		char  *space = " ";										// field seperater in input for parsing
		char  *parsedLine = strtok(line[i++], space);			// holds the parsed input line from the user

		// parsing lines by spaces
		while(parsedLine != NULL)
		{
			cmd[j] = parsedLine;								// putting parsed input into the cmd
			j++;

			parsedLine = strtok(NULL, space);
		}
		cmd[j] = NULL;

		// ignoring any commands set to NULL
		if(!cmd[0])
		{
			continue;
		}
		
		//new "cd" command with argument
		if(strcmp(cmd[0], "cd") == 0)
		{
			if(!cmd[1])
			{
				//system call just to check that chdir() worked, erase after testing.
				chdir("/home");
				system("pwd"); 
			}

			if(cmd[1])
			{
				chdir(cmd[1]);
				system("pwd");
			}	
			break;
		}

		// new "myhistory" user command
		if(strcmp(cmd[0], "myhistory") == 0){ //Check if myhistory is entered
			if(!cmd[1]){ //Check if no flags
				printf("Command History:\n");
				for(int k = 0; k < *historySize; k++){
					printf("[%d] %s\n", k+1, myhistory[k]);
				}

			}
			else if(strcmp(cmd[1], "-e") == 0){ //Check if -e flag is invoked
				if(!cmd[2]){
					printf("ERROR: Usage: myhistory -e <history_number>\n");
					break;
				}
				else{
					if((*cmd[2] - '0') > *historySize || (*cmd[2] - '0') <= 0){ //Check if <history_number> is valid
						printf("Error: History number not valid. numbers between 1 and %d currently\n", *historySize);
					}	
					else{ //Rerun chosen command
						int history = *cmd[2] - '0';
						char *temp;
						if((temp = malloc(512*sizeof(char))) == NULL){
							perror("malloc");
							exit(1);
						}
						strcpy(temp, myhistory[history-1]);
						processor(temp, historySize, myhistory, nextHistory);
						break;
					}
				}
			}
			else if(strcmp(cmd[1], "-c") == 0){ //Check if -c flag is invoked
				for(int k = 0; k < *historySize; k++){ //Free space in myhistory[]
					free(myhistory[k]);
				}
				*historySize = 0; //Reset history size
				*nextHistory = 0; //Reset next history location
			}


			break;
		}


		// new "path" user command
		if(strcmp(cmd[0], "path") == 0){

			// print pathname, if cmd[1] is NULL
			if(!cmd[1]){
				printf("%s\n", getenv("PATH"));
			}
			// remove pathname
			else if(strcmp(cmd[1], "-") == 0 && cmd[2]){
				char *tempPath = getenv("PATH");
				char *pos = strstr(tempPath, cmd[2]);
				memmove(pos-1, pos+strlen(cmd[2]), strlen(cmd[2])+1);
				setenv("PATH", tempPath, 1);
			}
			// append pathname in cmd[2]
			else if(strcmp(cmd[1], "+") == 0 && cmd[2]){
				char *tempPath = getenv("PATH");
				strcat(tempPath, ":");
				strcat(tempPath, cmd[2]);
				setenv("PATH", tempPath, 1);
			}
			else{
				perror("path");
			}
			break;
		}
		
		// executing process
		pid = fork();
		if(pid == 0)		// CHILD
		{
			
			// signal handling
			act.sa_handler = SIG_DFL;			// performing default action when signal recieved
			for(int n = 1; n <= 64; n++)
			{
				// catching all catch-able signals
				if(n ==9)
					continue;
				else if(n == 19)
					continue;
				else if(n == 32)
					continue;
				else if(n == 33)
					continue;
		
				// setting how to handle recieved signals
				assert(sigaction(n, &act, NULL) == 0);	// ERROR CHECK
			}	
		
			//JONATHANS REDIRECTION CODE
			//*********************************************************************************************************************//
			/*Checking for a '<' & '>' symbol and storing strings with commands/file names.*/
			int redirect = 0; //bool statement
			int piper = 0;	  //bool statement
			for(int p=0; p < j; p++)
			{
				if(strchr(cmd[p], '<')!=NULL)
				{
					int ifp;
					redirect = 1;
					char inputF[256];
					char *redirectionCMD [j];
					
					for(int k=0; k <= j; k++)
					{
						char *strResult = strchr(cmd[k], '<');

						if(strResult == NULL)
						{
							redirectionCMD[k]=cmd[k];
						}
						if(strResult != NULL) //found redirection signal.
						{
							redirectionCMD[k]='\0';
							strcpy(inputF, cmd[++k]);       //input file is 1 space after redirection signal.
							redirectionCMD[k]='\0';
							break;

						}

					}
					ifp = open(inputF, O_RDONLY);	
					dup2(ifp,fileno(stdin));
					close(ifp);
					execvp(redirectionCMD[0], redirectionCMD);
					dup2(0,fileno(stdin));
					printf("%s: command not found\n", redirectionCMD[0]);
					break;
				}
				if(strchr(cmd[p], '>')!=NULL)
				{
					redirect = 1;
					int ofp;

					char inputF[256];
					char *redirectionCMD [j];
					int k = 0;
					for(k=0; k < j; k++)
					{
						char *strResult = strchr(cmd[k], '>');

						if(strResult == NULL)
						{
							redirectionCMD[k]=cmd[k];
						}
						if(strResult != NULL) //found redirection signal.
						{
							redirectionCMD[k]='\0';
							strcpy(inputF, cmd[++k]);       //input file is 1 space after redirection signal.
							redirectionCMD[k]='\0';
							break;
						}
					}
					ofp = open(inputF, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					dup2(ofp,fileno(stdout));
					close(ofp);
					execvp(redirectionCMD[0], redirectionCMD);
					dup2(1, fileno(stdout));
					printf("%s: command not found\n", redirectionCMD[0]);
					break;
				}

				//JONATHANS REDIRECTION CODE ENDS
				//*********************************************************************************************************************//
				//JEREMY'S PIPELINING CODE BEGINS
				
				if(strchr(cmd[p], '|') !=NULL){ //Check if pipe is used
					piper = 1;				//bool if pipelining is used
					int twopipes = 0;		//bool if 2 pipes are found
					char *pipe1CMD[j];		//command to the left of first pipe
					char *pipe1outCMD[j];	//command to right of first pipe/left of second pipe
					char *pipe2outCMD[j];	//command to right of second pipe
					int firsttime = 1;		//bool if first pipe is found
					
					//for(int k=0; k < j; k++)
					int k = 0;			//index for pipe1CMD
					int out1ind = 0;	//"      " pipe1outCMD
					int out2ind = 0;	//"      " pipe2outCMD
					while(cmd[k] != NULL)
					{
						char *strResult = strchr(cmd[k], '|'); //Check if '|' is found

						if((strResult == NULL) && (firsttime == 1)) //If not a '|' and none have been found
						{
							
							pipe1CMD[k]=cmd[k];
						}
						else if((strResult != NULL) && (firsttime == 1)) //found 1st pipe signal.
						{
							firsttime = 0;
							pipe1CMD[k]='\0';
							
						}
						else if((strResult == NULL) && (firsttime == 0) && (twopipes == 0)) //If not a '|' and 1 has been found
						{
							pipe1outCMD[out1ind]=cmd[k];
							
							out1ind++;
						}
						else if((strResult != NULL) && (firsttime == 0))	//second pipe is found
						{
							twopipes = 1;
						}
						else if(((strResult == NULL) && (firsttime == 0)) && (twopipes == 1)) //If not a '|' and 2 have been found
						{
							pipe2outCMD[out2ind]=cmd[k];
							out2ind++;
						}
						k++;
					}
					pipe1outCMD[out1ind] = '\0';
					
					if(twopipes == 0){ //If 1 pipe will be used
						int fd1[2];		//file descriptors for pipe
						pipe(fd1);//make pipe
						pid_t pid;
						pid = fork();
						if(pid == 0){ //Fork the first child to execute the first command
							dup2(fd1[1], fileno(stdout)); //Duplicate write end of pipe to stdout
							
							close(fd1[1]);	//close both ends of pipe
							close(fd1[0]);
							
							execvp(pipe1CMD[0], pipe1CMD);
						}
						else if(pid == -1){ //Error checking
							perror("FORK");
							exit(EXIT_FAILURE);
						}
						else //parent)
						{
							pid = fork();
							if(pid == 0){ //Fork the second child to execute the second command
								dup2(fd1[0], fileno(stdin)); //duplicate the read end of pipe to stdin
								
								close(fd1[0]); //Close both ends of pipe
								close(fd1[1]);
								
								execvp(pipe1outCMD[0], pipe1outCMD);
							}
							else if(pid == -1){ //Error
								perror("FORK");
								exit(EXIT_FAILURE);
							}
						}
						
						close(fd1[0]); //Close both ends of pipe
						close(fd1[1]);
						
						//Parent waits for both children to finish
						for(int waiter = 0; waiter < 2; waiter++){
							wait(&status);
						}
					}
					else if(twopipes == 1){ //If two pipes are used
						int fd1[2], fd2[2]; //file descriptors for pipes
						pipe(fd1);		//make pipes
						pipe(fd2);
						pid_t pid;
						pid = fork();
						if(pid == 0){  //Fork the first child to execute the first command
							dup2(fd1[1], fileno(stdout)); //Duplicate write end of first pipe to stdout
							
							
							//Close both ends of pipes
							close(fd1[1]);
							close(fd1[0]);
							close(fd2[1]);
							close(fd2[0]);
							
							execvp(pipe1CMD[0], pipe1CMD);
						}
						else if(pid == -1){ //Error
								perror("FORK");
								exit(EXIT_FAILURE);
						}
						else //Parent
						{
							pid = fork();
							if(pid == 0){ //Fork the second child to execute the second command
								dup2(fd1[0], fileno(stdin));  //duplicate the read end of first pipe to stdin
								
								dup2(fd2[1], fileno(stdout));  //duplicate the write end of second pipe to stdout
								
								
								//Close both ends of pipes
								close(fd1[0]);
								close(fd1[1]);
								close(fd2[0]);
								close(fd2[1]);
								
								execvp(pipe1outCMD[0], pipe1outCMD);
							}
							else if(pid == -1){ //Error
								perror("FORK");
								exit(EXIT_FAILURE);
							}
							else{
								dup2(fd2[0], fileno(stdin));  //duplicate the read end of second pipe to stdin 
								
								
								//Close both ends of pipes
								close(fd1[0]);
								close(fd1[1]);
								close(fd2[0]);
								close(fd2[1]);
								
								execvp(pipe2outCMD[0], pipe2outCMD);
							}
							
							//Close both ends of pipes
							close(fd1[0]);
							close(fd1[1]);
							close(fd2[0]);
							close(fd2[1]);
							
							//Parent waits for all three children to finish
							for(int waiter = 0; waiter < 3; waiter++){
								wait(&status);
							}
						}
					}
				}
			}
			if(redirect==1)
			{
				//printf("%d\n", redirect);
				redirect = 0;
				abort();
				break;
			}
			if(piper==1) //If pipes were used
			{
				piper = 0;
				abort(); //Kill outer child
				break;
			}
			//JEREMY'S PIPELINING CODE ENDS
			//*********************************************************************************************************************//
			
			// executing the given command and checking for failure
			if(execvp(cmd[0], cmd) < 0)
			{
				printf("%s command not found\n", cmd[0]);
				exit(1);		
			}
		}
		else if(pid == -1)	// ERROR
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else				// PARENT
		{
			signal(SIGTTOU, SIG_IGN);
			// waiting for the child to finish
			waitpid(pid, &status, WUNTRACED);
		}

		// freeing up memory
		free(cmd);	
	}

	// exits if the line has an exit in it
	if(exitCheck == 1)
	{
		free(temp);
		exit(0);
	}
}
