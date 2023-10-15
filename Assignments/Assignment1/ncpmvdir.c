

#define _XOPEN_SOURCE 500	//defines feature test macro to enable certain POSIX functions and definitions
#include <stdio.h>		//standard input/output functions
#include <stdlib.h>		//standard library functions
#include <string.h>		//string manipulation functions
#include <sys/stat.h>		//File status functions
#include <unistd.h>		//Symbolic constants and types
#include <dirent.h>		//Directory handling functions
#include <ftw.h>		//file tree walk functions
#include <limits.h>		//constants for path length

//source folder path
char *sourcePath;

//destination folder path
char *destinationPath;

//varaiable to check whether to move or copy
int IsMove;

//number of extensions from command line
int numberOfExtensions;

//Array of file extensions to filter
char **extensions;

//Function to copy a file or create directory
void creationFileDirectory(const char *src, const char *dest) {
	struct stat sourceStat;
	stat(src, &sourceStat);

	//check if the source file is a regular file
	if (S_ISREG(sourceStat.st_mode)) {
		//open the source file in binary read mode
        	FILE *sourceFilePath = fopen(src, "rb");
		
		//open the destination file in binary write mode
        	FILE *destinationFilePath = fopen(dest, "wb");
		
		//create a buffer to store data while copying
        	char buffer[4096];
        	size_t bytes_read;
		//copy data from source folder to desination folder
		while ((bytes_read = fread(buffer, 1, sizeof(buffer), sourceFilePath)) > 0) {
           		 fwrite(buffer, 1, bytes_read, destinationFilePath);
        	}
		//close the source file
        	fclose(sourceFilePath);
		//close the destination file
       		fclose(destinationFilePath);
  	}
	//If the source file is directory
	else if (S_ISDIR(sourceStat.st_mode)) {
		//create a new directory  at the destination path with all the permissions(0777)
        	mkdir(dest, 0777);
    	}
}

//Function to recursively creating directories along a given path
void directoryRecursive(const char *path) {
	//create a copy of the path to avoid modifying original string
	char *copy = strdup(path);
	char*c=copy+1;
	while(*c){
        	if (*c == '/') {
            	*c = '\0';
            	mkdir(copy, 0777);
            	*c = '/';
        }
	c++;
    }
	//free the memory allocated for the copied path
	free(copy);
    	mkdir(path, 0777);
}

//Function to create directories recursively upto a specific destination path
void recursiveCreation(const char *src, const char *dest) {
	char *copy = strdup(dest);
	char *p=copy+1;
	while(*p){
        	if (*p == '/') {
            	*p = '\0';
		//Recursively create the directory path 
            	directoryRecursive(copy);
            	*p = '/';
        	}
	p++;
    	}
	free(copy);
	//create the file or directory at the specified source and destination paths
    	creationFileDirectory(src, dest);
}

//Function called by the file tree walk (nftw) to copy or move files
int copyOrMove(const char *folderPath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	int isCreate = 1;
	//check if  the number of extensions is greater than zero and if the current item is a regular file 
    	if (numberOfExtensions > 0 && S_ISREG(sb->st_mode)) {
        int extensionMatch = 0;

        	for (int i = 0; i < numberOfExtensions; i++) {
        		int lengthOfExtension = strlen(extensions[i]);
            		int lengthOfPath = strlen(folderPath);
		
			//check if the extension matches the file's extension
	        	if (strcmp(folderPath + lengthOfPath - lengthOfExtension, extensions[i]) == 0 &&
                	*(folderPath + lengthOfPath - lengthOfExtension - 1) == '.') {
                	extensionMatch = 1;
                	break;
       			}
        	}
		//If the extension matches, set isCreate to false 
		if (extensionMatch) {
       			isCreate = 0;
        	}
    	}
	//check if creation is required
	if (isCreate) {
		//Create the destination path for the current item
        	char path[strlen(folderPath) - strlen(sourcePath) + strlen(destinationPath) + 1];
        	strcpy(path, destinationPath);
        	strcat(path, folderPath + strlen(sourcePath));
		//recursively create directories and copy/move files
        	recursiveCreation(folderPath, path);

        	if (S_ISREG(sb->st_mode)) {
            		if (IsMove) {
				//move file to destination path
                		int result = rename(folderPath, path);
                		if (result != 0) {
                    			printf("Error moving file/directory: %s\n", folderPath);
                		}
            		} else {
				//copy file to destination path 
                		creationFileDirectory(folderPath, path);
            		  }
        	}
    	}

    return 0;
}

