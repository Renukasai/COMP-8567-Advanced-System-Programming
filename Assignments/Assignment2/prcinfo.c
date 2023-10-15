
#include <stdio.h>			
#include <stdlib.h>			
#include <unistd.h>			
#include <string.h>			
#include <dirent.h>			

#define MAX_PROCESSES 5			//Maximum number of processes
#define MAX_PID_LENGTH 10		//Maximum length of processid
#define MAX_DESCENDANT 100		//Maximum number of descendants
#define MAX_BUFFER_SIZE 256		//Maximum buffer size

int nondirect[MAX_DESCENDANT];			//Array to store nondirect descendants
int nondirectcn = 0;				//Counter for nondirect descendants

int directdescendant[MAX_DESCENDANT];		//Array to store direct descendants
int directdescendantcn = 0;			//Counter for direct descendants

int grandchildren[MAX_DESCENDANT];		//Array to store grandchildren
int grandchildrencn = 0;			//Counter for grandchildren

int defunct_directdescendant[MAX_DESCENDANT];	//Array to store defunct direct desendants
int defunct_directdescendantcn = 0;		//Counter for defunct direct descendants


//Function to check the status of a process whether defunct or nondefunct.
//If status is equal to 1 then defunct if status not equal to 1 then it is nondefunct
int checkstatus(char* process_id) {
    char path[20];
    int status = -1;

    // Construct the path to the "stat" file for the given process ID
    snprintf(path, sizeof(path), "/proc/%s/stat", process_id);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return status;  // Return the default status if file opening fails
    }

    char state;
    if (fscanf(file, "%*d %*s %c", &state) == 1) {
        status = (state == 'Z') ? 1 : 0;  // Set status based on process state
    }

    fclose(file);  // Close the file

    return status;
}


//Function to list the direct descendants of a process. 


void list_descendants(char* process_id, char* target_pid) {
    DIR* dir;
    struct dirent* ent;
    char path[20];

    dir = opendir("/proc");	//Open the Proc directory
    if (dir == NULL) {
        perror("opendir() error");
        return;
    }
//It loops to each entry in proc directory
   for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {

//Check if the entry is a directory and its name is a positive integer (PID)
    if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0) {
        strcpy(path, "/proc/");		//Copy "/proc/" to the path
        strcat(path, ent->d_name);	//Append PID to path
        strcat(path, "/stat");		//Append "/stat" to path

        FILE* file = fopen(path, "r");
        if (file != NULL) {
            char buf[256];
            if (fgets(buf, sizeof(buf), file) != NULL) {
                int pid;
                char comm[256], state;
                int ppid;

	//Parse the contents of the stat file to extract process information
                sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);
		
	//check if the parent processID matches the target PID
                if (ppid == atoi(target_pid)) { //comapares parent process id of the current process to target process id
                    directdescendant[directdescendantcn++] = pid;	//If match found then current process is direct descendant of parent process id and 
                                                                    //Add the PID to the descendants array
                }
            }
            fclose(file);					
        }
    }
}


    closedir(dir);						
}


// Function to list defunct direct descendants of a process.
void list_defunct_direct_descendants(char* process_id, char* target_pid, int show_defunct_descendants) {
    DIR* dir;                        
    struct dirent* ent;            
    char path[20];                   

    dir = opendir("/proc");          // Open the "/proc" directory
    if (dir == NULL) {
        perror("opendir() error");   
        return;
    }

    // Iterate over the directory entries
    for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0) {

            // Construct the path to the process's stat file
            strcpy(path, "/proc/");
            strcat(path, ent->d_name);
            strcat(path, "/stat");

            FILE* file = fopen(path, "r");  // Open the stat file
            if (file != NULL) {
                char buf[256];             // Buffer to read the file contents
                if (fgets(buf, sizeof(buf), file) != NULL) {
                    int pid;
                    char comm[256], state;
                    int ppid;

                    // Parse the contents of the stat file
                    sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);

                    if (ppid == atoi(target_pid)) { //comapares parent process id of the current process to target process id
                        // Check the status of the process
                        int status = checkstatus(ent->d_name);
                        if (status == 1) {
                            if (show_defunct_descendants) {
                                // Store the process ID in an array and print it
                                defunct_directdescendant[defunct_directdescendantcn++] = pid;
                               
                            }
                        }
                    }
                }
                fclose(file);            
            }
        }
    }

    closedir(dir);                     
}

