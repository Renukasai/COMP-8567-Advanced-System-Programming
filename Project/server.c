//Name1- Sai Renuka Telu
//StudentNO - 110096549
//Name2 - Kavya Seeramsetty
//StuentNo - 110099222
//Section -I



// Import the socket operations header file.
#include <sys/socket.h>
//Import the header file that defines structures for Internet addresses.
#include <netinet/in.h>
// Import the header file containing functions for manipulating IP addresses.
#include <arpa/inet.h>
// Import the header file for standard input/output operations.
#include <stdio.h>
// Import the header file for the standard library.
#include <stdlib.h>
// Import the header file for the POSIX API, which provides operating system services.
#include <unistd.h>
// Import the header file containing definitions for error numbers.
#include <errno.h>
// Import the header file that provides functions for manipulating strings.
#include <string.h>
// Import the header file for performing operations on directories.
#include <dirent.h>
// Import the header file for defining system data types.
#include <sys/types.h>
// Import the header file for handling signals.
#include <signal.h>
// Import the header file for the boolean data type.
#include <stdbool.h>
// Import the header file for functions related to waiting.
#include <sys/wait.h>
#define __USE_XOPEN // to enable macros
// Import the header file for file control options.
#include <fcntl.h>
// Import the header file for functions related to time.
#include <time.h>
// Import the header file for file status information.
#include <sys/stat.h>
// Import the header file for compression and decompression functions.
#include <zlib.h>
// Import the header file for implementation-defined constants.
#include <limits.h>
// LIST OF CONSTANTS
// The maximum count of tokens permitted within a structure that has been tokenized.
#define MAX_TOKENS 8
// The data segment size, commonly employed in file or network operations.
#define CHUNK_SIZE 16384
// The highest quantity of clients that a server is capable of accommodating.
#define MAX_CLIENTS 8
// Port number 1, typically employed for network communication.
#define PORT1 3160
// Port number 2, typically employed for a distinct network communication.
#define PORT2 3252
//declaratiohs for cli processinhf
// Define a method that performs a search for a file using both a filename and a socket descriptor.
void filesrch(char* file, int sd);
// Declare a method for retrieving data, including size parameters, an unzip flag, and a socket descriptor.
void tarfgetz(int size1, int size2, bool unzip, int sd);
//method declaration for that retrieves data using specified size parameters, an unzip indicator, and a socket descriptor.
void getdirf(char* date1, char* date2, bool unzip, int sd);
// Declaration of a function for handling a certain number of files, unzip flag, and socket descriptor.
void targzf(char* extensions[], int numFiles, bool unzip, int sd);
//methods whihc aer cruicial fro server-proceing
// Define a mechanism for managing connected clients using a descriptor.
void getConnectedClient(int);
// Define a process for handling client messages, utilizing a message array and a socket descriptor, and returning an integer value.
int processClient(char msg[], int sd);
// Specify a function that receives an acknowledgment from a socket descriptor and returns a boolean value.
bool getAck(int sd);
// Acknowledgment sent to a socket descriptor.
void sendAck(int sd);
//various acknowledgements raed
// Text indicating the "quit" command.
char exitMessage[] = "quit\n";
// Greeting message for the server.
// Notification of file discovery.
char welcomeMessage[] = "CONNECTION ESTABLISHED TO SERVER\n\n \
> Enter quit to exit\n \\n";
// Indication of file presence
char fileFoundMessage[] = "File Found!";
// Indication of file presence
char fileNotFound[] = "File Not Found!";
// Describing the ongoing process of file download.
char downloadingMessage[] = "Downloading Files";
// Alert for an invalid command error.
char errorMessage[] = "Try Again : Command is not valid!";
//various flgs thre 
// Identifier for the "get" operation.
char getAction[] = "-1";
// Identifier for the "unzip" operation.
char unzipAction[] = "-3";
// Identifier for signaling the presence of a file.
char fileFoundAction[] = "@#$%";
// Identifier for signaling the absence of a file.
char fileNotFoundAction[] = "-2";
// Identifier for an acknowledgment action.
char ack[] = "-ack";
// Base directory of the server.
char rootDir[] = ".";
//This function is in charge of transmitting a file to the client through the network.
// Socket descriptor used for communication with the client.
// boolean value indicating whether to unzip the file after transmission.
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void send_file(int sd, bool unzip) 
{
    //information requested flie
    char file[] = "temp.tar.gz";
    //characer aray to hld the drectory nme.
    char dirName[255];
    // opy he rot diretory pah o diName.
    strcpy(dirName, rootDir);
    // Cocatenate ath searator drName.
    strcat(dirName, "/");
     // Directry poiter r acessing th diectory.
    DIR* dp;
    // Ope te speified direcory.
    dp = opendir(dirName);
     // Structue  hol direcory enty infomation.
    struct dirent* dirp;
    // fag indicatnig whenflie requestde nto fuond
    bool isFound = false;
    //lopnig throgh diretory enties.
     while ((dirp = readdir(dp)) != NULL)
    {
       if (strcmp(dirp->d_name, file) == 0 && dirp->d_type != DT_DIR)
        {
            // validate whethre crrent enry maches desred fle nme ad isdiectory.
            //setting the value of hte flag to correct
            isFound = true;
            //knowing the client know that the information is theer
            printf("'%s' Requested file was found\n", file);
            //actionidentifier the code
            write(sd, getAction, sizeof(getAction));
            //know the client for the information is downloadign
            printf("Requesting download of files in client\n");
            //waitnng acknowledge form cli
            getAck(sd);
            // providingn fil to teh cli
            write(sd, file, strlen(file));
            // requetde informatoin setn 
            printf("Name of the file has been sent to the client\n");
            //waitnig fro teh ackmwledgemwnrt
            getAck(sd);
            //concatenatign requestde informatoin
            strcat(dirName, file);
            // opening-fil_for_reading
            int fd = open(dirName, O_RDONLY, S_IRUSR | S_IWUSR);
            //know teh file_size
            long fileSize = lseek(fd, 0L, SEEK_END);
            //response form teh srever to-teh cli
            write(sd, &fileSize, sizeof(long));
            printf("Size of the file has been sent to the client\n");
             //waitnig fro teh ackmwledgemwnrt
            getAck(sd);
            // Make space available for the file buffer.
            char* buffer = malloc(fileSize);
            // Go to the file's beginning in seek mode.
            lseek(fd, 0L, SEEK_SET);
            // Read-the contents of the file into the buffer.
            read(fd, buffer, fileSize);
            // Transmit the client's file content.
            write(sd, buffer, fileSize);
            //waitnig fro teh ackmwledgemwnrt
            getAck(sd);
            if (unzip == true) 
            {
                // If the 'unzip' flag is set, send an action identifier for unzipping.
                write(sd, unzipAction, sizeof(unzipAction));
                //insights mailed
                printf("Unziping file");
                //waitnig fro teh ackmwledgemwnrt
                getAck(sd);
            }
            //spave is avaianle
            free(buffer);
            //terminaet
            close(fd);
            //displya teh notifiaction
            printf("'%s' Sent file\n", file);
        }
    }
    if (!isFound)
    {
        // indisghts foutd
        printf("'%s'The requested file was not found\n", file);
        //make the client know-teh notificatino
        write(sd, fileNotFound, sizeof(fileNotFound));
    }
    // ternimaet
    closedir(dp);
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// Teh below-method is resonbile fro compresing requider informatino fiel
void compress_files(char* file_list[], int num_files) 
{
    // Initialize teh comand wih opions.
    char command[1000] = "tar -czvf temp.tar.gz";
    // loopnig throuh he lit  fies and add each to the ueds command.
    for (int i = 0; i < num_files; i++) 
    {
         // ensuiring space-separatro
        strcat(command, " ");
        // ensuring crrent filepath cmmand.
        strcat(command, file_list[i]);
    }
    // Execute the-command along with syadtem shel
    system(command);
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//method is reponsible for hndling communicaion wth conected cient.
void getConnectedClient(int sd) 
{
    // arsay holding the received-mesage
    char message[255];
    // varto strie numberof bytse ared form teh-sockey
    int n;
    // gerrting eth cliwnt
    write(sd, welcomeMessage, sizeof(welcomeMessage));
    // Continuously receive messages from the client.
    while (1)
       if (n = read(sd, message, 255)) 
       {
        // daa is reeived frm he lient, pocess it
        // Nul-teminatereceied mesage
            message[n] = '\0';
            //displying-mesage
            printf("Client - %d: %s", getpid(), message);
            if (!strcasecmp(message, exitMessage))
            {
                 // If the cliet sens an xit mesage, discnnect an eit.
                printf("Client - %d disconnected!\n", getpid());
                close(sd);
                //exti
                exit(0);
            }
            // Proess  cliet's mesage.
            processClient(message, sd);
        }
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//The functin processClint is resonsible fr inerpreting lient instrutions, lke seaching fo fies or peforming comression, 
//and it carres ot te apprpriate ations whie snding te oucomes ack though th iven ocket desriptor.
int processClient(char msg[], int sd) 
{
    
    // Define pointers for the command and the file name.
    char* command, * fileName;
    // Generate a temporary message buffer and duplicate the received message into it.
    char tempMessage[255];
    strcpy(tempMessage, msg);
    // Establish an array of token pointers and a variable to keep track of the token count.
    char* tokens[MAX_TOKENS]; 
    int numTokens = 0;
    // Set up a flag for the unzip operation.
    bool unzip = 0; 
    // Divide the message into tokens using space and newline characters as the delimiters
    char* token = strtok(tempMessage, " \n");
    //Keep tokenizing the message while there are additional tokens and the maximum token limit hasn't been reached.
    while (token != NULL && numTokens < MAX_TOKENS) 
    {
        // Verify if the current token is "-u".
        if (strcmp(token, "-u"
        ) == 0) 
        {
            // If the current token is "-u", update the 'unzip' flag to indicate an unzip operation.
            unzip = true;
        }
        else 
        {
            //If the token is not "-u," then add it to the 'tokens' array and increase the count of tokens.
            tokens[numTokens++] = token;
        }
        // Proceed to the following token in the message.
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

   else if (numTokens <= 4 && strcmp(tokens[0], "tarfgetz") == 0) {
        int size1 = atoi(tokens[1]);
        int size2 = atoi(tokens[2]);
        tarfgetz(size1, size2, unzip, sd);
    }
    else if (numTokens <= 4 && strcmp(tokens[0], "getdirf") == 0) 
    {
      // If there are at most 4 tokens and the command is "getdirf"
        // Extract the date parameters from tokens[1] and tokens[2]   
        char* date1 = tokens[1];
        char* date2 = tokens[2];
         //check if no dates are provided then display an error message
         if((tokens[1]==NULL && tokens[2]==NULL) ||(tokens[1]!=NULL && tokens[2]==NULL))
        write(sd, "Please provide date1 and date2", strlen("Please provide date1 and date2"));
        else
        // Call the 'getdirf' function with the extracted date parameters, the unzip flag, and the socket descriptor (sd)
        getdirf(date1, date2, unzip, sd);
    }
    //validate teh command is fgets
    else if (strcmp(tokens[0], "fgets") == 0) 
    {
         // Declre an aray to stre fie naes (maximm f  iles)
    char* files[] = { NULL };
    // Initalize  unt of fles
    int numFiles = 0;
    int i;
    // Initialie a flag to indicate whether to proceed with the command
    bool proceed = true; 
    // Iterae thrugh the toens stating frm the seond tokn (indx 1)
    for (i = 1; i < numTokens; i++) 
    {
        // Chec if the maxium nuber of fies (4) is exeeded
        if (numFiles >= 4) 
        {
            // Sed n eror mesage t theclient inicating th fil limt is exeeded
            write(sd, "File limit exceeded. Maximum 4 files are allowed.", strlen("File limit exceeded. Maximum 4 files are allowed."));
             // Set the 'proceed' flag to indicate not to proceed with the command
            proceed = false; 
             // Break out of the loop when the file limit is exceeded
            break; 
        }
        // Check if the current token is the "-u" flag
        if (strcmp(tokens[i], "-u") == 0) 
        {
             // Skip the "-u" flag and continue to the next token
            continue;
        }
        // Store the file name in the 'files' array and increment the count of files
        files[numFiles++] = tokens[i];
    }
    if (proceed && numFiles > 0) 
    {
    // methos-impllementation begins
    DIR* dir;
    struct dirent* ent;
    int file_count = 0;
    char* file_list[1000];
    // Iterate through the list of files provided by the client
    for (int i = 0; i < numFiles; i++) 
    {
        // Open the current directory for reading
            if ((dir = opendir(".")) != NULL) 
            {
                // Read directory entries
            while ((ent = readdir(dir)) != NULL) 
            {
                  // Check if the name of the current directory entry matches the client-provided file name
                if (strcmp(ent->d_name, files[i]) == 0) 
                {
                    // If a match is found, add the file's name to the 'file_list' array and increment the file count
                    file_list[file_count] = strdup(ent->d_name);
                    file_count++;
                }
            }
             // Close the directory after reading
            closedir(dir);
        }
        else 
        {
           //failed i opennnd
            perror("");
            exit(EXIT_FAILURE);
        }
    }
    // validate if any files were found during the search
    if (file_count == 0) 
    {
         // If no files were found, send a "file not found" message to the client
        write(sd, fileNotFound, sizeof(fileNotFound));
        // Output a message indicating that no files were found.
        printf("The requested files were not found\n");
    }
    else
     {
        // If files were there, send a message indicating their presence.
        char files_found[] = "The following files with the specified extensions were found:";
        write(sd, files_found, sizeof(files_found));
        // Print a message indicating that files were found
        printf("The requested files were found:\n");
        // Send each file name in the 'file_list' to the client
        for (int i = 0; i < file_count; i++) 
        {
           //Send the file name to the client using the socket descriptor.
            write(sd, file_list[i], sizeof(file_list[i]));
        }
    }
    // Chek if iles wre fund durng te seach
    if (file_count > 0) 
    {
         // If fils ere found copress tem nd end he ompressed ilesto he cient
        compress_files(file_list, file_count);
        // suming 'send_file' funcion send the cmpressed filesto the clent
        send_file(sd, unzip);
    }
    
}
 else if (proceed) 
{
    // If no files were provided by the client (and the 'proceed' flag is still true),
    // send a "No files provided" message to the client
    write(sd, "No files provided.", strlen("No files provided."));
}

}
// verify if the command is "targzf"
else if (strcmp(tokens[0], "targzf") == 0) 
{
    // Create an array for storing extensions (with a maximum of four extensions).
    char* extensions[] = { NULL };
    // Initialize the count of extensions
    int numExtensions = 0;
    int i;
    // setup a flag to indicate whether to proceed with the command
    bool proceed = true; 
    // Iterate through the tokens beginning from the second token (index 1),
    // and terminate if the maximum limit on the number of extensions (4) is reached.
    for (i = 1; i < numTokens && numExtensions <= 4; i++) 
    {   
    // Check if the limit on the number of extensions (4) is exceeded
    if (numExtensions>= 4) 
    {
        // Send an error message to the client indicating the extensions limit is exceeded
            write(sd, "Extensions limit exceeded. Maximum 4 extensions are allowed.", strlen("Extensions limit exceeded. Maximum 4 extensions are allowed."));
            // Set the 'proceed' flag to indicate not to proceed with the command
            proceed = false; // Set the flag to indicate not to proceed
            // Break out of the loop when the limit is exceeded
            break; // Break out of the loop when the limit is exceeded
    }
    // Check if the current token is the "-u" flag
    if (strcmp(tokens[i], "-u") == 0) 
    {
       // Skip the "-u" flag and continue to the next token
        continue;
    }
     // Store the extension in the 'extensions' array and increment the count of extensions
    extensions[numExtensions++] = tokens[i];
 }
  // If the 'proceed' flag is true and there are valid extensions
if (proceed && numExtensions > 0) 
{
    // Call the 'targzf' function with the extracted extensions, the unzip flag, and the socket descriptor (sd)
     targzf(extensions, numExtensions, unzip, sd);
} 
else if (proceed) 
{
     // If no valid extensions were provided, send a "No extensions provided" message to the client
        write(sd, "No extensions provided.", strlen("No extensions provided."));
}
}

else 
{
     // If the command is not recognized, send an error message to the client
        write(sd, errorMessage, strlen(errorMessage));
}
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// The filesrch function searches for a specified file in the given directory (rootDir) 
//and sends file information or a "file not found" message to the client via the socket descriptor (sd). It handles errors during the process.
void filesrch(char* file, int sd) 
{
    // Construct the full path for the directory
   char dirName[255];
    strcpy(dirName, rootDir);
    strcat(dirName, "/");
     // Open the directory
    DIR* dp;
    dp = opendir(dirName);
    if (dp == NULL) 
    {
         // If the directory cannot be opened, print an error message and return
        printf("Not a valid directory\n");
        return;
    }
    // define structures for directory entries and file status
    struct dirent* dirp;
    struct stat filestat;
    //indicating flag not foinf
    bool isFound = false;
    // Iterate through directory entries
    while ((dirp = readdir(dp)) != NULL) 
    {
          // Skip "." and ".." entries
       if (strcmp(dirp->d_name, ".") == 0 && strcmp(dirp->d_name, "..") == 0) 
       {
            continue;
        }
        // Build the full path for the current entry
        strcat(dirName, dirp->d_name);
        // geting the-status ofthe-file
        if (stat(dirName, &filestat) < 0) 
        {
            // If there's an error getting the file status, print an error message and return
            printf("Error due to file status\n");
            return;
        }
        // Check if the entry is a directory
        //if it isi skop
        if (S_ISDIR(filestat.st_mode)) 
        {
            // skip the directory
        }
        else if (strcmp(dirp->d_name, file) == 0) 
        {
            // If the current entry matches the requested file
            // Generate information about the file
            char file_info[1000];
            sprintf(file_info, "Found file %s\n  Size: %ld bytes\n  Created: %s", file, filestat.st_size, ctime(&filestat.st_ctime));
            // Send the file information to the client
            write(sd, file_info, strlen(file_info));
             // Set the 'isFound' flag to true
            isFound = true;
            // Exit the loop since we found the file
            break;
        }
         // Reset the directory path for the next iteration
        strcpy(dirName, rootDir);
        strcat(dirName, "/");
    }
     // If the file is not found, send a message to the client
    if (!isFound)
    {
        // displaying the response to-the-client
        printf("'%s' The requested file not found\n", file);
        write(sd, fileNotFound, sizeof(fileNotFound));
    }
    //closing the-directry
    closedir(dp);
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// The tarfgetz function checks size constraints, searches for files in the current directory within a specified size range, 
//and sends the results (file names) to the client through the socket descriptor (sd). It handles various error scenarios and optionally 
//compresses the found files when the unzip flag is set.
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
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// The getdirf function searches for files with modification dates within a specified range in the current directory and communicates the file names to the 
//client (using socket descriptor sd), handling missing or invalid dates and errors, with the option to compress found files when the unzip flag is set.
void getdirf(char* date1, char* date2, bool unzip, int sd) 
{
    // Pointer to directory structure
    DIR* dir;
    // Pointer to directory entry
    struct dirent* ent;
    // Structure to hold file status
    struct stat st;
     // Buffer to hold the path of each entry
    char path[1000];
    // Count of files found within the specified date range
    int file_count = 0;
    // Array to store the names of files found
    char* file_list[1000];
    // Parse date strings into time_t values
    // Initialize a struct for the first date
    struct tm date1_tm = { 0 };
    // Initialize a struct for the second date
    struct tm date2_tm = { 0 };
    // Parse the first date string (date1) using the format "%Y-%m-%d" and store it in date1_tm
    strptime(date1, "%Y-%m-%d", &date1_tm);
    // Parse the second date string (date2) using the format "%Y-%m-%d" and store it in date2_tm
    strptime(date2, "%Y-%m-%d", &date2_tm);
    // Convert the parsed date1_tm into a time_t value
    time_t date1_t = mktime(&date1_tm);
    // Convert the parsed date2_tm into a time_t value
    time_t date2_t = mktime(&date2_tm);
    // Check if date1 is greater than or equal to date2
    if (date1_t >= date2_t) 
    {
        // Provide an appropriate response to the client indicating that date1 must be less than date2
        char invalidDates[] = "Invalid command. date1 must be less than date2.";
        write(sd, invalidDates, sizeof(invalidDates));
        //printf("Invalid command. date1 must be less than date2.\n");
        return;
    }
    // Attempt to open the current directory
    if ((dir = opendir(".")) != NULL) 
    {
        // Loop through directory entries
        while ((ent = readdir(dir)) != NULL) 
        {
            // Skip "." and ".." entries
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) 
            {
                continue;
            }
             // Get file status
            if (lstat(ent->d_name, &st) == -1) 
            {
                 // Skip the current entry if there's an error getting the file status
                continue;
            }
            // Check if the current entry is a regular file
            if (!S_ISDIR(st.st_mode)) 
            {
              // Get the modification time of the file
                time_t file_time = st.st_mtime;
                // Check if the file's modification time is within the specified date range
                if (file_time >= date1_t && file_time <= date2_t) 
                {
                    // If the file's modification time is within the date range,
                    file_list[file_count] = strdup(ent->d_name);
                    // Increment the count of files in the list
                    file_count++;
                }
            }
        }
        // Close the directory
        closedir(dir);
    }
    else 
    {
   // Failed to open directory
   // Print an error message
        perror("");
        // Exit the program with a failure status
        exit(EXIT_FAILURE);
    }
     // Check if no files were found within the specified date range
    if (file_count == 0) 
    {
        // Send a "file not found" message to the client
        write(sd, fileNotFound, sizeof(fileNotFound));
        printf("None of the files were found.\n");
    }
    else 
    {
        // Files were found, send a message to the client with the list of found files
        char files_found[] = "The following files with the specified dates were found:";
        // Send the message to the client
        write(sd, files_found, sizeof(files_found));
        // Output the message to the server's console
        printf("The following files were found:\n");
        // Iterate through the list of found files
        for (int i = 0; i < file_count; i++) {
           // Send the file name to the client through the socket descriptor
            write(sd, file_list[i], sizeof(file_list[i]));
        }
    }
    // If files were found and the 'unzip' flag is set, compress the files and send them to the client
    if (file_count > 0) 
    {
        // Compress the files
        compress_files(file_list, file_count);
        // Send the compressed files to the client
        send_file(sd, unzip);
    }
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//This targzf function searches for files with specified extensions in the current directory and communicates the file names to the client (using socket descriptor sd), 
//handling cases where no files match the extensions or when errors occur during directory access. It also has the option to 
//compress the found files when the unzip flag is set.
void targzf(char* extensions[], int numFiles, bool unzip, int sd) 
{
    // Pointer to directory structure
    DIR* dir;
    // Pointer to directory entry
    struct dirent* ent;
    // Count of files found with specified extensions
    int file_count = 0;
    // Array to store the names of files found
    char* file_list[1000];
    // Attempt to open the current directory
    if ((dir = opendir(".")) != NULL) 
    {
        // Loop through directory entries
        while ((ent = readdir(dir)) != NULL) 
        {
            // Check each file for a matching extension
            for (int i = 0; i < numFiles; i++) 
            {
                // Get the current file extension
                const char* extension = extensions[i];
                // Calculate the length of the current extension and the length of the file name
                size_t extension_length = strlen(extension);
                size_t name_length = strlen(ent->d_name);
                // Check if the file name's length is greater than the extension's length,
            // and if the file name's extension matches the specified extension
                if (name_length > extension_length && strcmp(ent->d_name + name_length - extension_length, extension) == 0) 
                {
                    // Store the name of the file in the list
                    file_list[file_count] = strdup(ent->d_name);
                     // Increment the count of files in the list
                    file_count++;
                    // Move to the next directory entry (skip further extension comparisons)
                    break;
                }
            }
        }
        // Close the directory
        closedir(dir);
    }
    else 
    {
       // Failed to open the directory
       // Print an error message
        perror("");
        // Exit the program with a failure status
        exit(EXIT_FAILURE);
    }
    // Check if no files were found with the specified extensions
    if (file_count == 0) 
    {
         // No files with the specified extensions were found
        // Send a message to the client indicating no files were found
        char none_extensions[] = "None of the files with the specified extensions were found.\n";
         // Write the message to the client through the socket descriptor
        write(sd, none_extensions, sizeof(none_extensions));
         // Output a corresponding message to the server's console
        printf("None of the files with the specified extensions were found.\n");
    }
    else 
    {
        // Files with the specified extensions were found
        // Send a message to the client with the list of found files
        char extensions_found[] = "The following files with the specified extensions were found:";
        // Write the message to the client through the socket descriptor
        write(sd, extensions_found, sizeof(extensions_found));
        // Output the list of found files to the server's console
        printf("The following files with the specified extensions were found:\n");
        // Iterate through the list of found files and send each file name to the client
        for (int i = 0; i < file_count; i++) 
        {
           // printf("%s\n", file_list[i]);
           // Send the file name to the client through the socket descriptor
            write(sd, file_list[i], sizeof(file_list[i]));
        }
    }
    // If files were found and the 'unzip' flag is set, compress the files and send them to the client
    if (file_count > 0) 
    {
         // Compress the found files
        compress_files(file_list, file_count);
         // Send the compressed files to the client
        send_file(sd, unzip);
    }
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Function to send an acknowledgment message over the specified socket descriptor (sd).
void sendAck(int sd) 
{
    // Write the acknowledgment message to the socket.
    write(sd, ack, sizeof(ack));
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//Function to receive and validate an acknowledgment message from the specified socket descriptor (sd).
bool getAck(int sd) 
{
    // Buffer to store the received acknowledgment message.
    char ackMsg[sizeof(ack)];
    // Read the acknowledgment message from the socket.
    read(sd, ackMsg, sizeof(ackMsg));
    // Compare the received acknowledgment message with the expected acknowledgment (ack).
    if (strcmp(ack, ackMsg) == 0)
    // Return true if the received acknowledgment matches the expected acknowledgment.
        return true;
    else
    // Return false if the received acknowledgment does not match the expected acknowledgment.
        return false;
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//The main function establishes a TCP server that listens for client connections on a specified port. 
//When a client connects, it sends the count of connected clients and forks child processes to handle each client. 
//The getConnectedClient function is expected to manage client interactions.
int main(int argc, char* argv[]) 
{
    // Counter for the number of clients connected to server 1
    int countServer1 = 0;
     // Socket descriptors
    int sd1, client;
    // Socket address structure
    struct sockaddr_in socAddr;
     // Socket option value
    int opt = 1;
    // Length of the socket address
    int addrlen = sizeof(socAddr);
    //// Create a socket for the server to communicate with clients
    if ((sd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        //// Check if the socket creation failed
        fprintf(stderr, "Cannot create socket\n");
         // Exit the program with an error code (1)
        exit(1);
    }
    // Set socket options to allow reuse of the address and port
    if (setsockopt(sd1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) 
    {
        // Check if setting socket options failed
        perror("setsockopt");
        // Exit the program with an error code
        exit(EXIT_FAILURE);
    }
    // IPv4 address family
    socAddr.sin_family = AF_INET;
    // Use available local IP address
    socAddr.sin_addr.s_addr = INADDR_ANY;
    // Set the port using the specified constant PORT1
    socAddr.sin_port = htons(PORT1);
    // Bind the socket to the specified address and port
    if (bind(sd1, (struct sockaddr *)&socAddr, sizeof(socAddr)) < 0) 
    {
        // Check if binding the socket failed
        perror("bind failed");
        // Exit the program with an error code
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(sd1, MAX_CLIENTS) < 0) 
    {
         // Check if the listen operation failed
        perror("listen");
          // Exit the program with an error code
        exit(EXIT_FAILURE);
    }
     // Continuous loop to accept and handle incoming client connections
    while (1) 
    {
        // Accept a new client connection
        client = accept(sd1, (struct sockaddr *)NULL, NULL);
        countServer1 = countServer1 + 1;
        // Buffer to store the client count as a string
        char cs[2];
        // Convert the count to a string
        sprintf(cs, "%d", countServer1);
         // Send the client count to the client
        int valread = write(client, cs, 2);
        // Fork a child process to handle the connected client
        if (!fork()) 
        {
            //printf("Client %d is connected!\n", getpid());
            // Call a function (getConnectedClient) to handle the connected client
            getConnectedClient(client);
        }
        // Close the client socket in the parent process
        close(client);
    }
    // Exit the program with a success status
    return 0;
}
