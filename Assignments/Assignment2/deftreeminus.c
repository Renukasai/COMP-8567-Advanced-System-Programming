Sai Renuka telu
Section 1
Student ID: 110096549

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 256


//code defines a function kill_parent_process that recursively terminates all parent processes 
//starting from a given process ID. It uses pgrep to find parent processes and kill to terminate them.
void kill_parent_process(int processId) 
{
    char mode[256];
    snprintf(mode, sizeof(mode), "pgrep -P %d", processId);
    // Construct the command to find the parent process ID using the specified processId
    FILE* fp1 = popen(mode, "r");
    if (fp1 == NULL) {
        // if fp1 is null displaying corresponding error message
        printf("Error executing command.\n");
        return;
    }
    // Open a pipe to execute the command and read its output
    char out[256];
    while (fgets(out, sizeof(out), fp1) != NULL) 
    {
        // Read the output of the command line by line
       int parent_Pid = atoi(out);
        // Convert the output string to an integer, representing the parent process ID
        char mode2[256];
        snprintf(mode2, sizeof(mode2), "pgrep -P %d", parent_Pid);
        // Construct the command to find the parent process of the current parent process
        FILE* fp2 = popen(mode2, "r");
        if (fp2 == NULL) 
        {
            printf("Error executing command.\n");
            pclose(fp1);
            return;
        }
        // Open a new pipe to execute the command and read its output
        char out2[256];
        int defunct_Count = 0;
        while (fgets(out2, sizeof(out2), fp2) != NULL) 
        {
            // Read the output of the command line by line
            defunct_Count++;
            // Increment the defunct count for each line read
        }
        pclose(fp2);
        // Close the pipe
        if (defunct_Count > 0) 
        {
            printf("Terminating parent process: PID %d\n", parent_Pid);
            // Print a message indicating the termination of the parent process
            kill(parent_Pid, SIGKILL);
            // Terminate the parent process using the SIGKILL signal
            kill_parent_process(parent_Pid);
            // Recursively terminate the parent's parents
        }
    }
    pclose(fp1);
    // Close the initial pipe
}


//code defines a recursive function recursive_process_parent to terminate a process and its parent processes recursively. 
//It retrieves the parent process ID and finds the immediate child process by analyzing process information.
void recursive_process_parent(int processId)
{
    // Get the process ID of the parent process
    pid_t bash_Pid = getppid(); 
    // Initialize the immediate child process ID to -1
    pid_t immediate_Child_Pid = -1; 
    // Find the immediate child process
    // Execute the command to retrieve process information
    FILE* fp1 = popen("ps -eo ppid,pid,command", "r"); 
    if (fp1 != NULL)
    {
        char flag[256];
        // Skip the header line
        fgets(flag, sizeof(flag), fp1); 
        // Iterate through the output to find the immediate child process ID
        while (fgets(flag, sizeof(flag), fp1) != NULL)
        {
            pid_t pid, ppid;
            char command[256];

            // Read the next line of the output and extract the parent PID, child PID, and command
            sscanf(flag, "%d %d %[^\n]", &ppid, &pid, command);

            // Check if the parent PID matches the bash_Pid
            if (ppid == bash_Pid)
            {
                // Store the immediate child PID
                immediate_Child_Pid = pid; 
                // Exit the loop since immediate child has been found
                break; 
            }
        }
        pclose(fp1);
    }
    // Check if the process ID is the bash or root process
    if (processId == bash_Pid || processId == immediate_Child_Pid)
    {
        printf("It is a bash or a root, therefore not terminated\n");
        return;
    }
    char command[256];
    // Generate the command to find child processes
    snprintf(command, sizeof(command), "pgrep -P %d", processId); 
    // Execute the command to retrieve child processes
    FILE* fp2 = popen(command, "r"); 
    if (fp2 == NULL)
    {
        printf("Error while executing.\n");
        return;
    }
    // Counter for defunct processes
    int defunct_Count = 0; 
    char output1[256];
    while (fgets(output1, sizeof(output1), fp2) != NULL)
    {
        // Increment the defunct process count
        defunct_Count++; 
    }
    pclose(fp2);
    if (defunct_Count > 0)
    {
        printf("Terminating parent process: PID %d\n", processId);
        kill(processId, SIGKILL);
        // Recursively terminate the parent's parents
        kill_parent_process(processId); 
    }
    else
    {
        printf("There are no defuncts in the given process ID\n");
    }
}


