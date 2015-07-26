/**
 * COMP3520 - Operating Systems - Assignment 1 
 * 
 * author: Internet's own boy
 */

//Headers

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "myshell.h"

//Constants

#define MAX_BUFFER 1024	         // max line buffer
#define MAX_ARGS 64	             // max number of arguments (args)
#define SEPARATORS " \n\f\t\v"	 // tokens sparators
#define MAX_FILENAME 256         // max file name length

//Global Variables

extern char **environ;        // environment variables
char buffer[MAX_BUFFER];      // input buffer
char * args[MAX_ARGS];        // pointers to arg strings
char ** arg;                  // working pointer thru args
int argn;                     // number of args

char initialPath[MAX_BUFFER];   // start path
char shellPath[MAX_BUFFER];     // shell path
char currentPath[MAX_BUFFER];   // current path

//to be used for append(0), write(1) or read(2) operations 
char files[3][MAX_FILENAME]; 

// to chose the typ eof i/o redirect
char redirectType[3]; 

// redirection symboles
char *redirectSymbols[3] = { ">>", ">", "<" }; 

int main (int argc, char ** argv) 
{
  // ignore the CTRL+C (SIGINT), otherwise user can quit myshell using that
  signal(SIGINT, SIG_IGN); 

  // get the initial working path
  getcwd(initialPath, MAX_BUFFER-1); 
  strcpy(shellPath, initialPath);
  strcat(shellPath, "/myshell"); 

  // set the system "SHELL" environment variable to be our program
  setenv("SHELL", shellPath, 1); 
  
  // check for batch file, if it has been given as the run time argument
  if(argc > 2) 
  { 
    printf("Too many arguments! - One batch file only\n");
    exit(1);
  } 
  else if(argc == 2) 
  {
    //read the batch file contents and execute them sequentially
    readBatchFile(argv[1]); 
  }
  
  //get the user input from shell prompt
  while (!feof(stdin)) 
  {     
    //changePath();
    //get the current path, write it as our prompt
    getcwd(currentPath, MAX_BUFFER-1);
    printf("%s$ ",currentPath); 

    //read user's input
    if (fgets (buffer, MAX_BUFFER, stdin )) 
    { 
      // tokenize the user's input
      tokenizeCommand(); 

      /* all commands will be run an foreground, unless 
         user wants it to be executed in the background 
         using & symbol at the end of the current command 
       */
      int run_in_background = 0; 

      //check for & symbol at end of a command
      if(argn > 1 && !strcmp(args[argn - 1], "&")) 
      { 
      	//remove the & symbol and set the flag for background execution
        args[argn-1] = (char*)0; 
      	run_in_background = 1;
      }
      
      //check for background exection flag
      if(run_in_background) 
      { 
      	// create a child process and excute the program
      	switch(fork()) 
      	{
      	  case -1:
      	    printf("fork error\n");
            exit(1);
            break;
      	  case 0: 
            //save the parent as environment variable
      	    setenv("PARENT", shellPath, 1);

            //excute the command
      	    runCommand();
      	    exit(0); 
      	    break;
      	}
      }//end of if 

      //otherwise execute the process in foreground
      else 
      {       	
      	runCommand();
      }
    }
  }
  return 0;
}//end of main function

/* Tokenize the input from user */
void tokenizeCommand() 
{
  
  char *tmp, *point;
  int i;
  for(i=0; i<3; i++) 
  { 
    // append(>>) : 0, write(>): 1, read(<): 2
    redirectType[i] = 0;
    
    if((point = strstr(buffer,redirectSymbols[i])) != NULL) 
    { 
      // get the redirect symbol
      tmp = strtok(point+strlen(redirectSymbols[i]), SEPARATORS); 
      
      // get the file name
      strcpy(files[i],tmp); 
      
      // set input or output file name
      redirectType[i] = 1; 
      
      *point = '\0';
      if((tmp=strtok(NULL,SEPARATORS)) != NULL)
	       strcat(buffer,tmp);       
    }
  }//end of for loop
  
  /* Tokenize the input into args array */
  argn = 1;
  arg = args;
  
  // tokenize the input
  *arg++ = strtok(buffer,SEPARATORS); 
  
  //loop through the input and separate the tokens.
  while ((*arg++ = strtok(NULL,SEPARATORS))) 
  {
    argn++;
  }
}//end of function

/* restore the exit signal(CTRL+C) in the child process */
void signalChild() 
{
  signal(SIGINT, SIG_IGN);
  putchar('\n');
}

/* make file ready for I/O operations
   i.e. append(>>), or write(>), or read(<) */
void fileRedirections(int readPermission) 
{
  // open file to append new content
  if(redirectType[0] == 1) 
  {
    freopen(files[0], "a", stdout); 
  }

  // open file to write new content
  if(redirectType[1] == 1) 
  {
    freopen(files[1], "w", stdout); 
  }

  // open file for read its content
  if(redirectType[2] == 1 && readPermission == 1) 
  {
    freopen(files[2], "r", stdin); 
  }
}//end of function

/* restore output redirection, if > or >> has been set to write to file 
   ,it will be set back to standard output i.e. screen  */
