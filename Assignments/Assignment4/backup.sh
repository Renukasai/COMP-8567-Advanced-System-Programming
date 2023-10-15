Sai Renuka Telu
ID: 110096549
Section 1

#!/bin/bash

#This function is wriiten to create a backup directory if it doesn't already exist. It takes one argument (the directory path to be backed up), and if the directory doesn't exist, it will create it using mkdir -p
create_backup_directory_re() {
  #Takes one arguement which will be stored in the local variable "dir_re"
    local dir_re="$1"
    # Validating whether a directory specified by "$dir_re" exists using the "-d" option.
    if [ ! -d "$dir_re" ]; then
    #If the directory does not exist create a directory using mkdir 
        mkdir -p "$dir_re"
    fi
}

# This function is written to retrieve the latest backup number for a given backup type. It takes one argument (the backup type), and it searches for files with names starting with the specified backup type and ending with ".tar". It then extracts the numeric part from the latest file's name, representing the latest backup number.
get_latest_backup_number_re() {
    local backup_type_re="$1"
# The function uses "find" command to search for files in the directory specified by "$backup_dir_re/$backup_type_re". The "-type f" option ensures it only looks for regular files (not directories).The "-name" option specifies the pattern of the file names to search for. The pattern is "${backup_type_re}*.tar", which means files starting with "$backup_type_re" and ending with ".tar". The result of the find command will be a list of matching files. The list is then sorted numerically using "sort -n". The last line of the sorted list (which would be the latest backup) is captured in the variable "latest_backup_re".
    local latest_backup_re=$(find "$backup_dir_re/$backup_type_re" -type f -name "${backup_type_re}*.tar" | sort -n | tail -n 1)
    #The function checks whether the variable "latest_backup_re" contains any value or is not empty.
    if [ -n "$latest_backup_re" ]; then
  #If the variable is not empty, extract the numeric part of the file name using "grep -oE '[0-9]+'". This regex pattern extracts any sequence of digits in the file name.
        latest_backup_re=$(basename "$latest_backup_re" | grep -oE '[0-9]+')
  #Printing the extracted numeric part, which represents the latest backup number, so backup number will be in the form of cb001 or ib001.
        echo "$latest_backup_re"
    else
   # If the variable is empty (no backups found), echo "0" to indicate there are no previous backups.
        echo "0"
    fi
}

# This function ensures the creation of a complete backup of all ".txt" files present in the /home/renuka(home directory). It tracks the backup number for complete backups, creates the backup directory if needed, and generates backup filenames following the convention "cb001.tar" where "001" is the backup number. The function then adds an entry to the backup log, noting the creation time and the filename of the new backup.
create_complete_backup_re() {
 # Generate a timestamp in the format: "Day dd Month YYYY Time Zone".
    local timestamp_re="$(date +"%a %d %b %Y %r %Z")"
  # Define the backup type as "cb" (complete backup).
    local backup_type_re="cb"
  # Get the latest backup number for the backup type "cb".The "get_latest_backup_number_re" function is called to retrieve the latest backup number.
    local latest_cb_number_re=$(get_latest_backup_number_re "$backup_type_re")
# Create the backup directory for the complete backup. The "create_backup_directory_re" function is called to create the directory if it doesn't exist.
    create_backup_directory_re "$backup_dir_re/$backup_type_re"

# Find all .txt files in the "/home/renuka" directory and store their paths in the "txt_files_re" variable. Any errors encountered during the find process are redirected to /dev/null.
    txt_files_re=$(find "/home/renuka" -type f -name "*.txt" 2>/dev/null)
    # Check if there are any .txt files found.
    if [ -n "$txt_files_re" ]; then
     # If there are .txt files, calculate the next backup number.
        local cb_number_re=$((latest_cb_number_re + 1))
    # Create a tar archive of the .txt files with a filename in the format "cb001.tar" (where 001 is the backup number). The archive is created in the appropriate backup directory. Any errors during the tar creation process are redirected to /dev/null.
        tar -cf "$backup_dir_re/$backup_type_re/${backup_type_re}$(printf '%03d' "$cb_number_re").tar" $txt_files_re 2>/dev/null
 # Record the backup creation event in the backup log file. The log entry includes the timestamp and the name of the created backup file.
        echo "$timestamp_re ${backup_type_re}$(printf '%03d' "$cb_number_re").tar was created" >> "$backup_log_re"
    fi
}