// This code defines a function force_Terminate_Processes that terminates a parent process 
//if it has a number of child processes equal to or greater than a specified limit. 
//It retrieves process information using ps and pgrep commands
void force_Terminate_Processes(int parent_Pid, int limit)
{
    pid_t bash_Pid = getppid();
    pid_t immediate_Child_Pid = -1;
    // Get the process information (parent PID, child PID, command) using the "ps" command
    FILE* fp1 = popen("ps -eo ppid,pid,command", "r");
    if (fp1 != NULL)
    {
        char extend[256];
        fgets(extend, sizeof(extend), fp1); 
        // Iterate through the process information lines
        while (fgets(extend, sizeof(extend), fp1) != NULL)
        {
            pid_t pid, ppid;
            char command1[256];
            // Extract the parent PID, child PID, and command from the line
            sscanf(extend, "%d %d %[^\n]", &ppid, &pid, command1);
            // Check if the parent PID matches the bash_Pid
            if (ppid == bash_Pid)
            {
                 // Store the immediate child PID
                immediate_Child_Pid = pid; 
                // Exit the loop since immediate child has been found
                break; 
            }
        }
        pclose(fp1);
    }
    // Check if the parent process ID is the bash or immediate child process
    if (parent_Pid == bash_Pid || parent_Pid == immediate_Child_Pid)
    {
        printf("It is a bash or a root, therefore not terminated\n");
        return;
    }
    char command2[256];
    snprintf(command2, sizeof(command2), "pgrep -P %d", parent_Pid);
    // Get the child processes of the parent process using the "pgrep" command
    FILE* fp2 = popen(command2, "r");
    if (fp2 == NULL)
    {
        printf("Error while executing\n");
        return;
    }
    char output1[256];
    int child_Count = 0;
    // Iterate through the child process IDs
    while (fgets(output1, sizeof(output1), fp2) != NULL)
    {
        int child_Pid = atoi(output1);
        if (child_Pid != 0)
        {
            // Increment the child count if a valid child process ID is found
            child_Count++; 
        }
    }
    pclose(fp2);
    // Check if the child count meets the specified limit
    if (child_Count >= limit)
    {
        // Terminate the parent process
        kill(parent_Pid, SIGTERM); 
        printf("The Process %d is forcefully terminated.\n", parent_Pid);
    }
    else
    {
        printf("The Process %d does not meet the specified limit.\n", parent_Pid);
    }
}

