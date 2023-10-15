//Name1- Sai Renuka Telu
//StudentNO - 110096549
//Name2 - Kavya Seeramsetty
//StuentNo - 110099222
//section - I



#include <sys/socket.h>         //socket programming and network operations
#include <netinet/in.h>         //Internet addressing structures  and constants
#include <arpa/inet.h>          //Functions for IP address conversion
#include <stdio.h>              //Standard input or output functions
#include <stdlib.h>             //Memory allocation 
#include <unistd.h>             //POSIX API access
#include <errno.h>              //Error number values and handling
#include <string.h>             //String Manipulation Functions
#include <dirent.h>             //Directoty manipulation functions
#include <sys/types.h>          //basic system data types
#include <signal.h>             //signal handling functions and constants
#include <stdbool.h>            //boolean data types and values
#include <sys/wait.h>           //process related wait functions
#include <fcntl.h>              //File control operations 

 
#define PORT1 3160        //Pre-defined constant for a specific port number(PORT1)
#define PORT2 3252          //Pre-defined constant for a specific port number(PORT2)

//Global initializations of 2 variables to keep the tracking of server load.
int countServer1=0;              // Keeps track of load or count for Server 1
int countServer2=0;              // Keeps track of load or count for Server 2(mirror)

//Function declerations
void downloadFile(int sock);
bool getAck(int sock);
void sendAck(int sock);
void func(int sock);

//Messages
char foundMsg[] = "File Found!";
char downloadMsg[] = "Downloading Files";

char getAction[] = "-1";
char unzipAction[] = "-3";
char fileFoundAction[] = "@#$%";
char fileNotFoundAction[] = "-2";
char ack[] = "-ack";
char exitMsg[] = "quit\n";

//The unzipFile function automates the process of extracting a compressed .tar.gz file using the tar command-line utility. It constructs the appropriate command string and uses the system function to execute it. This function can be useful when dealing with compressed archives in programs that need to extract files as part of their functionality.
void unzipFile(){
    //define the name of the compressed file to be extracted.
    char file_name[]="temp.tar.gz";
    // Generate a buffer to store the shell command used for the extraction process
    char command[1000];
    // Construct the shell command to extract the file using 'tar' command. 
    //-xzf: Flags for extracting, handling gzip compression. 
    //%s: Placeholder for the file name to be extracted
    sprintf(command, "tar -xzf %s", file_name);
    // Trigger the system call to execute the constructed command.
    system(command);
}

//This function manages server communication using separate processes: one for receiving server responses and another for user input. It forks into parent and child processes, with the parent reading server messages, possibly invoking actions like file downloads or extractions. The child reads user input, transmitting it to the server, and exits gracefully upon exit command or error.
void func(int sock) {
    //Declaring two integer variables n and pid to store information relevant to data transmission and process management within the scope of the code. "n"  variable is used for storing the number of bytes read or written during data transmission and "pid" is used for storing the process ID (PID) of a newly created process.
    int n, pid;
//initializes a character pointer message with a null value, indicating it's not pointing to any memory location yet.
    char *message = NULL; //this is a dynamically allocated buffer.

// The variable buffer_size is declared to denote the size of a data storage buffer, which is initialized to 1024 bytes.
    size_t buffer_size = 1024; 
//Fork the process to create two seperate execution paths.
    pid = fork();
// This section executes within the parent process (when pid is a non-zero value)
    if (pid) {

//In this while block it continuously allocates memory for the message buffer, ensuring that there's enough space to read data from the server. If memory allocation fails, it prints an error and exits the program. This process repeats indefinitely to read and process data from the server.
        while (1) { //Enters an infinite loop 
            // Allocate memory before reading from server
            message = (char *)malloc(buffer_size);
    //checks if the memory allocation for the message buffer was unsuccessful. If message is still NULL, it means memory allocation failed.
            if (!message) {
// If memory allocation failed, perror("malloc failed") prints an error message describing the failure to the standard error stream.
                perror("malloc failed");
//exit(EXIT_FAILURE) terminates the program with a failure status code to indicate that something critical went wrong
                exit(EXIT_FAILURE);
            }
//if block within the while loop processes received server data: converting it into a string, performing actions like file downloads or extractions based on message content, and displaying server responses or errors. It ensures dynamic handling of server interactions in a continuous loop.
            if (n = read(sock, message, buffer_size - 1)) {
                message[n] = '\0';
    //check if the recieved message indicates an action 
                if (strcmp(message, getAction) == 0)
                    downloadFile(sock);// trigger the download action
                else if (strcmp(message, unzipAction) == 0)
                    unzipFile();//trigger the file extraction action

    //The else part handles the case where the received message is not associated with predefined actions. It prints the received server response to the standard error stream, indicating the server's message.
                else
                    fprintf(stderr, "Server: %s\n", message);

                free(message); // Free dynamically allocated memory
            }
        }
    }

//this section executes within the child process.
    if (!pid) {

//The while loop manages user input, continuously reading messages from the user and transmitting them to the server. It terminates if the user inputs the exit command (exitMsg) like quit. This loop ensures ongoing user interaction, transmits messages to the server, and handles the program's termination.
        while (1) {
            // Allocate memory before writing to server
            message = (char *)malloc(buffer_size);

//If memory allocation using malloc() fails (when message is NULL), it prints an error and terminates the program with a failure status code
            if (!message) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
//Read user input from standard output 
            if (n = read(0, message, buffer_size - 1)) {
                message[n] = '\0';
        //check if the user input is not just a new line character
                if (strcasecmp(message, "\n") != 0) {
        //write the user input to the server.
                    write(sock, message, strlen(message) + 1);
        //check if the command entered is exit message 
                    if (!strcasecmp(message, exitMsg)) {
                        printf("Bye Bye!!\n");
            //If it is quit command close the socket connection.
                        close(sock);
            //terminate the parent process to exit the program 
                        kill(getppid(), SIGTERM);
                        free(message); // Free dynamically allocated memory
                        exit(0);
                    }
                }

                free(message); // Free dynamically allocated memory
            }
        }
    }
}