//Function to check whether  a given filename corresponds to directory or not
int isDirectory(const char *fileName) {
	struct stat path;
	//check if file/directory exists and retrieve its information
    	if (stat(fileName, &path) != 0) {
        	return 0;
    	}
	//check if the retrieved information indicates a directory
    	return S_ISDIR(path.st_mode) ? 1 : 0;
}

//Function to recursively delete the source directory after moving
void deleteSource(const char *path) {
	struct stat st;
	//check if file or directory exists and retrieve its information
    	if (stat(path, &st) != 0) {
        	printf("Error getting file information: %s\n", path);
        	return;
    	}
	//check if the path corresponds to a directory
    	if (S_ISDIR(st.st_mode)) {
        	DIR *dir = opendir(path);
		//check if the directory can be opened 
        	if (dir == NULL) {
            		printf("Error opening directory: %s\n", path);
            		return;
        	}	
        	struct dirent *entry;	//declares a pointer variable named entry of type struct dirent
        	while ((entry = readdir(dir)) != NULL) {
			//skip the current directory(.) and parent directory(..)
            		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                	continue;
            		}
			//create the subpath by appending the entry name to the current path 
            		char subpath[strlen(path) + strlen(entry->d_name) + 2];
            		sprintf(subpath, "%s/%s", path, entry->d_name);

			//recursively delete the subpath
            		deleteSource(subpath);
        	}
        	closedir(dir);
        	rmdir(path);	//Remove the directory
    	} else {
        	unlink(path);	//Remove the file
    	}
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		//check if the required command lines arguments are provided 
        	printf("ExecutableFileName sourcePath destinationPath [options] [extensions]\n");
        	printf("Arguments (-mv, -cp) are required. Extensions are optional.\n");
        	exit(1);
    	}
	//Extract the source and destination paths from command line arguments
    	sourcePath = strdup(argv[1]);
    	destinationPath = strdup(argv[2]);
	
	//normalize the source path by removing a trailing '/'
    	if (sourcePath[strlen(sourcePath) - 1] == '/') {
        	*(sourcePath + strlen(sourcePath) - 1) = '\0';
    	}
	//check if the source path is a directory
    	if (!isDirectory(sourcePath)) {
        	printf("Source path must be a directory.\n");
        	exit(1);
    	}
	//normalize destination path by removing a trailing '/'
    	if (destinationPath[strlen(destinationPath) - 1] == '/') {
        	*(destinationPath + strlen(destinationPath) - 1) = '\0';
    	}
	//Get the last component of source path (folder name)
    	char *sourceFolder = strrchr(sourcePath, '/');
    	char newDestination[strlen(destinationPath) + strlen(sourceFolder) + 1];
    	strcpy(newDestination, destinationPath);
    	strcat(newDestination, sourceFolder);	//create the new destination path by appending the source folder name
    	destinationPath = newDestination;
	
	//determine the operation mode (copy or move) based on option provided
    	char *option = argv[3];

    	if (strcmp(option, "-cp") == 0) {
        	IsMove = 0;	//set move flag to 0(copy mode)
    	} else if (strcmp(option, "-mv") == 0) {
        	IsMove = 1;	//set move flag to 1(move mode)
    	} else {
        	printf("Options are move (-mv) or copy (-cp).\n");
        	exit(1);
    	}
	//extract the file extensions(if provided) from the command line arguments
    	numberOfExtensions = argc - 4;
   	 extensions = argv + 4;
	
	//perform the copy or move operation using nftw function
    	if (IsMove) {	
		//move mode
        	int result = nftw(sourcePath, copyOrMove, 64, FTW_DEPTH | FTW_PHYS);
        	if (result == 0) {
            		deleteSource(sourcePath);	//delete the source directory after successfull move
        	} else {
        		printf("Error moving files or directories\n");
        	}
    	} else {
		//copy mode
        	int result = nftw(sourcePath, copyOrMove, 64, FTW_DEPTH | FTW_PHYS);
        	if (result != 0) {
            		printf("Error copying files\n");
        	}
    	}

    	return 0;
}