// Function to list the grandchildren of a process
void list_grandchildren(char* process_id, char* target_pid) {
    DIR* dir;                       
    struct dirent* ent;            
    char path[20];                   

    dir = opendir("/proc");          
    if (dir == NULL) {
        perror("opendir() error");   
        return;
    }

    // Iterate over the directory entries
    for (ent= readdir(dir); ent != NULL; ent = readdir(dir)) {
        if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0) {

            // Construct the path to the process's stat file
            strcpy(path, "/proc/");
            strcat(path, ent->d_name);
            strcat(path, "/stat");

            FILE* file = fopen(path, "r");  // Open the stat file
            if (file != NULL) {
                char buf[256];             // Buffer to read the file contents
                if (fgets(buf, sizeof(buf), file) != NULL) {
                    int pid;
                    char comm[256], state;
                    int ppid;

                    // Parse the contents of the stat file
                    sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);
                //
                    if (ppid == atoi(process_id)) {//compares the current process parent id to target pid if it match it indicates that current process is direct
                                                    //descendant of target process id.
                        DIR* dir2;              // Directory pointer for the second iteration
                        struct dirent* ent2;  // Directory entry structure for the second iteration
                        char path2[20];         // Path buffer for the second iteration

                        dir2 = opendir("/proc");         // Open the "/proc" directory again
                        if (dir2 == NULL) {
                            perror("opendir() error");   // Error handling if directory opening fails
                            return;
                        }

                        // Iterate over the directory entries again
                        for (ent2 = readdir(dir2); ent2 != NULL; ent2 = readdir(dir2)) {
                            if (ent2->d_type == DT_DIR && atoi(ent2->d_name) > 0) {
                                // Construct the path to the second process's stat file
                                strcpy(path2, "/proc/");
                                strcat(path2, ent2->d_name);
                                strcat(path2, "/stat");

                                FILE* file2 = fopen(path2, "r");  // Open the second stat file
                                if (file2 != NULL) {
                                    char buf2[256];             // Buffer to read the second file contents
                                    if (fgets(buf2, sizeof(buf2), file2) != NULL) {
                                        int pid2;
                                        char comm2[256], state2;
                                        int ppid2;

                                        // Parse the contents of the second stat file
                                        sscanf(buf2, "%d %s %c %d", &pid2, comm2, &state2, &ppid2);

                                        if (ppid2 == pid) {	  //Check if the parent process ID of the second process matches the current process's PID
   								 // If true, it means that the second process is a direct child of the current process,
    								// making it a grandchild of the original process
                                            
					grandchildren[grandchildrencn++] = pid2;  // Store the grandchild PID
                                        }
                                    }
                                    fclose(file2);             // Close the second stat file
                                }
                            }
                        }

                        closedir(dir2);              // Close the second directory
                    }
                }
                fclose(file);             // Close the stat file
            }
        }
    }

    closedir(dir);                     // Close the main directory
}


// Function to find the siblings of a process.
void findSiblingProcesses(int processId, int defunctOnly) {
    char command[MAX_BUFFER_SIZE];   // Buffer to store the command
    snprintf(command, sizeof(command), "pgrep -P $(ps -p %d -o ppid=)", processId);  // Combined 2 commands, ps -p -o ppid gives parent process id of the given process id.
                                                                                    //pgrep -P finds all processes with same parent process id

    FILE *fp = popen(command, "r");  // Execute the command and open a pipe to read the output
    if (fp == NULL) {
        printf("Error executing command.\n");  
        return;
    }

    char buffer[MAX_BUFFER_SIZE];   // Buffer to read the output
    for (; fgets(buffer, sizeof(buffer), fp) != NULL; ) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove the trailing newline character from the buffer
        int siblingPid = atoi(buffer);  	// Convert the buffer content to an integer representing the sibling PID

        // Exclude the process ID itself
        if (siblingPid != processId) {  //Sibling process id is checked against the target process id to exclude the target process id from printing.
            if (defunctOnly) {
                int status = checkstatus(buffer);  // Check the process status
                if (status == 1) {
                    printf("%d\n", siblingPid);  // Print the sibling PID if it is defunct
                }
            } else {
                printf("%d\n", siblingPid);  // Print the sibling PID
            }
        }
    }

    pclose(fp);  // Close the pipe and terminate the command execution
}

