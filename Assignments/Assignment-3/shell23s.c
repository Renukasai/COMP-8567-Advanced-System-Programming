Sai Renuka Telu
110096549
Section 1

//fundamental libraries needed
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pwd.h>
#include <stdbool.h>

//line is a preprocessor directive in C that defines a maximum arguements with a value of 5.
#define MAX_ARGS_Re 5   

//line is a preprocessor directive in C that defines a maximum command len with a value of 7.  
#define MAX_COMMAND_LEN_Re 7    

//imposes a 256-character limit on the input command string.   
#define MAX_INPUT_SIZE_Re 256      

//line declares an array of pointers to characters named j_Re which stores command arguments
char *j_Re[MAX_ARGS_Re + 1];  

//line declares an integer which tracks number of command arguements
int f_Re;                          

//The runCommand_Ren function is used to execute a command with the specified input file descriptor, output file descriptor, background flag, and a pointer to the status variable.
void runCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re);

//The runSingleCommand_Ren function is a convenience function that serves as a wrapper for the runCommand_Ren function. It simplifies the execution of a single command by directly calling runCommand_Ren with the provided arguments.
void runSingleCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

//The runSequentialCommand_Re function is another convenience function that acts as a wrapper for the runCommand_Re function. It is used to execute commands sequentially(;)
void runSequentialCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

// The runPipedCommand_Re function is a wrapper for the runCommand_Re function, intended for executing piped commands(|).
void runPipedCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

//The runOutputRedirectionCommand_Re function is a wrapper for the runCommand_Re function,intended for executing commands with output redirection (>).
void runOutputRedirectionCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

//The runAppendRedirectionCommand_Re function is a wrapper for the runCommand_Re function, specifically used for executing commands with append redirection(>>).
void runAppendRedirectionCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

//The runInputRedirectionCommand_Re function is a wrapper for the runCommand_Re function,specifically used for executing commands with input redirection(<).
void runInputRedirectionCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    runCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, stat_Re);
}

// The runCdCommand_Re function is a convenience function to handle the cd command separately.
void runCdCommand_Re() {
    //It performs a check to ensure that the second argument is not null.
    if (j_Re[1] != NULL) {
        // Change directory using chdir() within the parent process 
        if (chdir(j_Re[1]) != 0) {
            //If chdir() returns a non-zero value, it indicates an error
            perror("There was an error encountered while trying to change directory");
        }
    } else {
        //If the second argument is not provided, it prints an error message
        fprintf(stderr, "cd: missing argument\n");
    }
}

// This function handle the execution of individual commands within the shell program.It handles the setup of input/output redirection, execution of the command, and synchronization with the child process. The function also provides error handling.
void runCommand_Re(int input_file_d_Re, int output_file_d_Re, int bg_Re, int *stat_Re) {
    //A child process is generated using the fork() system call.
    pid_t pid_Re = fork();
    // checks if child process is 0 or not; if it is 0, condition is satisfied
    if (pid_Re == 0) {
        //It performs a verification to check if input redirection is specified.
        if (input_file_d_Re != STDIN_FILENO) {
           //It creates a duplicate of the file descriptor and assigns it to the standard input (stdin).
            dup2(input_file_d_Re, STDIN_FILENO);
            // The original file descriptor is closed to ensure that the command's input or output is redirected correctly.
            close(input_file_d_Re);
        }
        // output-redirection is specified or not will be checked
        if (output_file_d_Re != STDOUT_FILENO) {
            //It creates a duplicate of the file descriptor and assigns it to the standard output (stdout)
            dup2(output_file_d_Re, STDOUT_FILENO);
            // The initial file descriptor is closed
            close(output_file_d_Re);
        }
       //If the command is specified to be executed as a background process
        if (bg_Re) {
            // it creates a new session for the child process
            setsid();
        }
        // Handle the directory change command separately
        if (strcmp(j_Re[0], "cd") == 0) {
        //If the command is "cd", call and execute the runCdCommand_Re() function
            runCdCommand_Re();
            // Stop the child process with a status indicating success
            exit(EXIT_SUCCESS);
        }
        // Arguments in the j_Re array are executed using the command execvp. execvp() function is called to execute the command specified by the j_Re array. If execvp() succeeds, the child process starts executing the new command.
        execvp(j_Re[0], j_Re);
        // In case of a failure in the execvp command, an error message will be printed.
        perror("There was an error encountered while trying to execute the command.");
        // Stop the child process with a status indicating success
        exit(EXIT_FAILURE);
        // An error was encountered while performing the fork operation.
    } else if (pid_Re < 0) {
        //In the event of an error, a message is displayed.
        perror("An error occurred while attempting to fork a process.");
    } else {
        //If the command is not specified as a background process
        if (!bg_Re) {
            // Wait for the child process to complete and store its exit status
            waitpid(pid_Re, stat_Re, 0);
        }
        // If the command is cd, update the working directory within the parent process 
        if (strcmp(j_Re[0], "cd") == 0) {
            runCdCommand_Re();
        }
    }
}

