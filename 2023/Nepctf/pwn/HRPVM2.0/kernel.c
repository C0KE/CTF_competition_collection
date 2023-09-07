#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>

char user_space[256];
// File struct
typedef struct File {
    char name[20];
    char content[1280];
    char owner[10];
    int owner_read;
    int owner_write;
    int owner_execute;
    int world_read;
    int world_write;
    int world_execute;
    int file_descriptor;
    struct File* next;
} File;
// User struct
typedef struct User {
    char name[20];
    int is_root;
    int uid;
    int gid;
    int groups[10];
} User;

// Register struct
typedef struct Regs {
    char rax[256];
    char rdi[256];
    char rsi[256];
    char rdx[256];
} Regs;



// Directory struct
typedef struct Directory {
    char name[20];
    struct File* files;
    struct Directory* subdirectories;
    struct Directory* next;
    struct Directory* parent;
} Directory;

// Driver struct
typedef struct Driver {
    char kernel_space[1024];
} Driver;
User *user;
Regs regs;
Driver *driver;
Directory* rootDirectory;
Directory* currentDirectory;
long long int global_fd=2;
int init_flag=0;
// Function to find a file by name in the current directory
File* findFileByName( const char* fileName) {
    long long int temp_fd=atoi(fileName);
    File* file = currentDirectory->files;
    while (file != NULL) {
        if (strcmp(file->name, fileName)==0||(file->file_descriptor==temp_fd)) {
            return file;
        }
        file = file->next;
    }
    return NULL; // File not found
}

// Function to create a new file
File* createFile(const char* name, const char* content, const char* owner) {
    
    
    File* newFile = (File*)malloc(sizeof(File));
    strcpy(newFile->name, name);
    strcpy(newFile->content, content);
    strcpy(newFile->owner, owner);
    newFile->owner_read = 1;
    newFile->owner_write = 1;
    newFile->owner_execute = 1;
    newFile->world_read = 1;
    newFile->world_write = 1;
    newFile->world_execute = 1;
    newFile->file_descriptor = -1;
    newFile->next = NULL;
    return newFile;
}
void copyString(char* a, const char* b, int size) {
    while (size > 0 && *b != '\0') {
        *a = *b;

        a++;
        b++;
        size--;
    }
}



// Function to create a new directory
Directory* createDirectory(const char* name, Directory* parent) {
    Directory* newDirectory = (Directory*)malloc(sizeof(Directory));
    strcpy(newDirectory->name, name);
    newDirectory->files = NULL;
    newDirectory->subdirectories = NULL;
    newDirectory->next = NULL;
    newDirectory->parent = parent;
    return newDirectory;
}

// Function to bind a file to a directory
void bindFileToDirectory(File* file, Directory* directory) {
    file->next = directory->files;
    directory->files = file;
}

// Function to create a subdirectory in a directory
void createSubdirectory(Directory* parentDirectory, Directory* subdirectory) {
    subdirectory->next = parentDirectory->subdirectories;
    parentDirectory->subdirectories = subdirectory;
}

// Function to create a new directory in the current directory
void mkdir( const char* directoryName) {
    Directory* newDirectory = createDirectory(directoryName,currentDirectory);
    createSubdirectory(currentDirectory, newDirectory);
}


// Function to change directory
void changeDirectory(const char* directoryName) {
    if (strcmp(directoryName, "..") == 0) {
        // Move to parent directory
        if (currentDirectory->parent != NULL) {
            currentDirectory=currentDirectory->parent;
            return;
        } else {
            printf("[-]Already in the root directory.\n");
            return;
        }
    }

    Directory* subdirectory = currentDirectory->subdirectories;
    while (subdirectory != NULL) {
        if (strcmp(subdirectory->name, directoryName) == 0) {
            // Move to subdirectory
            currentDirectory=subdirectory;
            return;
        }
        subdirectory = subdirectory->next;
    }

    // Directory not found
    printf("[-]Directory '%s' not found.\n", directoryName);
    return;
}