// Function to list the non-direct descendants of a process
void list_non_direct_descendants(char* process_id, char* target_pid) {
    DIR* dir;  
    struct dirent* ent;  
    char path[20];  
    dir = opendir("/proc");  
    if (dir == NULL) {
        perror("opendir() error");  
        return;
    }

    // Iterate over each entry in the "/proc" directory
    while ((ent = readdir(dir)) != NULL) {
        // Check if the entry is a directory and has a numeric name greater than zero
        if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0) {
            // Construct the path to the "stat" file for the current process
            strcpy(path, "/proc/");
            strcat(path, ent->d_name);
            strcat(path, "/stat");

            FILE* file = fopen(path, "r");  // Open the "stat" file
            if (file != NULL) {
                char buf[256];  // Buffer to read the contents of the "stat" file

                // Read the contents of the "stat" file into the buffer
                if (fgets(buf, sizeof(buf), file) != NULL) {
                    int pid;
                    char comm[256], state;
                    int ppid;

                    // Extract the PID, command name, state, and PPID from the buffer
                    sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);

                    // Check if the current process is not the target PID or its direct child, if condition satisfies 
                    //then the current process id is non dircet descendant of target pid.
                    if (ppid != atoi(target_pid) && pid != atoi(target_pid)) {

                        // to verify the current process is nondirect descendant
                        //set a flag to 0 and initialize a variable current pid with ppid.
                        int isNonDirectDescendant = 0;
                        int currentPid = ppid;

                        //loop Traverse the ancestor hierarchy of the current process, continues until the current pid becomes 0 or non direct descendant.
                        for (; currentPid != 0; ) {
                            if (currentPid == atoi(target_pid)) {
                                isNonDirectDescendant = 1;
                                break;
                            }

                            // Open and read the stat file of the current process's ancestor
                            char ancestor_path[20];
                            snprintf(ancestor_path, sizeof(ancestor_path), "/proc/%d/stat", currentPid);

                            FILE* ancestor_file = fopen(ancestor_path, "r");
                            if (ancestor_file != NULL) {
                                char ancestor_buf[256];  // Buffer for ancestor's stat file contents
                                if (fgets(ancestor_buf, sizeof(ancestor_buf), ancestor_file) != NULL) {
                                    int ancestor_ppid;
                                    sscanf(ancestor_buf, "%*d %*s %*c %d", &ancestor_ppid);

                                    currentPid = ancestor_ppid;
                                }
                                fclose(ancestor_file);
                            } else {
                                break;
                            }
                        }

                        if (isNonDirectDescendant) {
                            // The current process is a non-direct descendant
                            nondirect[nondirectcn++] = pid;
                        }
                    }
                }
                fclose(file);
            }
        }
    }

    closedir(dir);  
}

//Function to print the process information of process, retrieves information using sscanf by traversing through proc directory.
void print_process_info(char* process_id, int show_descendants, char* target_pid, int show_siblings, int show_grandchildren, int show_non_direct_descendants, int show_defunct_descendants) {
    char path[20];
    snprintf(path, sizeof(path), "/proc/%s/stat", process_id);  // Construct the path to the "stat" file for the given process ID

    FILE* file = fopen(path, "r");  // Open the "stat" file
    if (file != NULL) {
        char buf[256];  // Buffer to read the contents of the "stat" file

        // Read the contents of the "stat" file into the buffer
        if (fgets(buf, sizeof(buf), file) != NULL) {
            int pid;
            char comm[256], state;
            int ppid;

            sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);

            // Check if the flag is set to show descendants and the process ID matches the target PID
            if (show_descendants && strcmp(process_id, target_pid) == 0)
                list_descendants(process_id, target_pid);  // Call a function to list the descendants of the process

            // Check if the flag is set to show grandchildren and the process ID matches the target PID
            if (show_grandchildren && strcmp(process_id, target_pid) == 0)
                list_grandchildren(process_id, target_pid);  // Call a function to list the grandchildren of the process

            // Check if the flag is set to show non-direct descendants and the process ID matches the target PID
            if (show_non_direct_descendants && strcmp(process_id, target_pid) == 0)
                list_non_direct_descendants(process_id, target_pid);  // Call a function to list the non-direct descendants of the process

            // Check if the flag is set to show defunct descendants and the process ID matches the target PID
            if (show_defunct_descendants && strcmp(process_id, target_pid) == 0)
                list_defunct_direct_descendants(process_id, target_pid, show_defunct_descendants);  // Call a function to list the defunct descendants of 		the process

            printf("%d %d\n", pid, ppid);  // Print the PID and PPID of the process
        }
        fclose(file);  
    } else {
        printf("Process with PID %s not found.\n", process_id);  // Print an error message if the process is not found
        return;
    }
}