void resetIORedirection() 
{
  // check for append or write 
  if(redirectType[0] || redirectType[1]) 
  {
    // set the standard output as the system screen
    freopen("/dev/tty","w",stdout);
  } 
}

/* execute command */
void runCommand() 
{
  //check for command
  if (args[0]) 
  { 
    
    // check for built-in commands

    // clr - Clear the screen 
    if (!strcmp(args[0],"clr")) 
    {
      system("clear");
    }
    
    // dir -  List directory contents
    else if(!strcmp(args[0],"dir")) 
    {
      // handle file redirection 
      fileRedirections(0); 
      
      char tmp[MAX_BUFFER];
      strcpy(tmp, "ls -la ");
      if(args[1])
      {
	       strcat(tmp, args[1]);
      }

      //excute the command
      system(tmp); 

      // restore i/o redirection
      resetIORedirection(); 
    }
    
    // environ - print environment variables
    else if(!strcmp(args[0],"environ")) 
    {
      // handle i/o redirection 
      fileRedirections(0); 
      char **env = environ;
      
      //loop through environment variables and print them out
      while( *env ) 
      {
      	printf( "%s\n", *env );
      	env++;
      }

      // restore i/o redirection
      resetIORedirection(); 
    }
    
    // cd - change current directory 
    else if(!strcmp(args[0], "cd")) 
    {
      // check for given directory
      if(!args[1]) 
      { 
	       //no argument given, just print out the current working directory
         printf( "%s\n", currentPath ); 
      } 
      
      //otherwise, change the working directory
      else 
      {
      	//change the directory and check for outcome
      	if(!chdir(args[1])) 
      	{ 
      	  //get the current working directory
          getcwd(currentPath, MAX_BUFFER-1); 

      	  // set the environment variable accordingly 
      	  setenv("PWD", currentPath, 1); 
      	} 

        //if directory doesn't exist
      	else 
      	{      	  
      	  printf("%s: No such file or directory\n", args[1]);
      	}
      }
    }
   
    // echo - print out given string to standard output 
    else if(!strcmp(args[0], "echo")) 
    {
      // handle i/o redirection 
      fileRedirections(0); 

      //to store the user's input
      char *comment = (char *)malloc(MAX_BUFFER);
      strcpy(comment, "");
      arg = &args[1];
      
      //loop through the string to eliminate the unnecessary spaces
      while(*arg) 
      { 
      	strcat(comment, *arg++);
      	strcat(comment, " "); 
      }
      printf("%s\n", comment); 
      
      // clear buffer and free the assigned memory
      memset(comment, 0, MAX_BUFFER); 
      free(comment);
      
      // restore i/o redirection
      resetIORedirection(); 
    }
    
    // help - print out the user manual
    else if(!strcmp(args[0], "help")) 
    {
      // handle i/o redirection 
      fileRedirections(1); 

      //prepare the command to be run by system
      char tmp[MAX_BUFFER];
      strcpy(tmp, "more ");
      strcat(tmp, initialPath);
      
      // readme file needs to be in same folder as myshell
      strcat(tmp, "/readme"); 
      system(tmp);
      putchar('\n');
      
      // restore i/o redirection
      resetIORedirection(); 
    }
    
    // pause - freeze the shell until user presses "Enter" key */
    else if(!strcmp(args[0], "pause")) 
    {
      // use getpass function to disable screen output. 
      // getpass is for getting password but here it suits our purpose
      getpass("Press Enter to continue\n"); 
    }
    
    // quit - exit the shell */
    else if (!strcmp(args[0],"quit"))
    {
      exit(0);
    }
    
    //handlle external commands - using child process
    //external commands are the ones outside the built-in commands
    else 
    {       
      int status;
      pid_t pid;

      // quit child process first, if any 
      signal(SIGINT, signalChild); 
      
      //create the child and excute it
      switch(pid = fork()) 
      {
      	//child creation failed
        case -1:
      	  printf ("fork error");
          exit(1);
      	  break;
	  
      	// child created successfully 
      	case 0: 
          //set the environment variable for parent process
      	  setenv("PARENT", shellPath, 1);
      	  
      	  // handle i/o redirection
      	  fileRedirections(1); 

      	  // execute the command (child)
      	  if(execvp(args[0],args) == -1) 
          {
      	    printf("command not found\n");
          }
          exit(1);
      	  break;
      }//end of switch

      //flush out the output buffer
      fflush(stdout);

      // wait for child process
      waitpid(pid, &status, 0); 
    }

  }//end of outer if
  
}//end of function

/* Read batch file and execute its contentc */
void readBatchFile(char fileName[MAX_FILENAME]) 
{
  FILE *fp;
  int lineNumber = 1;

  //open the file for read operation
  fp = fopen(fileName, "r"); 

  //check for error
  if(fp == NULL)
  {
    printf("Batch file does not exist\n");
    exit(1);
  }

  //loop through the file and read its contents
  while (fgets (buffer, MAX_BUFFER, fp )) 
  {     
    // print the command number
    printf("%d. %s", lineNumber++, buffer); 

    // tokenize each line
    tokenizeCommand(); 

    // execute each command
    runCommand(); 

    //add new line character after each command
    putchar('\n');
  }

  fclose(fp);
  exit(0); 
} 