//The defunct_limit function counts the number of defunct processes under a given parent process ID. 
//If the count is below a limit, it reports the number of zombie processes.
void defunct_limit(int parent_Pid, int limit) {
    char mode[MAX_LENGTH];
    snprintf(mode, sizeof(mode), "pgrep -P %d", parent_Pid);
    FILE* fp1 = popen(mode, "r");
    
    // Check if opening the command output file failed
    if (fp1 == NULL) {
        printf("Error while executing\n");
        return;
    }
    
    char end[MAX_LENGTH];
    int defunct_Count = 0;
    
    // Count the number of defunct processes by reading lines from the command output file
    for (; fgets(end, sizeof(end), fp1) != NULL; defunct_Count++);
    
    pclose(fp1);
    
    // If there are no defunct processes, print a message and return
    if (defunct_Count == 0) {
        printf("Processes are not forcefully terminated.\n");
        return;
    }
    
    // If the number of defunct processes is below the limit, print the count
    if (defunct_Count < limit) 
    {
        printf("The process only have %d zombie.\n", defunct_Count);
    } 
    else 
    {
        pid_t bash_Pid = getppid();
        pid_t immediate_Child_Pid = -1;
        FILE* fp2 = popen("ps -eo ppid,pid,command", "r");
        
        // Check if opening the process information file failed
        if (fp2 != NULL) 
        {
            char line1[256];
            fgets(line1, sizeof(line1), fp2); // Discard header line
            
            // Iterate through the process information and find the immediate child process ID
            while (fgets(line1, sizeof(line1), fp2) != NULL) 
            {
                pid_t pid, ppid;
                char command1[256];
                sscanf(line1, "%d %d %[^\n]", &ppid, &pid, command1);
                
                // Check if the parent PID matches the bash_Pid
                if (ppid == bash_Pid) 
                {
                    immediate_Child_Pid = pid;
                    break;
                }
            }
            
            pclose(fp2);
        }
        
        // Check if the parent PID is either the bash_Pid or the immediate child PID
        if (parent_Pid == bash_Pid || parent_Pid == immediate_Child_Pid) 
        {
            // No processes are forcefully terminated as it is a bash shell or an immediate child of bash
            //printf("No processes are forcefully terminated as it is a bash shell or an immediate child of bash.\n");
        } 
        else 
        {
            // Terminate the parent process
            printf("Terminating parent process: PID %d\n", parent_Pid);
            kill(parent_Pid, SIGKILL);
        }
    }
}


typedef struct {
    int pid;
    int elapsed_time_var;
} Process_Info;

//The get_defunct_processes function retrieves information about defunct child processes. 
//It executes commands to find child processes, retrieves their elapsed time, and stores the data in the defunct_Processes array. 
void get_defunct_processes(int parent_Pid, Process_Info** defunct_Processes, int* defunct_Count) {
    char command1[MAX_LENGTH]; // Buffer to store the command
    snprintf(command1, sizeof(command1), "pgrep -P %d", parent_Pid); // Generate the command to find child processes
    FILE* fp1 = popen(command1, "r"); // Execute the command and open the pipe for reading
    if (fp1 == NULL) {
        printf("Error executing command.\n"); // Display an error message if the command execution fails
        return;
    }
    char output1[MAX_LENGTH]; // Buffer to store the output of the command
    for (; fgets(output1, sizeof(output1), fp1) != NULL; ) {
        int child_Pid = atoi(output1); // Convert the child PID from string to integer
        if (child_Pid != 0) {
            snprintf(command1, sizeof(command1), "ps -o pid,etime= -p %d", child_Pid); // Generate the command to get process information
            FILE* child_Fp = popen(command1, "r"); // Execute the command for the child process
            if (child_Fp == NULL) {
                printf("Error executing command.\n"); // Display an error message if the command execution fails
                continue;
            }
            char child_Output[MAX_LENGTH]; // Buffer to store the output of the command
            fgets(child_Output, sizeof(child_Output), child_Fp); // Read the header line (discard it)
            fgets(child_Output, sizeof(child_Output), child_Fp); // Read the line containing process information
            int elapsed_Hours = 0, elapsed_Minutes = 0, elapsed_Seconds = 0; // Variables to store the elapsed time
            sscanf(child_Output, "%*d %d:%d:%d", &elapsed_Hours, &elapsed_Minutes, &elapsed_Seconds); // Extract the elapsed time values
            int total_Elapsed_Minutes = (elapsed_Hours * 60) + elapsed_Minutes; // Calculate the total elapsed minutes
            *defunct_Processes = (*defunct_Count == 0) ? malloc(sizeof(Process_Info)) : realloc(*defunct_Processes, (*defunct_Count + 1) * sizeof(Process_Info)); // Allocate or reallocate memory for the defunct processes array
            (*defunct_Processes)[*defunct_Count].pid = child_Pid; // Store the child PID in the defunct processes array
            (*defunct_Processes)[*defunct_Count].elapsed_time_var = total_Elapsed_Minutes; // Store the elapsed time in the defunct processes array
            (*defunct_Count)++; // Increment the defunct count
            pclose(child_Fp); // Close the pipe for the child process
        }
    }
    pclose(fp1); // Close the pipe for the parent process
}