int main(int argc, char* argv[]) {
    // Check the number of command-line arguments. Arguments should not be less than 3.
    if (argc < 3 || argc > MAX_PROCESSES + 2) {
        printf("Usage: ./prcinfo [root_process] [process_id1] [process_id2]... [process_id(n)] [OPTION]\n");
        return 1;
    }

    char* root_process = argv[1];  // Get the root process ID from command-line arguments

    // Initialize flags for various options
    int show_descendants = 0;
    int show_siblings = 0;
    int show_grandchildren = 0;
    int show_status = 0;
    int show_non_direct_descendants = 0;
    int show_defunct_siblings = 0;
    int show_defunct_descendants = 0;

    // Process the last command-line argument to set the appropriate option flags
    if (argc > 2 && strcmp(argv[argc - 1], "-dd") == 0) {
        show_descendants = 1;
        argc--;		//last argument option should be ignored
    } else if (argc > 2 && strcmp(argv[argc - 1], "-sb") == 0) {
        show_siblings = 1;
        argc--;
    } else if (argc > 2 && strcmp(argv[argc - 1], "-gc") == 0) {
        show_grandchildren = 1;
        argc--;
    } else if (argc > 2 && strcmp(argv[argc - 1], "-zz") == 0) {
        show_status = 1;
        argc--;
    } else if (argc > 2 && strcmp(argv[argc - 1], "-nd") == 0) {
        show_non_direct_descendants = 1;
        argc--;
    } else if (argc > 2 && strcmp(argv[argc - 1], "-sz") == 0) {
        show_defunct_siblings = 1;
        show_siblings = 1;
        argc--;
    } else if (argc > 2 && strcmp(argv[argc - 1], "-zc") == 0) {
        show_defunct_descendants = 1;
        argc--;
    }

    // Iterate over the command-line arguments starting from index 2 (skip the root process)
    for (int i = 2; i < argc; i++) {
        // Call the function to print process information based on the options
        print_process_info(argv[i], show_descendants, argv[2], show_siblings, show_grandchildren, show_non_direct_descendants, show_defunct_descendants);
        printf("\n");
    }

    // Print grandchildren if the option is enabled
    if (argc > 2 && show_grandchildren) {
        for (int i = 0; i < grandchildrencn; i++) {
            printf("%d\n", grandchildren[i]);
        }
    }

    // Print descendants if the option is enabled
    if (argc > 2 && show_descendants) {
        for (int i = 0; i < directdescendantcn; i++) {
            printf("%d\n", directdescendant[i]);
        }
    }

    // Print defunct descendants if the option is enabled
    if (argc > 2 && show_defunct_descendants) {
        for (int i = 0; i < defunct_directdescendantcn; i++) {
            printf("%d\n", defunct_directdescendant[i]);
        }
    }

    // Print non-direct descendants if the option is enabled
    if (argc > 2 && show_non_direct_descendants) {
        for (int i = 0; i < nondirectcn; i++) {
            printf("%d\n", nondirect[i]);
        }
    }

    // Print siblings if the option is enabled
    if (argc > 2 && show_siblings) {
        if (show_defunct_siblings) {
            findSiblingProcesses(atoi(argv[2]), 1);  // Print only defunct siblings
        } else {
            findSiblingProcesses(atoi(argv[2]), 0);  // Print all siblings
        }
    }

    // Check process status if the option is enabled
  if (argc > 2 && show_status) {
    int status = checkstatus(argv[2]);
    const char* status_message = (status == 1) ? "DEFUNCT" : (status == 0) ? "NOT DEFUNCT" : "Error retrieving process status.";
    printf("\n%s\n", status_message);
}


    return 0;
}
