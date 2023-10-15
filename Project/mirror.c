//Name1- Sai Renuka Telu
//StudentNO - 110096549
//Name2 - Kavya Seeramsetty
//StuentNo - 110099222
//Section -I


#include <sys/socket.h>      // Provides socket functions for network communication
#include <netinet/in.h>      // Defines structures for internet domain addresses
#include <arpa/inet.h>       // Provides functions for manipulating IP addresses
#include <stdio.h>           // Standard I/O functions like printf and scanf
#include <stdlib.h>          // General utilities and memory management functions
#include <unistd.h>          // Provides various system calls, including basic I/O
#include <errno.h>           // Defines error numbers and related error handling
#define __USE_XOPEN         
#include <string.h>          // String manipulation functions like strcpy, strcat
#include <dirent.h>          // Directory manipulation functions and types
#include <sys/types.h>       // Basic system data types
#include <signal.h>          // Defines signal handling functions and macros
#include <stdbool.h>         // Defines boolean data type and values
#include <sys/wait.h>        // Functions and macros for working with child processes
#include <fcntl.h>           // File control options and manipulation functions
#include <time.h>            // Time-related functions and data types
#include <sys/stat.h>        // Functions for working with file information
#include <zlib.h>            // Compression and decompression functions (gzip)
#include <limits.h>          // Defines various implementation-specific limits


#define MAX_TOKENS 8    // Maximum number of tokens parsed from input
#define CHUNK_SIZE 16384  // Size of data chunks for file transfer
#define MAX_CLIENTS 8    // Maximum number of concurrent clients
#define PORT1 3160       // Port number 1 for the server
#define PORT2 3252       // Port number 2 for the mirror


//Function declerations
void filesrch(char* file, int sd);  // Search for a specific file and provide details if found
void tarfgetz(int size1, int size2, bool unzip, int sd);  // Get files within specified size range
void getdirf(char* date1, char* date2, bool unzip, int sd);  // Get files within specified date range
void targzf(char* extensions[], int numFiles, bool unzip, int sd);  // Get files with specified extensions
void getConnectedClient(int);  // Handle the connection with a connected client
int processClient(char msg[], int sd);  // Process client messages and execute corresponding actions
bool getAck(int sd);  // Receive acknowledgment from the client
void sendAck(int sd);  // Send acknowledgment to the client


char exitMessage[] = "quit\n";  // Message to indicate that client has exited.
char welcomeMessage[] = "CONNECTION ESTABLISHED TO MIRROR \n\n Enter quit to exit\n \n";  // Welcome message sent to connected clients.
char fileFoundMessage[] = "Requested File Found!";  // Message indicating successful file retrieval
char fileNotFound[] = "Requested File Not Found!";  // Message indicating file not found
char downloadingMessage[] = "Downloading Files";  // Message indicating file download process
char errorMessage[] = "Please make another attempt: The command you entered is invalid!";  // Message for invalid client command
char getAction[] = "-1";  // Action code for indicating a "get" request
char unzipAction[] = "-3";  // Action code for indicating an "unzip" request
char fileFoundAction[] = "@#$%";  // Action code for indicating a successful file found response
char fileNotFoundAction[] = "-2";  // Action code for indicating a file not found response
char ack[] = "-ack";  // Action code for acknowledging successful communication