// This function is responsible for expanding the tilde (~) symbol in a path. It retrieves the user's home directory and appends the remaining part of the path after the tilde.
char* expandTilde(char* path) {
    if (path[0] == '~') {
        char* homeDir;
        char* restOfPath;
        // Verify if there is a directory mentioned immediately after the tilde (~) symbol
        if (path[1] == '/' || path[1] == '\0') {
            homeDir = getenv("HOME"); // Retrieve the value stored in the HOME environment variable
            restOfPath = &path[1];    // Extract the remaining part of the path after the tilde (~)
        } else {
            // Extract the username provided after the tilde (~)
            char* usernameEnd = strchr(path, '/');
//The if statement handles the case when there is no username specified after the tilde (~) by checking if usernameEnd is NULL. If it is NULL, it means that the tilde represents the home directory itself, so we set usernameEnd to the null terminator (\0) to indicate the end of the path.
            if (usernameEnd == NULL) {
                usernameEnd = strchr(path, '\0');
            }
            // Extract the username from the path
            size_t usernameLen = usernameEnd - &path[1];
            //The line char username[usernameLen + 1]; declares a character array called username with a size of usernameLen + 1.
            char username[usernameLen + 1];
            //This line copies a substring from path to username using strncpy: strncpy(username, &path[1], usernameLen);
            strncpy(username, &path[1], usernameLen);
            //This line sets the null terminator at the end of the username string: username[usernameLen] = '\0';
            username[usernameLen] = '\0';
            
            // Get the home directory of the specified user
            struct passwd* pwd = getpwnam(username);
        //The block of code is written to handle the case when the pwd pointer is NULL, indicating that the specified user does not exist. It prints an error message to inform the user about the non-existent user and returns NULL.
            if (pwd == NULL) {
                fprintf(stderr, "User '%s' not found\n", username);
                return NULL;
            }
        //The code assigns the value of pwd->pw_dir to the homeDir variable, which represents the home directory of the specified user
            homeDir = pwd->pw_dir;
        // It assigns the value of usernameEnd to the restOfPath variable, which represents the remaining part of the path after the username.
            restOfPath = usernameEnd;
        }
        
        // This calculates the length of the homeDir string using the strlen function and stores it in the variable homeDirLen.
        size_t homeDirLen = strlen(homeDir);
        //calculates the length of the restOfPath string using the strlen function and stores it in the variable restOfPathLen
        size_t restOfPathLen = strlen(restOfPath);
    //allocates memory dynamically for the expandedPath string using malloc. The size of the allocated memory is determined by the sum of homeDirLen, restOfPathLen, and 1 (for the null terminator).
        char* expandedPath = malloc(homeDirLen + restOfPathLen + 1);
    //uses strcpy to copy the contents of the homeDir string into the expandedPath string. This ensures that the expandedPath string starts with the value of homeDir.
        strcpy(expandedPath, homeDir);
    //The code uses strcat to concatenate the contents of the restOfPath string to the end of the expandedPath string. This appends the value of restOfPath to the existing content of expandedPath
        strcat(expandedPath, restOfPath);
        
        return expandedPath;
    }
    
    return path;
}