//The process_forefull_termination function terminates the parent process if any of the defunct processes' elapsed time exceeds a given limit.
//It iterates through the defunct processes, checks their elapsed time, and terminates the parent process if necessary.
void process_forefull_termination(Process_Info* defunct_Processes, int defunct_Count, int limit) 
{
    pid_t parent_Pid = getppid(); // Get the parent process ID
    int i = 0; // Initialize loop counter

    while (i < defunct_Count) // Iterate through the defunct processes
    {
        if (defunct_Processes[i].elapsed_time_var > limit) // Check if elapsed time exceeds the limit
        {
            if (defunct_Processes[i].pid != parent_Pid) // Ensure the process is not the parent itself
            {
                kill(parent_Pid, SIGTERM); // Terminate the parent process
            }
        }

        i++; // Increment the loop counter
    }

    printf("Parent process %d forcefully terminated.\n", parent_Pid); // Print message indicating parent process termination
}



typedef struct {
    int apid;
    int appid;
    int adefuncts;
    int aexempted;
} aProcessInfo;

void forceTerminateProcess1(int process_Id) 
{
    // kill the required processid which should be forecefully terminated
    kill(process_Id, SIGTERM);
    printf("The Process %d is forcefully terminated.\n", process_Id);
}


//This code analyzes zombie processes and performs termination based on certain conditions.
//It retrieves process information using the ps command, tracks defunct processes, and terminates eligible processes if the termination threshold is met. 
//Exempted processes and special cases are considered during termination.
void parsing_Processes(int root_Pid, int termination_Threshold, int exempted_ProcessId) {
    // Get the PID of the bash shell and initialize immediate child PID
    pid_t bash_Pid = getppid();
    pid_t immediate_Child_Pid = -1;

    // Open a pipe to execute the 'ps' command and retrieve process information
    FILE *fp1 = popen("ps -eo pid,ppid,state", "r");
    if (fp1 == NULL) {
        printf("Error while executing\n");
        return;
    }

    char line1[MAX_LENGTH];
    fgets(line1, sizeof(line1), fp1); // Discard header line

    // Initialize variables to store process information
    aProcessInfo *processes = NULL;
    int process_Count = 0;
    int process_Capacity = 0;

    // Loop through each line of the output from 'ps' command
    for (; fgets(line1, sizeof(line1), fp1) != NULL;) {
        int pid, ppid;
        char state1[MAX_LENGTH];
        sscanf(line1, "%d %d %s", &pid, &ppid, state1);

        // Check if the process is a zombie (defunct state)
        if (strcmp(state1, "Z") == 0) {
            // Increase the capacity of the processes array if needed
            if (process_Count >= process_Capacity) {
                process_Capacity = (process_Capacity == 0) ? 1 : (2 * process_Capacity);
                processes = realloc(processes, process_Capacity * sizeof(aProcessInfo));
            }

            // Store process information in the array
            processes[process_Count].apid = pid;
            processes[process_Count].appid = ppid;
            processes[process_Count].adefuncts = 0;
            processes[process_Count].aexempted = (pid == exempted_ProcessId) ? 1 : 0;
            process_Count++;
        }
    }

    pclose(fp1); // Close the pipe

    int defunct_Count = 0;

    // Process the collected zombie processes
    for (int i = 0; i < process_Count; i++) {
        printf("Terminating Parent process: PID %d\n", processes[i].appid);

        // Check if the process is exempted from termination
        if (processes[i].aexempted) {
            printf("Not terminating Parent process: PID %d\n", processes[i].apid);
            continue;
        }

        // Check if the parent process is the root process
        if (processes[i].appid == root_Pid) {
            processes[i].adefuncts++;
            defunct_Count++;
        } else {
            int parent_Index = -1;

            // Find the index of the parent process in the processes array
            for (int k = 0; k < process_Count; k++) {
                if (processes[k].apid == processes[i].appid) {
                    parent_Index = k;
                    break;
                }
            }

            // Update the defunct count of the parent process and increase the overall defunct count
            if (parent_Index != -1) {
                processes[parent_Index].adefuncts += (processes[i].adefuncts + 1);
                defunct_Count++;
            }
        }
    }

    // Check if the defunct count meets the termination threshold
    if (defunct_Count >= termination_Threshold) {
        for (int i = 0; i < process_Count; i++) {
            // Check if the process is not exempted and has exceeded the termination threshold
            if (!processes[i].aexempted && processes[i].adefuncts >= termination_Threshold) {
                // Check if the process is the root process
                if (processes[i].appid == root_Pid) {
                    // Check if the root process is not the bash shell or immediate child of bash
                    if (processes[i].appid != bash_Pid && processes[i].appid != immediate_Child_Pid) {
                        // Forcefully terminate the root process
                        forceTerminateProcess1(root_Pid);
                        break;
                    }
                } else {
                    // Forcefully terminate the non-root process
                    forceTerminateProcess1(processes[i].appid);
                }
            }
        }
    }

    free(processes); // Free the allocated memory for the processes array
}