char rootDir[] = ".";  // The root directory path used as the base for file operations
/*--------------------------------------------------------------------------------------------------------*/
//The function send_file transfers a specified file over a network connection. It searches for a file named "temp.tar.gz" in the root directory, sends its contents to the connected client, and optionally unzips the file. It involves reading the file's contents, determining its size, and transmitting the data to the client.
void send_file(int sd, bool unzip) {
// The name of the file to be sent is "temp.tar.gz". This variable holds the filename for the transfer process
    char file[] = "temp.tar.gz";

// This variable dirName is used to store the directory path where the target file is located. It has a maximum length of 255 characters to accommodate the path.
    char dirName[255];
    strcpy(dirName, rootDir);  // Initialize dirName with rootDir
    strcat(dirName, "/");  // Concatenate rootDir with "/"
    DIR* dp;  // Directory pointer
    dp = opendir(dirName);  // Open the directory
    struct dirent* dirp;  // Directory entry pointer
    bool isFound = false;  // Flag to indicate if the file is found
//This while loop iterates through directory entries using the variable dp, checking for a match with the target file name. If a match is found and the entry is not a directory, it sends the file's content to a client, optionally unzipping it, and waits for acknowledgments. This loop handles locating and transmitting the target file to a connected client.
    while ((dirp = readdir(dp)) != NULL) {  // Iterate through directory entries
//The if block confirms if the current directory entry's name matches the target file's name and is not a directory. If true, it sends the file's details to the client, including content, size, and potential unzipping, facilitating file transmission.
        if (strcmp(dirp->d_name, file) == 0 && dirp->d_type != DT_DIR) {
            isFound = true;  // Mark file as found
            printf(" '%s'Requested file was found\n", file);  // Display a message indicating that the requested file has been found.
            write(sd, getAction, sizeof(getAction));  // Send get action to client
            printf("Requesting download of files in client\n");  // Print request message
            getAck(sd);  // Wait for acknowledgement
            write(sd, file, strlen(file));  // Send file name to client
            printf("Name of the file has been sent to the client\n");  // Print the name of the file that has been sent to client.
            getAck(sd);  // Wait for acknowledgement

            strcat(dirName, file);  // Concatenate directory name with file name
            int fd = open(dirName, O_RDONLY, S_IRUSR | S_IWUSR);  // Open the file with appropriate permissions.

            long fileSize = lseek(fd, 0L, SEEK_END);  //Determine the size of the file by using the lseek function to position the file pointer at the end of the file.
            write(sd, &fileSize, sizeof(long));  // Send file size to client
            printf("Size of the file has been sent to the client\n");  // Print the size of the file that was sent to client.
            getAck(sd);  // Wait for acknowledgement

            char* buffer = malloc(fileSize);  // Allocate memory for file data
            lseek(fd, 0L, SEEK_SET);  // Set file pointer to the beginning
            read(fd, buffer, fileSize);  // Read file data into buffer
            write(sd, buffer, fileSize);  // Send file data to client
            getAck(sd);  // Wait for acknowledgement

            if (unzip == true) {  // If unzip flag is true
                write(sd, unzipAction, sizeof(unzipAction));  // Send unzip action to client
                printf("Unzipping file\n");  // Print unzip message
                getAck(sd);  // Wait for acknowledgement
            }
            free(buffer);  // Free allocated memory
            close(fd);  // Close the file
            printf(" '%s'Sent file\n", file);  // Print file sent message
        }
    }
  // If the file was not found
    if (!isFound) {
        printf(" '%s' The requested file was not found\n", file);  // Display a file not found message
        write(sd, fileNotFound, sizeof(fileNotFound));  // Send file not found message to client
    }

    closedir(dp);  // Close the directory
}
/*--------------------------------------------------------------------------------------------------------*/