// Function to print current directory path
void printCurrentDirectoryPath() {
    if (currentDirectory == NULL) {
        printf("Invalid current directory.\n");
        return;
    }

    char path[256] = "";
    Directory* directory = currentDirectory;

    // Traverse the directory hierarchy from current directory to root
    while (directory != NULL) {
        if (strcmp(directory->name, "") != 0) {
            char temp[256];
            strcpy(temp, path);
            sprintf(path, "%s/%s", directory->name, temp);
        }
        directory = directory->parent;
    }

    // Print the final path
    printf("%s\n", path);
}

// Initialize user, file and driver
void init() {
    setbuf(stdin,0);
    setbuf(stdout,0);
    user = (User*)malloc(sizeof(User));
    strncpy(user->name, "HRP", sizeof(user->name) - 1);
    user->name[sizeof(user->name) - 1] = '\0';
    user->is_root = 0;
    user->uid = 1000;
    user->gid = 1000;
    user->groups[0] = 1000;
    user->groups[1] = -1;

    // Create root directory
    Directory* rootDirectory = createDirectory("/",NULL);
    File* file1 = createFile("flag", "flag{test}", "root");
    File* file2 = createFile("README", "Perhaps you think this question is very similar to HRPVM, but it is a bit different. The author of the question is quite awkward and went to work on the web. Let's play with the entire check-in question for everyone", "HRP");

    // Bind files to root directory
    bindFileToDirectory(file1, rootDirectory);
    bindFileToDirectory(file2, rootDirectory);
    currentDirectory=rootDirectory;

    driver = (Driver*)malloc(sizeof(Driver));
    strncpy(driver->kernel_space, "WELCOME TO NEPCTF!,THIS IS AN EASY DRIVER!", sizeof(driver->kernel_space) - 1);
    driver->kernel_space[sizeof(driver->kernel_space) - 1] = '\0';


     printf(" ___  ___  ________  ________  ___      ___ _____ ______     _______      ________\n");
    printf("|\\  \\|\\  \\|\\   __  \\|\\   __  \\|\\  \\    /  /|\\   _ \\  _   \\  /  ___  \\    |\\   __  \\    \n");
    printf("\\ \\  \\\\\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\  /  / | \\  \\\\\\__\\ \\  \\/__/|_/  /|   \\ \\  \\|\\  \\   \n");
    printf(" \\ \\   __  \\ \\   _  _\\ \\   ____\\ \\  \\/  / / \\ \\  \\\\|__| \\  \\__|//  / /    \\ \\  \\\\\\  \\  \n");
    printf("  \\ \\  \\ \\  \\ \\  \\\\  \\\\ \\  \\___|\\ \\    / /   \\ \\  \\    \\ \\  \\  /  /_/__  __\\ \\  \\\\\\  \\ \n");
    printf("   \\ \\__\\ \\__\\ \\__\\\\ _\\\\ \\__\\    \\ \\__/ /     \\ \\__\\    \\ \\__\\|\\________\\__\\ \\_______\\\n");
    printf("    \\|__|\\|__|\\|__|\\|__|\\|__|     \\|__|/       \\|__|     \\|__|\\|_______\\|__|\\|_______|\n");
    printf("\t\t\t\t\t\t\t\t\t\t\t-power by HRP\n");
    printf("%s\n","Make a wish to Nepnep");
    char *message = (char *)malloc(0x68);
    if (message == NULL) {
        printf("Failed to allocate memory.\n");
        return ;
    }

    scanf("%104s",message);
    getchar();
    init_flag=1;

}






// Handle syscall
void syscall() {
    int syscall_number = atoi(regs.rax);
    File *file = findFileByName(regs.rdi);
    if(file==NULL)
    {
        return;
    }
    switch(syscall_number) {
        case 0:  // open
            if (user->is_root || file->world_read) {
                    file->file_descriptor = ++global_fd;
                    printf("[+] File %s is opened. File descriptor: %d\n", file->name, file->file_descriptor);
                } else {
                    printf("[-] Permission denied!\n");
                }
            break;
        case 1:  // read
            if (file->file_descriptor >= 0) {
                if (user->is_root || file->world_read) {
                    copyString(user_space, file->content, atoi(regs.rdx));
                    printf("[+] Read %d bytes from file: %s\n", atoi(regs.rdx), user_space);
                } else {
                    printf("[-] Permission denied!\n");
                }
            } else if (file->file_descriptor == -1 && user->is_root) {
                copyString(user_space, driver->kernel_space, atoi(regs.rdx));
                printf("[+] Read %d bytes from driver: %s\n", atoi(regs.rdx), user_space);
            }
            break;
        case 2:  // write
            if (file->file_descriptor >= 0 ) {
                if (user->is_root || file->world_write) {
                    copyString(file->content, regs.rsi, atoi(regs.rdx));
                    printf("[+] Write %d bytes to file\n", atoi(regs.rdx));
                } else {
                    printf("[-] Permission denied!\n");
                }
            } else if (file->file_descriptor == -1 && user->is_root) {
                copyString(driver->kernel_space, regs.rsi, atoi(regs.rdx));
                printf("[+] Write %d bytes to driver\n", atoi(regs.rdx));
            }
            break;
    }
}