//The downloadFile function manages the process of receiving a file from the server, including reading the filename and size, downloading the content, and saving it locally. It also handles acknowledgments with the server and provides status updates to the user.
void downloadFile(int sock) {

// n is declared to be used for tracking the number of bytes read from the server during data transmission or reception.
    int n;
//Buffer to hold the received filename. 
    char fileName[255];

    printf("Initiating file downloads!!!\n"); //Printing message on console as files are being downloading
    sendAck(sock);                            //sending acknowledgement to server.

    if (n = read(sock, fileName, 255)) {      // Read the filename from the server into the 'fileName' buffer
        fileName[n] = '\0';                   // Null-terminate the received filename
    }
    printf("FileName: %s\n", fileName);      //Print the received filename on console.
    sendAck(sock);                           //sending acknowledgement to server.

// Open the local file for writing with specified permissions
    int fd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    long fileSize;      // Variable to hold the size of the incoming file
    read(sock, &fileSize, sizeof(long));    // Read the file size from the server
    printf("FileSize: %ld\n", fileSize);  // Print the size of the received file
    sendAck(sock);                        //sending acknowledgement to server.

    char* buffer = malloc(fileSize);  // Allocate memory for the file content
    read(sock, buffer, fileSize);  // Read the file content into the buffer
    write(fd, buffer, fileSize);  // Write the buffer content to the local file
    sendAck(sock);  // Send acknowledgment to server

    free(buffer);  // Free the dynamically allocated buffer
    close(fd);  // Close the local file

    printf(" '%s' The requested files have been downloaded!!!\n", fileName);    //Print a message on console if files requested are downloaded.
}

//The function sendAck is designed to transmit an acknowledgment message (ack) to a server using the provided socket sock. This acknowledgment helps establish successful communication and coordination between the client and server during interactions.
void sendAck(int sock) {
    write(sock, ack, sizeof(ack));
}

//The getAck function reads an acknowledgment message from the server using the given socket sock. It compares the received acknowledgment (ackMsg) with a predefined acknowledgment message (ack) and returns true if they match, indicating successful communication; otherwise, it returns false. This function helps confirm proper synchronization between the client and server during interactions.
bool getAck(int sock) {
    char ackMsg[sizeof(ack)];  // Buffer to hold the received acknowledgment message

    read(sock, ackMsg, sizeof(ackMsg));  // Read acknowledgment from the server
    if (strcmp(ack, ackMsg) == 0)  // Compare received acknowledgment with predefined acknowledgment
        return true;  // Return true if acknowledgments match (successful communication)
    else
        return false;  // Return false if acknowledgments do not match (communication issue)
}