//The function compress_files is designed to create a compressed .tar.gz archive named "temp.tar.gz" by utilizing the tar utility. It takes an array of file names and their count as input. The function constructs a command that includes these file names and executes it using system, resulting in the creation of a compressed archive containing the specified files.
void compress_files(char* file_list[], int num_files) {
    char command[1000] = "tar -czvf temp.tar.gz";  // Initialize the compression command
//The for loop iterates through the array of file names and appends each file name to the existing command string, separated by spaces. This constructs a shell command for compressing the specified files into a single archive.
    for (int i = 0; i < num_files; i++) {
        strcat(command, " ");  // Add a space before each file name
        strcat(command, file_list[i]);  // Concatenate the file name to the command
    }
    system(command);  // Execute the compression command using the system
}
/*--------------------------------------------------------------------------------------------------------*/
//This function establishes communication with a connected client by sending a welcome message and receiving messages from the client. It continuously listens for messages, processes them using the processClient function, and handles client disconnection when the "exit" message is received. It facilitates client-server interaction within the defined protocol.
void getConnectedClient(int sd) {
    char message[255];  // Initialize buffer for received message
    int n;  // Initialize variable to store read result
    
    write(sd, welcomeMessage, sizeof(welcomeMessage));  // Send welcome message to client
    
    while (1)  // Infinite loop to keep receiving messages
//this if block is responsible for reading and processing messages from the connected client, handling the case where the client requests disconnection, and passing valid messages to be further processed by the server.
        if (n = read(sd, message, 255)) {  // Read message from client
            message[n] = '\0';  // Null-terminate the received message
            printf("Client - %d: %s", getpid(), message);  // Print client's message 
//this if block handles the situation where the client wishes to exit the communication, closing the connection and smoothly terminating the server process.
            if (!strcasecmp(message, exitMessage))  // Check if client wants to exit
            {
                printf("Client - %d disconnected!\n", getpid());  // Print client disconnection
                close(sd);  // Close the connection
                exit(0);  // Exit the process
            }

            processClient(message, sd);  // Process the client's message
        }
}
/*--------------------------------------------------------------------------------------------------------*/
//The function processClient processes the client's message by parsing the input command and arguments, then executing corresponding actions. It handles commands related to file searches, archiving, and sending files to the client. It checks for specific command patterns, extracts relevant data, and executes the appropriate functions to handle file operations while considering optional flags like unzipping. The function aims to interpret and respond to various client requests within the given protocol.
int processClient(char msg[], int sd) {
    char* command, * fileName;  // Declare variables for command and file name
    char tempMessage[255];  // Temporary buffer for the received message
    strcpy(tempMessage, msg);  // Copy the received message to the temporary buffer
    char* tokens[MAX_TOKENS];  // Array to store command tokens
    int numTokens = 0;  // Counter for the number of tokens extracted
    bool unzip = 0;  // Flag to indicate whether unzip option is specified
    char* token = strtok(tempMessage, " \n");  // Tokenize the received message

//this loop separates the command and its parameters from the received message, while also determining whether the "unzip" option is present in the command.
    while (token != NULL && numTokens < MAX_TOKENS) {
        // If the current token is "-u", set the unzip flag to true.
        if (strcmp(token, "-u") == 0) {
            unzip = true;
        }
        //Otherwise, store the token in the tokens array and increment the token count.
        else {
            tokens[numTokens++] = token;
        }
        // Move to the next token using strtok, considering spaces and newline characters as delimiters.
        token = strtok(NULL, " \n");
    }
//If first token matches "filesrch", execute the filesrch function using the second token as an argument.
    if (strcmp(tokens[0], "filesrch") == 0) {
        //if only ffirst token filesrch is provided without any filename it displays an error message.
        if (numTokens==1)
        write(sd, "Error: No filename provided.", strlen("Error: No filename provided."));
    //check if more than 2 files are given for search then display an error
    else if(numTokens>2){
        write(sd,"Maximum limit exceeded for filesrch", strlen("Maximum limit exceeded for filesrch"));
    }
    else
        filesrch(tokens[1], sd);//Call the filesrch function with the specified file name
    }
//if the number of tokens is at most 4 and the first token matches "tarfgetz", convert the second and third tokens to integers and execute the tarfgetz function with the converted integers, the unzip flag, and the socket descriptor as arguments.
    else if (numTokens <= 4 && strcmp(tokens[0], "tarfgetz") == 0) {
        int size1 = atoi(tokens[1]);  // Convert the token to integer
        int size2 = atoi(tokens[2]);  // Convert the token to integer
        tarfgetz(size1, size2, unzip, sd);  // Call the tarfgetz function with the specified sizes
    }
//if the number of tokens is at most 4 and the first token matches "getdirf", assign the second and third tokens to date1 and date2 respectively, then execute the getdirf function with date1, date2, the unzip flag, and the socket descriptor as arguments.
    else if (numTokens <= 4 && strcmp(tokens[0], "getdirf") == 0) {
        char* date1 = tokens[1];  // Store the token as date1
        char* date2 = tokens[2];  // Store the token as date2
        //check if no dates are provided then display an error message
        if((tokens[1]==NULL && tokens[2]==NULL) ||(tokens[1]!=NULL && tokens[2]==NULL))
        write(sd, "Please provide date1 and date2", strlen("Please provide date1 and date2"));
        else
        getdirf(date1, date2, unzip, sd);  // Call the getdirf function with the specified dates
    }
//The purpose of the else block is to handle the case where the command from the client is "fgets". It processes the provided files, checks their existence, lists them, and sends them to the client after compressing if necessary. It ensures graceful handling of file-related operations based on client input.
 else if (strcmp(tokens[0], "fgets") == 0) {
    char* files[] = { NULL };  // Initialize an array to store file names
        int numFiles = 0;  // Counter for the number of files specified
        int i;
        bool proceed = true;  // Flag to determine if the command should proceed
//This for loop processes the tokens provided by the client's "fgets" command, adding filenames to the files array while handling file limits and skipping the "-u" flag if present. It ensures a maximum of 4 files and avoids exceeding this limit.
    for (i = 1; i < numTokens; i++) {
    //Check if the file limit is exceeded, if yes, send a limit-exceeded message and set the proceed flag to false.
        if (numFiles >= 4) {    
            write(sd, "File limit exceeded. Maximum 4 files are allowed.", strlen("File limit exceeded. Maximum 4 files are allowed."));
            proceed = false; // Set the flag to indicate not to proceed
            break; // Break out of the loop when the limit is exceeded
        }
        if (strcmp(tokens[i], "-u") == 0) { //If the current token is "-u", skip it.
            // Skip the -u flag
            continue;
        }  
    //Otherwise, add the token as a file name and increment the file count.
        files[numFiles++] = tokens[i];
    }
//This if block is written to process the collected file names, search for them in the current directory, and send the results back to the client. It checks if the proceed flag is true and there are files to process. It iterates through the files, searching for each one in the directory, and prepares a list of found files. If any files are found, it sends a message with their names to the client and compresses and sends those files if needed.
if (proceed && numFiles > 0) {  // If proceeding is allowed and there are files to process
    DIR* dir;  // Directory pointer
    struct dirent* ent;  // Directory entry structure
    int file_count = 0;  // Count of found files
    char* file_list[1000];  // Array to hold found file names
    // Check if each file exists in the current directory
    for (int i = 0; i < numFiles; i++) {
        if ((dir = opendir(".")) != NULL) {  // Open current directory
            while ((ent = readdir(dir)) != NULL) {  // Iterate through directory entries
                if (strcmp(ent->d_name, files[i]) == 0) {  // If file name matches
                    file_list[file_count] = strdup(ent->d_name);  // Add file name to the list
                    file_count++;
                }
            }
            closedir(dir);  // Close the directory
        }
        else {
            // Failed to open directory
            perror("");  // Display an error message
            exit(EXIT_FAILURE);  // Exit with failure status
        }
    }
    if (file_count == 0) {  // If no files were found
        write(sd, fileNotFound, sizeof(fileNotFound));  // Send file not found message to client if no files were found
        printf("The requested files were not found\n");  //Display a message on console that no files were found.
    }
    else {
        char files_found[] = "The following files with the specified extensions were found:";  // Message to display on console if files found 
        write(sd, files_found, sizeof(files_found));  // Send message to client
        printf("The requested files were found:\n");  // Display message to server console
        for (int i = 0; i < file_count; i++) {
            write(sd, file_list[i], sizeof(file_list[i]));  // File names were sent to client.
        }
    }
    // Compress the found files if necessary
    if (file_count > 0) {
        compress_files(file_list, file_count);  // Compress the found files
        send_file(sd, unzip);  // Send the compressed files to client
    }
}
//This block sends a message to the client if there are no files provided for processing, and the proceeding is allowed based on the previous conditions.
         else if (proceed) {
          write(sd, "No files provided.", strlen("No files provided."));
            }
    }
//The else block manages the "targzf" command to search for files with specified extensions. It handles extension processing, limits their count, and decides on further actions. If valid extensions are provided, it invokes the "targzf" function; otherwise, it informs the absence of extensions or sends an error message if the command is unrecognized.
    else if (strcmp(tokens[0], "targzf") == 0) {  // Check if command is "targzf"
    char* extensions[] = { NULL };  // Initialize array to store extensions
    int numExtensions = 0;  // Initialize count of extensions
    int i;
    bool proceed = true;  // Flag for command processing
    for (i = 1; i < numTokens && numExtensions <= 4; i++) {  // Iterate through tokens for extensions
        if (numExtensions>= 4) {  // Check if extension limit is exceeded
            write(sd, "Extensions limit exceeded. Maximum 4 extensions are allowed.", strlen("Extensions limit exceeded. Maximum 4 extensions are allowed."));
            proceed = false;  // Set flag to stop processing
            break;  // Exit loop when limit is exceeded
        }
        if (strcmp(tokens[i], "-u") == 0) {  // Skip the unzip flag
            continue;
        }
        extensions[numExtensions++] = tokens[i];  // Store extension in array
    }
    
    if (proceed && numExtensions > 0) {  // If processing is allowed and extensions are provided
        targzf(extensions, numExtensions, unzip, sd);  // Call targzf function
    } else if (proceed) {  // If processing is allowed but no extensions provided
        write(sd, "No extensions provided.", strlen("No extensions provided."));
    }
}
    else {
        write(sd, errorMessage, strlen(errorMessage));  // Send error message for unrecognized command
    }
}