#This function ensures the creation of an incremental backup of ".txt" files that have been modified within the last 2 minutes in the /home/renuka (home directory). It tracks the backup number for incremental backups, creates the backup directory if needed, and generates backup filenames following the convention "ibXXX.tar" where "XXX" is the backup number. The function then adds log entries indicating whether an incremental backup was created or not, based on the presence of modified ".txt" files.
create_incremental_backup_re() {
#Generate a timestamp in the format: "Day dd Month YYYY Time Zone".
    local timestamp_re="$(date +"%a %d %b %Y %r %Z")"
    # Define the backup type as "ib" (incremental backup).
    local backup_type_re="ib"
# Get the latest backup number for the backup type "ib". The "get_latest_backup_number_re" function is called to retrieve the latest backup number.
    local latest_ib_number_re=$(get_latest_backup_number_re "$backup_type_re")
  
 # Create the backup directory for the incremental backup. The "create_backup_directory_re" function is called to create the directory if it doesn't exist.
    create_backup_directory_re "$backup_dir_re/$backup_type_re"

#Find .txt files modified within the last 2 minutes and store their paths in the "txt_files_re" variable. Any errors encountered during the find process are redirected to /dev/null.
    txt_files_re=$(find "/home/renuka" -type f -name "*.txt" -mmin -2 2>/dev/null)
    # Check if there are any .txt files found.
    if [ -n "$txt_files_re" ]; then
# If there are .txt files, calculate the next backup number.
        local ib_number_re=$((latest_ib_number_re + 1))
# Create a tar archive of the .txt files modified within the last 2 minutes. The archive is given a filename in the format "ib001.tar" (where 001 is the backup number). The archive is created in the appropriate backup directory. Any errors during the tar creation process are redirected to /dev/null.
        tar -cf "$backup_dir_re/$backup_type_re/${backup_type_re}$(printf '%03d' "$ib_number_re").tar" $txt_files_re 2>/dev/null
   # Record the backup creation event in the backup log file. The log entry includes the timestamp and the name of the created backup file.
        echo "$timestamp_re ${backup_type_re}$(printf '%03d' "$ib_number_re").tar was created" >> "$backup_log_re"
    else
 # If there are no .txt files modified within the last 2 minutes, add a log entry indicating no changes.
        echo "$timestamp_re No changes-Incremental backup was not created" >> "$backup_log_re"
    fi
}

# Function to wait for 2 minutes (120 seconds).
wait_2_minutes_re() {
# The `sleep` command pauses the script for 2 minutes using the "2m" argument, where "m" stands for minutes.
    sleep 2m
}

# Paths and filenames to be created
backup_dir_re="/home/renuka/home/backup"
backup_log_re="$backup_dir_re/backup.log"

# Creating the main backup directory if it doesn't exist.
create_backup_directory_re "$backup_dir_re"
# Creating a subdirectory named "cb" (complete backup) within the main backup directory.
create_backup_directory_re "$backup_dir_re/cb"
# Creating another subdirectory named "ib" (incremental backup) within the main backup directory.
create_backup_directory_re "$backup_dir_re/ib"

# Initial complete backup
create_complete_backup_re
# Start an infinite loop using 'while true'
while true; do

    # Wait for 2 minutes
    wait_2_minutes_re

    # Check if there are changes within the 8-minute interval
    has_changes_re=0
    for i_re in {1..3}; do
        create_incremental_backup_re
        if [ $? -eq 0 ]; then
            has_changes_re=1
        fi
        wait_2_minutes_re
    done

    # If there are no changes within the 8-minute interval, print "No changes statement for every 2 minutes time interval"
    if [ "$has_changes_re" -eq 0 ]; then
        timestamp_no_changes_re="$(date +"%a %d %b %Y %r %Z")"
        echo "$timestamp_no_changes_re No changes-Incremental backup was not created" >> "$backup_log_re"
    fi

    # Perform complete backup after 8 minutes
    create_complete_backup_re
done &