//the main function initiates socket connections to a server and dynamically handles communication based on server counts, utilizing the function to manage interactions.
int main(int argc, char const* argv[])
{
//a meesage indicating that client has started.
printf("Running client\n");
//sock variable represents the main socket used for communication with the server. It is initialized to 0 as it establishes a default state for the socket variable before any specific assignment occurs in socket programming.
//valread will store the number of bytes read from the server.
//cleint_fd hold the file descriptor for the first client socket.
//sock2 represents a second socket, potentially used for alternative communication.
//client_fd2 holds the file descriptor for the second client socket.
int sock = 0, valread, client_fd,sock2,client_fd2;
//The variable command is declared as a character array with a capacity of 1024 bytes. This array is likely intended to hold commands or messages related to socket communication.
char command[1024];
//struct sockaddr_in serv_addr initializes a structure to store address information required for IPv4 socket communication.
struct sockaddr_in serv_addr;

//The if block is written to check whether the creation of a socket was successful. It assigns the result of the socket() function call to the sock variable. If the assignment is successful, it proceeds with a value greater than or equal to 0, indicating a valid socket descriptor. If the assignment is unsuccessful (returns a value less than 0), it indicates an error in socket creation, and an error message is printed. The return -1; statement terminates the function with an error status code, indicating the failure of socket creation.
if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //3 parameters(domain, connection, protocol(to run successfull by default 0))
{
printf("\n Socket creation error \n");
return -1;
}
// serv_addr.sin_family = AF_INET; assigns the address family AF_INET to the sin_family field of the serv_addr structure. This field specifies that the socket address structure is intended for IPv4 communication.
serv_addr.sin_family = AF_INET;

//The line serv_addr.sin_port = htons(PORT1); sets the port number of the serv_addr structure to the value of PORT1 after converting it to network byte order using the htons() function. This ensures correct representation of the port number for proper socket communication.
serv_addr.sin_port = htons(PORT1);

//This if checks if the conversion of the IP address in argv[1] to binary format using inet_pton() for the AF_INET address family (IPv4) is successful. If the conversion result is less than or equal to 0, indicating an error or unsupported address format, it prints an error message and returns -1 to signal the failure of address conversion. This helps ensure that only valid IP addresses are used for socket communication.
if (inet_pton(AF_INET, argv[1] , &serv_addr.sin_addr) <= 0) 
{
printf("\nInvalid address/ Address not supported \n");
return -1;
}

//The if statement verifies the success of a connection attempt to the server using the connect() function. It assigns the function's result to client_fd. If the assignment is negative, an error message is printed, and -1 is returned to indicate a failed connection attempt, ensuring proper error handling. 
//the connect() function establishes a connection from a client socket to a server socket, enabling communication between them.
if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))< 0) 
{
printf("\nConnection Failed \n");
return -1;
}

//The line char sc1[2]; declares a character array named sc1 with a capacity to hold 2 characters. It's likely intended to store a short response or message received from the server.
char sc1[2];
//reads data from the socket specified by sock into the array sc1. It attempts to read 2 bytes of data from the socket and assigns the number of bytes actually read to the variable valread. This is a common operation for receiving short messages or responses from the server.
valread=read(sock, sc1, 2);

//sscanf() function to parse the content of the sc1 array as an integer. It extracts the integer value from the array and stores it in the variable countServer1.
sscanf(sc1, "%d", &countServer1);

// the first six client conenctions are conencted to server using PORT1 and next 6 client conenctions are conencted to mirror using PORT2.
//The if condition checks if countServer1 meets specific criteria. If it's less than or equal to 6 or greater than 12 and odd, it manages communication, closes the socket, and returns 0 to indicate success.
if(countServer1<=6 || (countServer1>12 && countServer1%2==1))
{
printf("\ncountServer1:%d\n",countServer1); //prints the avlue of countServer1
func(sock);     //calls the function func() to manage communication with server
close(client_fd);   //closed the file descritor.
return 0;   //returns 0 on successfull completion.
}

//in else block a new socket sock2 is created using the socket() function. If the socket creation fails (returns a value less than 0), an error message is printed, and the function returns -1 to indicate the failure. This prepares for an alternative communication path when the initial conditions are not satisfied.
else{
if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
printf("\n Socket creation error \n");
return -1;
}
//Update port  and establish connection to the second server
serv_addr.sin_port = htons(PORT2);
int client_fd2;
if ((client_fd2 = connect(sock2, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))< 0) 
{
printf("\nConnection Failed \n");
return -1;
}
// Read server's response into 'sc2' and extract 'countServer2'
char sc2[2];
valread=read(sock2, sc2, 2);
sscanf(sc2, "%d", &countServer2);

//The if statement checks if countServer2 is greater than 12. If this condition is met, it assigns the value of countServer1 to countServer2. This ensures that countServer2 remains consistent with countServer1.
if(countServer2>12)
countServer2=countServer1;
printf("\ncountServer2:%d\n",countServer2);
func(sock2);// calling function func with socket2 to mannage communication with mirror
close(client_fd2);  //closing file descriptor 2.
return 0;   //return 0 if success.
}
}    