/*--------------------------------------------------------------------------------------------------------*/

//This function is designed to search for a requested file within a specified directory. It takes the name of the file and the socket descriptor as inputs. The function searches through the directory for the specified file, retrieves its information (size and creation time), and sends this information back to the client through the provided socket descriptor. If the file is not found, it sends an appropriate message indicating that the file was not found.
void filesrch(char* file, int sd) {
    char dirName[255];  // Stores the directory name
    strcpy(dirName, rootDir);  // Initialize the directory name with root directory
    strcat(dirName, "/");  // Append a slash to the directory name
    DIR* dp;  // Directory pointer
    dp = opendir(dirName);  // Open the directory
    if (dp == NULL) {  // Check if the directory is invalid
        printf("Not a valid directory\n");  // Display error message if directory is not valid.
        return;  // Exit the function
    }
    struct dirent* dirp;  // Directory entry structure
    struct stat filestat;  // File status structure
    bool isFound = false;  // Flag to track if the file is found
//The while loop is to search for a specific file within the specified directory and send its information to the client if found. If the file is not found, an appropriate message is sent to the client.
    while ((dirp = readdir(dp)) != NULL) {  // Loop through directory entries
        if (strcmp(dirp->d_name, ".") == 0 && strcmp(dirp->d_name, "..") == 0) {
            continue;  // Skip current and parent directory entries
        }
        strcat(dirName, dirp->d_name);  // Concatenate the directory name with file name
        if (stat(dirName, &filestat) < 0) {  // Get file status
            printf("Error due to file status\n");  // Print error message if not a valid status
            return;  // Exit the function
        }
//this condition checks if the current entry in the directory is a directory itself. If it is a directory it skips as the function purpose is to handles files not directories.
        if (S_ISDIR(filestat.st_mode)) {
            // skip the directory
        }
//When a file matching the requested name is found, this code segment generates a message with file details (name, size, creation time), sends it to the client, marks the file as found, and exits the loop.
        else if (strcmp(dirp->d_name, file) == 0) {  // Compare file name with requested file
            char file_info[1000];  // Buffer to store file information
            sprintf(file_info, "Found file %s\n  Size: %ld bytes\n  Created: %s", file, filestat.st_size, ctime(&filestat.st_ctime));  // Format file information
            write(sd, file_info, strlen(file_info));  // Send file information to the client
            isFound = true;  // Mark the file as found
            break;  // Exit the loop
        }
        strcpy(dirName, rootDir);  // Reset directory name for the next iteration
        strcat(dirName, "/");  // Append a slash to the directory name
    }
    if (!isFound) {  // If the file was not found
        printf(" '%s' Request file not found\n", file);  // Print message indicating file not found
        write(sd, fileNotFound, sizeof(fileNotFound));  // Send file not found message to the client
    }

    closedir(dp);  // Close the directory
}
/*--------------------------------------------------------------------------------------------------------*/
//This function searches for files within a specified size range in the current directory, compiles a list of files that match the criteria, and sends the list to the client. If requested, it can also compress and send the files to the client. It handles various size constraints and invalid inputs to ensure proper functionality.
void tarfgetz(int size1, int size2, bool unzip, int sd) {
    DIR* dir;  // Directory pointer
    struct dirent* ent;  // Directory entry structure
    struct stat st;  // File status structure
    char path[1000];  // Path buffer
    int file_count = 0;  // Counter for matching files
    char* file_list[1000];  // Array to store matching file names

//This if block reads through the current directory to find files within a specific size range (size1 to size2). It skips current and parent directory entries, collects file status using lstat, and if a file's size falls within the specified range, its name is added to the file_list. This process is aimed at identifying files meeting the size criteria within the current directory
  if ((dir = opendir(".")) != NULL) {  // Open the current directory for reading
    while ((ent = readdir(dir)) != NULL) {  // Iterate through each entry in the directory
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;  // Skip current and parent directory entries
        }
        if (lstat(ent->d_name, &st) == -1) {
            continue;  // Skip if unable to get file status
        }
        if (!S_ISDIR(st.st_mode)) {  // Check if the entry is not a directory
            int file_size = st.st_size;  // Get the size of the file
            if (file_size >= size1 && file_size <= size2) {  // Check if the file size is within the specified range
                file_list[file_count] = strdup(ent->d_name);  // Add the file name to the list
                file_count++;  // Increment the count of found files
            }
        }
    }
    closedir(dir);  // Close the directory
}
//This else block is executed if the attempt to open the current directory using opendir fails. It prints an error message using perror to the standard error stream and exits the program with a failure status using exit(EXIT_FAILURE)
  else {
    // Failed to open directory
    perror("");  // Print an error message related to the failed directory opening
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}
//this part of the code is responsible for communicating to both the client and the server that no files meeting the size criteria were found in the directory.
    if (file_count == 0) {  // If no files were found within the specified size range 
    // Send a message to the client indicating that no files were found
    write(sd, fileNotFound, sizeof(fileNotFound));  
    // Print a message to the server's console indicating the same
    printf("None of the files were found.\n");
}
//This else block handles the scenario where files were found in the directory that match the specified size range. It sends a message to the client indicating the found files, prints relevant information to the server's console, and sends the list of file names to the client for further processing.
    else {
    // Prepare a message indicating that files were found and the specified sizes
    char files_found[] = "The following files with the specified sizes were found:";
    // Send the files_found message to the client
    write(sd, files_found, sizeof(files_found));
    // Print a message to the server's console indicating the same
    printf("The following files were found:\n");
    // Print the count of found files to the server's console
    printf("%d", file_count);
    // Loop through the list of found files and process each one
    for (int i = 0; i < file_count; i++) {
        // Print the name of the current file to the server's console
        printf("%s\n", file_list[i]);
        
        // Send the name of the current file to the client
        write(sd, file_list[i], sizeof(file_list[i]));
    }
}
//This code block checks if any files match the specified size range. If found, it compresses the files into a tar archive and sends it to the client for download, potentially unzipping it based on the 'unzip' flag.
   if (file_count > 0) {
    // Compress the list of found files into a tar archive
    compress_files(file_list, file_count); 
    // Send the compressed tar archive to the client for download
    send_file(sd, unzip);
}

}
/*--------------------------------------------------------------------------------------------------------*/

