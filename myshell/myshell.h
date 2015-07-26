/**
 * COMP3520 - Operating Systems - Assignment 1 
 * 
 * author: Internet's own boy
 */

/* ========== FUNCTION PROTOTYPES ========== */

/**
 * this function is responsible for printing error messages.
 * it will be called whenever a error occurs, prints out the error message and
 * exit the program 
 */
//void handleError(char*); 

/* this function will be called if we need to change 
 * the currently working to a new one 
 */
void changePath(); 

/* this function will be used after getting the user's input to tokenize them
 * and make them ready to be interpreted by other functions.
 * it uses the logic from external_cmds_sol.c from the lab exercises 
 * 
 */
void tokenizeCommand(); 

/* this function handles the signal sent from a child process. 
 * it basically initiats the ctrl+c signal
 */
void signalChild(); 

/* this function controls the I/O redirections.
 * it can interpret 3 types of redirections: 
 * >  for writing 
 * >> for appending 
 * <  for reading
 */
void ioRedirect(int); 

/* it sets back the changed i/o redirection to its default value
 */
void ioRedirectClose(); 

/* this function interprets and executes the commands entered by user
 * only the commands that have been mentioned in the assignment description.
 * if any commands outside of those entered it will try to create a child process and
 * execute it
 */
void runCommand(); 

/* this function is responsible to handle the batch file 
 * it reads the file, tokenizes it and executes them
 */
void readBatchFile(); 