int main(int argc, char* argv[]) 
{
    // Check the number of command-line arguments
    if (argc < 2 || argc > 8) // Ensure correct number of arguments
    {
        printf("Incorrect number of arguments %s\n", argv[0]);
        return 1;
    }
    
    // Parse command-line arguments
    int process_Id = atoi(argv[1]); // Get the process ID
    int limit = -1; // Initialize limit variable
    int threshold_limit = -1; // Initialize threshold limit variable
    int root_Pid = atoi(argv[1]); // Set root process ID
    int termination_Threshold = -1; // Initialize termination threshold
    int exempted_ProcessId = -1; // Initialize exempted process ID
    
    // Handle different argument cases
    if (argc == 3 && strcmp(argv[2], "-b") == 0) // Recursive case
    {
        recursive_process_parent(process_Id);
    } 
    else if (argc >= 3 && strcmp(argv[2], "-t") == 0) // Threshold limit case
    {
        threshold_limit = atoi(argv[3]); // Get threshold limit
    } 
    else if (argc == 4 && strcmp(argv[2], "-b") == 0) // Force termination case
    {
        int threshold_limit = atoi(argv[3]); // Get threshold limit
        force_Terminate_Processes(process_Id, threshold_limit);
    } 
    else if (argc == 5 && strcmp(argv[2], "-b") == 0 && strncmp(argv[4], "-", 1) == 0) // Parsing processes case
    {
        termination_Threshold = atoi(argv[3]); // Get termination threshold
        exempted_ProcessId = atoi(argv[4] + 1); // Get exempted process ID
        parsing_Processes(root_Pid, termination_Threshold, exempted_ProcessId);
    } 
    else // Default case
    {
        recursive_process_parent(process_Id);
    }
    
    // Perform termination based on threshold or limit
    if (threshold_limit != -1) // Threshold limit termination case
    {
        Process_Info* defunct_Processes = NULL;
        int defunct_Count = 0;
        get_defunct_processes(process_Id, &defunct_Processes, &defunct_Count);
        if (defunct_Count > 0) // Check if defunct processes exist
        {
           process_forefull_termination(defunct_Processes, defunct_Count, threshold_limit);
        } 
        else 
        {
            printf("No defunct processes found.\n");
        }
        free(defunct_Processes);
    } 
    else // Defunct limit termination case
    {
        defunct_limit(process_Id, limit);
    }
    
    return 0; // Return success
}