//The function getdirf is written to search and retrieve files within a specified date range in the current directory. It takes two date strings, date1 and date2, along with flags for unzipping and a socket descriptor. It parses the date strings, compares file modification times within the given range, and sends the list of found files to the client. If the unzip flag is set, it also compresses and sends the files. The function ensures proper handling of missing or invalid date inputs.
void getdirf(char* date1, char* date2, bool unzip, int sd) {
    DIR* dir;  // Directory structure
    struct dirent* ent;  // Directory entry structure
    struct stat st;  // File status structure
    char path[1000];  // Buffer for file path
    int file_count = 0;  // Count of found files
    char* file_list[1000];  // Array to store found file names

    struct tm date1_tm = { 0 };  // Initialize struct to store parsed date1
    struct tm date2_tm = { 0 };  // Initialize struct to store parsed date2
    strptime(date1, "%Y-%m-%d", &date1_tm);  // Parse date1 string into struct tm format
    strptime(date2, "%Y-%m-%d", &date2_tm);  // Parse date2 string into struct tm format
    time_t date1_t = mktime(&date1_tm);  // Convert parsed date1 struct tm into a time_t value
    time_t date2_t = mktime(&date2_tm);  // Convert parsed date2 struct tm into a time_t value

//This if block checks if date1 is greater than or equal to date2 and handles the case when the condition is true. It sends an error message to the client indicating that date1 must be less than date2 and then returns from the function.
  if (date1_t >= date2_t) {
    // Check if date1 is greater than or equal to date2
    char invalidDates[] = "Invalid command. date1 must be less than date2.";
    write(sd, invalidDates, sizeof(invalidDates));  // Send error message to the client
    //printf("Invalid command. date1 must be less than date2.\n");
    return;  // Return from the function
}

//This if block is written to traverse through the contents of the current directory and filter files based on their modification time (timestamp). It skips the current and parent directory entries, and for each regular file, it compares the file's modification time with the provided date range (date1 to date2). If the file's modification time falls within the specified range, the file's name is added to the list of files. The block also ensures that the directory is opened successfully, and after processing, it closes the directory.
  if ((dir = opendir(".")) != NULL) {
    // Open the current directory
    while ((ent = readdir(dir)) != NULL) {
        // Loop through directory entries
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;  // Skip current and parent directory entries
        }
        if (lstat(ent->d_name, &st) == -1) {
            continue;  // Skip if unable to get file status
        }
        if (!S_ISDIR(st.st_mode)) {
            // Check if the entry is not a directory
            time_t file_time = st.st_mtime;
            // Get the file modification time
            if (file_time >= date1_t && file_time <= date2_t) {
                // Check if the modification time falls within the given range
                file_list[file_count] = strdup(ent->d_name);
                // Add the file name to the list
                file_count++;
            }
        }
    }
    closedir(dir);  // Close the directory
}
   else {
    // Failed to open directory
    perror("");  // Print an error message
    exit(EXIT_FAILURE);  // Terminate the program with an error status
}
//if block checks if no files were found within the specified date range. If no files were found, it sends a message to the client indicating that no files were found and also prints a message to the console.
   if (file_count == 0) {
    write(sd, fileNotFound, sizeof(fileNotFound));  // Send a message indicating no files were found
    printf("None of the files were found.\n");  // Print a corresponding message to the console
}
//This else block is executed when files are found within the specified date range. It sends a message to the client indicating that files were found and also prints a message to the console. Then, it iterates through the list of found file names and sends each name to the client.
 else {
    char files_found[] = "The following files with the specified dates were found:";
    write(sd, files_found, sizeof(files_found));  // Send a message indicating files were found
    printf("The following files were found:\n");  // Print a corresponding message to the console
    for (int i = 0; i < file_count; i++) {
        write(sd, file_list[i], sizeof(file_list[i]));  // Send each found file name to the client
    }
}
// if block is executed if files were found within the specified date range. It calls the compress_files function to compress the found files and then uses the send_file function to send the compressed files to the client, if the unzip flag is true.
   if (file_count > 0) {
    compress_files(file_list, file_count);  // Compress the found files
    send_file(sd, unzip);  // Send the compressed files to the client if unzip is true
    }
}