// Handle ls
void ls() {
    // Print files
    File* file = currentDirectory->files;
    while (file != NULL) {
        printf("File: %s\n", file->name);
        file = file->next;
    }

    // Print directories
    Directory* directory = currentDirectory->subdirectories;
    while (directory != NULL) {
        printf("Directory: %s/\n", directory->name);
        directory = directory->next;
    }
}

void echo(char *content) {
    char *data = strtok(content, ">");
    char *filename = strtok(NULL, "\n");
    if(init_flag)
    {
        File* tempfile=findFileByName(filename);
    if(tempfile!=NULL)
    {
        printf("[-]The file %s also exists.\n",tempfile->name);
        return;
    }
    }
    if (filename && data) {
        File* newFile = createFile(filename, data, user->name);
        bindFileToDirectory(newFile, currentDirectory);

        printf("[+] Echo into %s: %s\n", filename, data);
    } else {
        printf("[-] Error in echo: Invalid format\n");
    }
}

void rm(const char* fileName) {
    File* file = currentDirectory->files;
    File* prev = NULL;

    // Search for the file
    while (file != NULL) {
        if (strcmp(file->name, fileName) == 0) {
            // Found the file, remove it from the list
            if (prev != NULL) {
                prev->next = file->next;
            } else {
                currentDirectory->files = file->next;
            }
            free(file);
            printf("[+] File %s is removed.\n", fileName);
            return;
        }
        prev = file;
        file = file->next;
    }

    printf("[-] File %s not found.\n", fileName);
}

int isAlphanumeric(const char *string) {
    const char *pattern = "^[A-Za-z0-9]+$";
    regex_t regex;
    int result;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        printf("Failed to compile regex.\n");
        return -1;
    }

    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);

    if (result == 0) {
        return 1;  // 字符串只包含字母和数字
    } else if (result == REG_NOMATCH) {
        return 0;  // 字符串包含非法字符
    } else {
        printf("Regex match failed.\n");
        return -1; // 正则表达式匹配失败
    }
}

