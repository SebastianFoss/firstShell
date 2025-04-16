#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>

/*
 * Sebastian Foss
 * CPSC 3500 Spring Quarter
 * myShell basic shell creation
 */

using namespace std;

void tokenize_command(char* command, char tokens[20][20], char* args[21]) {
    int tokenCount = 0;
    char* token = strtok(command, " ");
    while (token != NULL && tokenCount < 20) {
        strncpy(tokens[tokenCount], token, 19);
        tokens[tokenCount][19] = '\0';
        args[tokenCount] = tokens[tokenCount];
        tokenCount++;
        token = strtok(NULL, " ");
    }
    args[tokenCount] = NULL; // NULL terminate for execvp
}

int main() {

    string line;
    bool isEmpty = false;
    while (!isEmpty) {
        cout << "myshell$" << flush; // display myshell

        getline(cin, line);

        string trimmed = line; //
        trimmed.erase(remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());

        if (line.empty()) {
            isEmpty = true;
        }
    }


    char input[500]; // char for the input characters
    strncpy(input, line.c_str(), 499); // copies the string into the char array
    input[499] = '\0';

    char* cmd1 = strtok(input, "|"); // for when there are multiple commands for pipelining
    char* cmd2 = strtok(NULL, "|"); // second command


    if (cmd2 == NULL) {
        // runs for single command
    }
    char tokens[20][20]; // 2d array allocating 20 tokens
    char* args[21]; // for 20 tokens and the null pointer
    tokenize_command(input, tokens, args);

    char* args1[21]; char* tokens1[20][20];
    char* args2[21]; char* tokens2[20][20];

    int count = 0; //  for multiple commands pipelined
    while (token != NULL && count < 20) { //
        strncpy(tokens[count], token, 19);
        tokens[count][19] = '\0'; // need to ensure that it is a cstring
        args[count] = tokens[count];
        count++;
        token = strtok(NULL, " ");
    } // based on single command


    int counter = 0; // for single command
    while (token != NULL && counter < 20) { //
        strcpy(tokens[counter], token);
        args[counter] = tokens[counter];
        counter++;
        token = strtok(NULL, " ");
    }

    args[counter] = NULL; // because exec requires the array to be null-terminated

/*
 * Below
 */

    pid_t pid = fork();

    if (pid < 0) {
        perror("failed to fork");
        exit(1);
    }
    else if (pid == 0) { //  this is now the child process -- never returns if successful
        execvp(args[0], args);
        perror("failed to exec");
        exit(1);
    }
    else {
        // this is the parent process's action
        int status;
        waitpid(pid, &status, 0); // prevents zombie process
        cout << "child process " << pid << "exited with status " << WEXITSTATUS(status) << endl;
        // WEXITSTATUS(status) gives the actual return code from the child process

    }

    /*
     * - Pipe is made before forking
     * - After forking, child 1 redirects output to child 2
     * - 1 redirects stdout to pipe write-end
     * - 2 redirects stdin to pipe read-end
     *
     */

    int pipefd[2];
    pipe(pipefd);

    return 0;
}