/*--------------------------------------------------------------------------------------------------------*/
//The function targzf is designed to search for files with specified extensions in the current directory. It takes an array of extensions, the number of extensions, a flag to indicate whether to unzip the files, and a socket descriptor for communication with a client. It searches for files matching the given extensions, lists them, and if found, compresses and optionally sends them to the client
void targzf(char* extensions[], int numFiles, bool unzip, int sd) {
    DIR* dir;  // Directory structure pointer
    struct dirent* ent;  // Directory entry structure pointer
    int file_count = 0;  // Counter for found files
    char* file_list[1000];  // Array to store file names
    // Check if each file with the specified extension exists in the current directory
    if ((dir = opendir(".")) != NULL) {  // Open the current directory
        while ((ent = readdir(dir)) != NULL) {  // Iterate through directory entries
            for (int i = 0; i < numFiles; i++) {  // Loop through the specified extensions
                const char* extension = extensions[i];  // Get the current extension
                size_t extension_length = strlen(extension);  // Length of the extension
                size_t name_length = strlen(ent->d_name);  // Length of the entry's name
                if (name_length > extension_length && strcmp(ent->d_name + name_length - extension_length, extension) == 0) {   // Compare file extension with the given extension  
                    file_list[file_count] = strdup(ent->d_name);  // Store the file name in the list
                    file_count++;  // Increment the file count
                    break;  // Move to the next directory entry
                }
            }
        }
        closedir(dir);  // Close the directory
    }
    else {
        // Failed to open directory
        perror("");  // Print error message
        exit(EXIT_FAILURE);  // Terminate the program
    }

//In this block, if no files with the specified extensions are found in the directory, it sends a message to the client indicating that no files were found, and also prints the same message to the console for local debugging or logging purposes.
    if (file_count == 0) {
        char none_extensions[] = "None of the files with the specified extensions were found.\n";
        write(sd, none_extensions, sizeof(none_extensions));  // Send no files found message
        printf("None of the files with the specified extensions were found.\n");  // Print message if no files found with given extensions.
    }
//In this block, if files with the specified extensions are found in the directory, it sends a message to the client indicating that files were found, and also prints the same message to the console for local debugging or logging purposes. Then, it iterates through the list of file names and sends each file name to the client.
    else {
        char extensions_found[] = "The following files with the specified extensions were found:";
        write(sd, extensions_found, sizeof(extensions_found));  // Send files found message
        printf("The following files with the specified extensions were found:\n");  // Print message
        for (int i = 0; i < file_count; i++) {
            write(sd, file_list[i], sizeof(file_list[i]));  // Send each file name to client
        }
    }

    // Compress the found files if necessary
    if (file_count > 0) {
        compress_files(file_list, file_count);  // Compress files
        send_file(sd, unzip);  // Send compressed files to client
    }
}
/*--------------------------------------------------------------------------------------------------------*/
//This function sends an acknowledgment message (ack) to the client using the provided socket descriptor sd. It acknowledges the successful receipt or completion of a certain operation
void sendAck(int sd) {
    write(sd, ack, sizeof(ack));  // Send an acknowledgment message to the client
}
/*--------------------------------------------------------------------------------------------------------*/
//This function reads an acknowledgment message from the client using the provided socket descriptor sd. It compares the received acknowledgment message with the expected acknowledgment (ack) and returns true if they match, indicating successful acknowledgment. Otherwise, it returns false.
bool getAck(int sd) {
    char ackMsg[sizeof(ack)];
    read(sd, ackMsg, sizeof(ackMsg));  // Read acknowledgment message from client
    if (strcmp(ack, ackMsg) == 0)  // Compare received acknowledgment with expected acknowledgment
        return true;  // Return true if acknowledgments match
    else
        return false;  // Return false if acknowledgments do not match
}
/*--------------------------------------------------------------------------------------------------------*/
//This is a main function that sets up a server to listen for incoming connections, accept clients, and fork child processes to handle client requests concurrently.  It creates a socket, binds it to a specific port, and accepts client connections. For each client, it increments a client count, sends it to the client, and spawns a child process to handle client interactions using the getConnectedClient function. The server continues to accept and process clients in an infinite loop.
int main(int argc, char* argv[]) {
    
    int countServer1 = 6; // Initialize a variable to keep track of the number of clients connected to the server.
    int sd1, client; // Declare variables to hold socket descriptors for the server and client connections.
    struct sockaddr_in socAddr; // Declare a sockaddr_in structure to configure the server's socket address.
    int opt = 1; // Initialize a variable to set socket options.
    int addrlen = sizeof(socAddr); // Initialize a variable to store the size of the sockaddr_in structure.


//a socket is created using the socket function. If the socket creation fails (socket returns a negative value), an error message is printed to the standard error stream, and the program exits with an error status.
    if ((sd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Cannot create socket\n"); // Print an error message if socket creation fails.
    exit(1); // Exit the program with an error status.
}
//Forcefully attaching socket to the port. The setsockopt function is used to set socket options. If setting the options fails, an error message is printed, and the program exits with an error status. The SO_REUSEADDR and SO_REUSEPORT options are being set, which allow reuse of local addresses and ports.
    if (setsockopt(sd1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    perror("setsockopt"); // Print an error message if setting socket options fails.
    exit(EXIT_FAILURE); // Exit the program with an error status.
}
    
    socAddr.sin_family = AF_INET; // Set the address family to AF_INET (IPv4).
    socAddr.sin_addr.s_addr = INADDR_ANY; // Set the IP address to any available local address.
    socAddr.sin_port = htons(PORT2); // Set the port number using the htons function to convert to network byte order.


//the bind function is used to associate a socket with a specific IP address and port number. If the binding operation fails, an error message is printed using perror, and the program exits with an error status using exit(EXIT_FAILURE).
    if (bind(sd1, (struct sockaddr *)&socAddr, sizeof(socAddr)) < 0) {
        perror("bind failed"); // Print error message
        exit(EXIT_FAILURE); // Exit with error status
    }

//the listen function is used to make the server socket listen for incoming connections. If the listening operation fails, an error message is printed using perror, and the program exits with an error status using exit(EXIT_FAILURE).
    if (listen(sd1, MAX_CLIENTS) < 0) {
        perror("listen"); // Print error message
        exit(EXIT_FAILURE); // Exit with error status
    }

//In an infinite loop, the server accepts incoming client connections, increments the client count, and sends the count to the client. Then, it creates a child process to handle client interactions and closes the client socket before continuing the loop.
 while (1) {
    client = accept(sd1, (struct sockaddr *)NULL, NULL); // Accept a new client connection
    countServer1 = countServer1 + 1; // Increment the client count
    char cs[2];
    sprintf(cs, "%d", countServer1); // Convert client count to string
    int valread = write(client, cs, 2); // Send client count to client

    if (!fork()) {
        //printf("Client %d is connected!\n", getpid());
        getConnectedClient(client); // Handle client interactions in child process
    }

    close(client); // Close the client socket
}
    return 0; // Return success status
}