void handle_mount(char* filename) {
    char route[256] = "/";
    
    strcat(route,  currentDirectory->name);
    strcat(route, "/");
    
    if (user->is_root ) {
                File *current = findFileByName(filename);
                if (current==NULL)
                {
                    printf("[-] Error in mount: File not found\n");
                    return;
                }
                int res=isAlphanumeric(filename);
                if(res!=1)
                {
                    printf("[-] Error in mount: Invalid filename\n");
                    return;
                }
                strcat(route, filename);
                FILE* device = fopen(route, "wb");
            if (device == NULL) {
                printf("Unable to open physical device.\n");
                return;
            }

            fwrite(current->content, sizeof(char), strlen(current->content), device);

            fclose(device);

            printf("Device mounted\n");
            return;

            } else {
                printf("[-] Permission denied!\n");
                return;
        }

    
}
void exec(char *filename) {
    File *current_file = findFileByName(filename);

    if (current_file) {
        char content_copy[256];
        strncpy(content_copy, current_file->content, sizeof(content_copy) - 1);
        content_copy[sizeof(content_copy) - 1] = '\0';

        regex_t regex;
    int ret;

    // 编译正则表达式模式
    ret = regcomp(&regex, "([a-z]+)\\s+([a-z]+),([a-zA-Z0-9]+)|syscall\\s+([0-9]+);", REG_EXTENDED);
    if (ret) {
        return ;
    }

    regmatch_t match[5];
    char* str = content_copy;
    
    while (regexec(&regex, str, 5, match, 0) == 0) {
        // 检查是syscall指令还是普通指令
        if (match[4].rm_so != -1 && match[4].rm_eo != -1) {
            // 匹配到syscall指令
            char syscallNum[16];
            strncpy(syscallNum, str + match[4].rm_so, match[4].rm_eo - match[4].rm_so);
            syscallNum[match[4].rm_eo - match[4].rm_so] = '\0';
            strncpy(regs.rax, syscallNum, sizeof(regs.rax) - 1);
            regs.rax[sizeof(regs.rax) - 1] = '\0';
                        syscall();
        } else {
            // 匹配到普通指令
            char instruction[16];
            char reg[16];
            char value[16];
            
            strncpy(instruction, str + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
            instruction[match[1].rm_eo - match[1].rm_so] = '\0';
            
            strncpy(reg, str + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
            reg[match[2].rm_eo - match[2].rm_so] = '\0';
            
            strncpy(value, str + match[3].rm_so, match[3].rm_eo - match[3].rm_so);
            value[match[3].rm_eo - match[3].rm_so] = '\0';
            if (strcmp(instruction, "mov") == 0) {
                    if (reg && value) {
                        if (strcmp(reg, "rdi") == 0) {
                            strncpy(regs.rdi, value, sizeof(regs.rdi) - 1);
                            regs.rdi[sizeof(regs.rdi) - 1] = '\0';
                        } else if (strcmp(reg, "rsi") == 0) {
                            strncpy(regs.rsi, value, sizeof(regs.rsi) - 1);
                            regs.rsi[sizeof(regs.rsi) - 1] = '\0';
                        } else if (strcmp(reg, "rdx") == 0) {
                            strncpy(regs.rdx, value, sizeof(regs.rdx) - 1);
                            regs.rdx[sizeof(regs.rdx) - 1] = '\0';
                        }
                    }
                }

        }
        
        // 更新字符串指针，继续匹配下一个指令
        str += match[0].rm_eo;
    }
    
    // 释放正则表达式资源
    regfree(&regex);
        
    }


}




// Handle id
void id() {
    printf("uid=%d(%s) gid=%d(%s) groups=", user->uid, user->name, user->gid, user->name);
    for (int i = 0; i < 10 && user->groups[i] != -1; i++) {
        printf("%d(%s) ", user->groups[i], user->name);
    }
    printf("\n");
}

// Handle cat
void cat(char* filename) {
    File* curr = currentDirectory->files;
    while (curr) {
        if (strcmp(curr->name, filename) == 0) {
            if (user->is_root || strcmp(curr->owner, user->name) == 0) {
                printf("%s\n", curr->content);
                return;
            } else {
                printf("[-] Permission denied!\n");
                return;
            }
        }
        curr = curr->next;
    }

    printf("[-] Error in cat: File not found\n");
}


int main() {
    init();

    char command[2560];
    while (1) {
        printf("~%s@%s$ ",currentDirectory->name,user->name);
        scanf(" %2550[^\n]", command);
        char *operation = strtok(command, " ");
        if (strcmp(operation, "echo") == 0) {
            echo(strtok(NULL, "\n"));
        } else if (strcmp(operation, "ls") == 0) {
            ls();
        } else if (strcmp(operation, "rm") == 0) {
            rm(strtok(NULL, "\n"));
        }else if (strcmp(operation, "exec") == 0) {
            exec(strtok(NULL, "\n"));
        } else if (strcmp(operation, "id") == 0) {
            id();
        } else if (strcmp(operation, "clear") == 0) {
             printf("\033[2J\033[H");
        }else if (strcmp(operation, "cat") == 0) {
            cat(strtok(NULL, "\n"));
        } else if (strcmp(operation, "mount") == 0) {
            handle_mount(strtok(NULL, "\n"));
        }else if (strcmp(operation, "cd") == 0) {
            changeDirectory(strtok(NULL, "\n"));
        }else if (strcmp(operation, "pwd") == 0) {
            printCurrentDirectoryPath();
        }else if (strcmp(operation, "mkdir") == 0) {
            mkdir(strtok(NULL, "\n"));
        }else if (strcmp(operation, "exit") == 0) {
            break;
        } else {
            printf("[-] Command not found: %s\n", operation);
        }
    }
    return 0;
}