//This function is used to parse a command string, identify special characters, and execute the command accordingly. It takes the input command string comma_Re as a parameter.This function supports various command options such as background execution, input/output redirection, piping, and sequential execution.
void handleCommand_Re(char *comma_Re) {
    //Provides the list of special characters used for command parsing
    const char *specialChar_Re = "|><&;";
    //this is a token to store individual command or option
    char *t_Re;
    //this is flag to inidcate background execution
    int bg_Re = 0;
    //this is a file descriptor for input redirection
    int input_file_d_Re = STDIN_FILENO;
    //this is a file descriptor for output redirection
    int output_file_d_Re = STDOUT_FILENO;
    //this is the file descriptor for pipe operations
    int pipefds_Re[2];
    //this is the counter for number of pipe operations
    int pipeCount_Re = 0;
    //the exit status of previous command 
    int previousStat_Re = 0;
    //this is the flag to determine if the command should be executed or not.
    int runCommand_fl_Re = 1;

//Reset the command arguement counter to 0.
    f_Re = 0;

//initializes the tokenization process by using the strtok function.
    t_Re = strtok(comma_Re, " \t\n");

//The while loop is written to iterate over each token (command or option) extracted from the input command string using the strtok function. The loop continues as long as there are tokens available(i.e., t_Re is not NULL)
    while (t_Re != NULL) {

//Here it checks if the current number of arguments (f_Re) has exceeded the maximum allowed number of arguments (MAX_ARGS_).If the condition evaluates to true, an error message is printed using printf to inform the user that they have exceeded the maximum number of arguments allowed.
        if (f_Re > MAX_ARGS_Re) {
            printf("The number of arguments provided has exceeded the maximum limit allowed.\n");
            return;
        }
//if statement checks if the current token (t_Re) is equal to the "&" character sequence.
        if (strcmp(t_Re, "&") == 0) {
//It checks if the number of arguments (f_Re) falls outside the expected range of valid values. If the condition evaluates to true, it means that the number of arguments is either less than 1 or greater than 5, which indicates an invalid command.
            if (f_Re < 1 || f_Re > 5) {
                printf("The command entered is not recognized.\n");
                return;
            }
//The runCommand_fl_Re flag is set to 0, indicating that the command should not be executed immediately (as it will be run in the background).
            runCommand_fl_Re = 0;
        //The variable bg_Re is set to 1, indicating that the command should be executed in the background.
            bg_Re = 1;
            break;

//else if handle the case when the command contains the && operator, which represents the logical "AND" operator.
        } else if (strcmp(t_Re, "&&") == 0) {
//In if block, it checks if the number of arguments (f_Re) falls outside the valid range of 1 to 5, if the condition is true then it indicates that it is an invalid command
            if (f_Re < 1 || f_Re > 5) {
                printf("The command entered is not recognized.\n");
                return;
            }
//The j_Re array is used to store command arguments.By assigning NULL, it marks the end of the argument list for a particular command.     
            j_Re[f_Re] = NULL;
    
//This if block is written to conditionally execute a single command based on the value of the runCommand_fl_Re variable. By checking the runCommand_fl_Re flag, the program can decide whether to execute a command or skip it based on the result of previous command evaluations or logical operators (&&, ||). This enables conditional command execution based on the success or failure of previous commands.
            if (runCommand_fl_Re) {
                runSingleCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
            }
            f_Re = 0;   //this line resets the value of f_Re to 0.

//This if-else block determines the value of the runCommand_fl_Re flag based on the value of previousStatus. If previousStatus is non-zero, runCommand_fl_Re flag is set to 0; otherwise, it is set to 1 to allow the execution of subsequent commands connected by &&.
            if (previousStat_Re != 0) {
                runCommand_fl_Re = 0;
            } else {
                runCommand_fl_Re = 1;
            }
//This block handles the logical OR operator (||) in the command. It validates the number of arguments and executes the command if necessary.
        } else if (strcmp(t_Re, "||") == 0) {
            if (f_Re < 1 || f_Re > 5) {
                printf("The command entered is not recognized.\n");
                return;
            }
//This line sets the current argument in the j_Re array to NULL, indicating the end of the argument list for the current command.
            j_Re[f_Re] = NULL;

//This if block is written to check if the runCommand_fl_Re flag is set to true. If it is true, it means that the current command should be executed. The runSingleCommand_Re function is then called to run the command with the provided input and output file descriptors, background flag, and the pointer to the previous status.
            if (runCommand_fl_Re) {
                runSingleCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
            }
//This statement is used to reset the value of f_Re integer to 0. It is necessary to reset the count of command arguments  to 0 after executing a command.
            f_Re = 0;

//The if-else block determines the value of runCommand_fl_Re based on previousStatus            
            if (previousStat_Re == 0) {
                runCommand_fl_Re = 0;
            } else {
                runCommand_fl_Re = 1;
            }
// This else if block checks if the token t_Re is a pipe symbol (|). If it is, it validates the number of arguments (f_Re) and prints an error message if it is outside the valid range.           
        } else if (strcmp(t_Re, "|") == 0) {
            if (f_Re < 1 || f_Re > 5) {
                printf("The command entered is not recognized.\n");
                return;
            }
//marks the end of the current command arguments by setting the pointer at index f_Re in the j_Re array to NULL.    
            j_Re[f_Re] = NULL;

//The if block is written to check if the number of pipe operations has exceeded the maximum limit allowed, which is 7. If the condition is true, an error message is been displayed.
            if (pipeCount_Re >= 7) {
                printf("The number of arguments provided has exceeded the maximum limit allowed.\n");
                return;
            }
//Incrementing pipeCount_Re to track the number of pipe operations encountered.
            pipeCount_Re++; 
//If creating a pipe using pipe() fails, display an error message using perror() and return from the function. 
            if (pipe(pipefds_Re) < 0) {
                perror("An error occurred while attempting to use a pipe ");
                return;
            }
 //Assigning the write end of the pipe (pipefds_Re[1]) to the variable output_file_d_Re. This ensures that the output of the current command will be written to the pipe for the next command to read. 
            output_file_d_Re = pipefds_Re[1];
 // Execute the piped command by calling the runPipedCommand_Re function with the appropriate arguments: input_file_d_Re, output_file_d_Re, bg_Re, and &previousStat_Re       
            runPipedCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
   // Close the write end of the pipe (pipefds_Re[1]).
            close(pipefds_Re[1]);
//Assign the read end of the pipe (pipefds_Re[0]) to the input_file_d_Re variable to use it as the input for the next command.
            input_file_d_Re = pipefds_Re[0];
  //Reset the command argument counter (f_Re) to 0 to start parsing the next command.
            f_Re = 0;
//If the current token is >, indicating output redirection, the next token is obtained using strtok and assigned to t_Re. If t_Re is NULL, it means there is no file specified for output redirection. In this case, a message is printed indicating that the command is not recognized, and the function returns.
        } else if (strcmp(t_Re, ">") == 0) {
            t_Re = strtok(NULL, " \t\n");
            if (t_Re == NULL) {
                printf("The command entered is not recognized.\n");
                return;
            }
//Open the file specified by t_Re for writing with appropriate flags and permissions, and assign the file descriptor to output_file_d_Re.          
            output_file_d_Re = open(t_Re, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

//If opening the file specified by t_Re for writing fails (i.e., output_file_d_Re < 0), an error message is printed using perror().
            if (output_file_d_Re < 0) {
                perror("The system encountered an error while attempting to open the file.");
                return;
            }
  
  //Set the next element of the j_Re array to NULL, indicating the end of the command arguments.
            j_Re[f_Re] = NULL;
//Execute the command with output redirection by calling the runOutputRedirectionCommand_Re function, passing the input file descriptor, output file descriptor, background flag, and a pointer to the variable holding the previous command's status.
            runOutputRedirectionCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
  //Reset the command argument counter to zero for the next command.
            f_Re = 0;

//The else if block handles the >> symbol for output redirection in append mode. It checks if the current token is >>, retrieves the next token as the file name, and validates its presence. If the file name is missing, an error message is printed, and the program returns. 
        } else if (strcmp(t_Re, ">>") == 0) {
            t_Re = strtok(NULL, " \t\n");
            if (t_Re == NULL) {
                printf("The command entered is not recognized.\n");
                return;
            }
//Open the file specified by t_Re for writing with appropriate flags and permissions, and assign the file descriptor to output_file_d_Re.     
            output_file_d_Re = open(t_Re, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  //If opening the file specified by t_Re for writing fails (i.e., output_file_d_Re < 0), an error message is printed using perror().
            if (output_file_d_Re < 0) {
                perror("The system encountered an error while attempting to open the file");
                return;
            }
//Set the next element of the j_Re array to NULL, indicating the end of the command arguments.
            j_Re[f_Re] = NULL;
//executes the command with output and append mode redirection using the runAppendRedirectionCommand_Re function
            runAppendRedirectionCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
    //Reset the command argument counter to zero for the next command.
            f_Re = 0;

//This else if block is written to handle input redirection. It checks if the current token (t_Re) is the "<" symbol, indicating that input redirection is specified for the command. 
        } else if (strcmp(t_Re, "<") == 0) {
            t_Re = strtok(NULL, " \t\n");
            if (t_Re == NULL) {
                printf("The command entered is not recognized.\n");
                return;
            }
        //Open the input file t_Re for reading and assign the file descriptor to input_file_d_Re.
            input_file_d_Re = open(t_Re, O_RDONLY);
    
    //Check if the input file t_Re could be opened successfully. If not, print an error message using perror() and return from the function.
            if (input_file_d_Re < 0) {
                perror("The system encountered an error while attempting to open the file");
                return;
            }
    //This block is used to handle the case when a semicolon (;) is encountered in the command string. It checks if the number of command arguments f_Re is valid (between 1 and 5), and if not, it prints an error message and returns from the function.

        } else if (strcmp(t_Re, ";") == 0) {
            if (f_Re < 1 || f_Re > 5) {
                printf("The command entered is not recognized.\n");
                return;
            }
    //Set the next element of the j_Re array to NULL, indicating the end of the command arguments.
            j_Re[f_Re] = NULL;
  
  //This if block is responsible for executing a sequential command when the condition runCommand_fl_Re is true. It calls the runSequentialCommand_Re function with the specified input file descriptor, output file descriptor, background flag, and a pointer to the previousStat_Re variable
            if (runCommand_fl_Re) {
                runSequentialCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
            }
     //Reset the command argument counter to zero for the next command.
            f_Re = 0;
    //This line sets the value of the runCommand_fl_Re variable to 1, indicating that a command should be executed
            runCommand_fl_Re = 1;
    
//This else block is executed when none of the special characters (&, &&, ||, |, >, >>, <, ;) are encountered. It also handles the expansion of the tilde (~) symbol in a path and stores the expanded path in the appropriate array for further processing. Also increments the f_Re variable to keep track of the number of command arguments.
        } else {
            j_Re[f_Re] = expandTilde(t_Re);
            if (j_Re[f_Re] == NULL) {
                return;
            }
            f_Re++;
        }
//This line uses strtok() function to extract the next token from the input string comma_Re
        t_Re = strtok(NULL, " \t\n");
    }

//This block of code checks if there are any remaining command arguments (f_Re > 0) after the loop ends.
    if (f_Re > 0) {
        if (f_Re < 1 || f_Re > 5) {
            printf("The command entered is not recognized.\n");
            return;
        }
//sets the next element in the j_Re array to NULL, indicating the end of the command arguments.
         j_Re[f_Re] = NULL;

//The if statement checks the value of the runCommand_fl_Re variable. If it is non-zero (i.e., true), it executes the runSingleCommand_Re() function. The runSingleCommand_Re() function is responsible for executing a single command.
        if (runCommand_fl_Re) {
            runSingleCommand_Re(input_file_d_Re, output_file_d_Re, bg_Re, &previousStat_Re);
        }
    }
}

//The program implements a simple shell that operates in an infinite loop. Within each iteration of the loop, it prompts the user for input, reads the input, removes leading and trailing whitespaces, expands tilde (~) symbols, and formats the command.
int main() {
    char comma_Re[MAX_INPUT_SIZE_Re]; //Buffer to store the user input
    int commandCount_Re = 0;  // Keep track of the count of commands.

    while (1) {         //Enters an infinite loop
        printf("mshell$ "); //Print the shell prompt
        if (fgets(comma_Re, sizeof(comma_Re), stdin) == NULL)
            break;      //If no input is provided break out of the loop

        if (strlen(comma_Re) > 1) {
            
            // Calculate the total number of commands.
            char *temp_Re = strdup(comma_Re);     // Generate a temporary duplicate of the input string.
            char *to_Re = strtok(temp_Re, "|><&;"); // Tokenize the string using special characters as delimiters
            while (to_Re != NULL) {
                commandCount_Re++;            // For every token, increment the command count by one.
                to_Re = strtok(NULL, "|><&;");  // Retrieve the subsequent token.
            }
            free(temp_Re);        // Deallocate the memory assigned to the temporary string.
            
//If the count of commands exceeds the maximum limit of 7, display an error message indicating that the maximum number of commands allowed has been exceeded.
            if (commandCount_Re > 7) {
                printf("The number of arguments provided has exceeded the maximum limit allowed.\n");
            } else {
                handleCommand_Re(comma_Re);  // Invoke the function to execute the user command
            }

            commandCount_Re = 0;  // Restore the command count to its initial value.
        }
    }

    return 0;
}
