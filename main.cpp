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

        char tokens[20][20]; // 2d array allocating 20 tokens
        char* args[21]; // for 20 tokens and the null pointer
        tokenize_command(input, tokens, args);

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
    }
    else {
        // runs for both
        char tokens1[20][20], tokens2[20][20];
        char* args1[21], *args2[21];
        tokenize_command(input, tokens1, args1);
        tokenize_command(input, tokens2, args2);

        // begin pipelining

        /*
         * - Pipe is made before forking
         * - After forking, child 1 redirects output to child 2
         * - 1 redirects stdout to pipe write-end
         * - 2 redirects stdin to pipe read-end
         */

        int pipefd[2];
        if (pipe(pipefd) < 0) {
            perror("failed to create pipe");
            exit(1);
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            // child 1 redirects stdout to write end
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            execvp(args1[0], args1);
            perror("failed to exec");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // child 2 redirects stdin to read end
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(args2[0], args2);
            perror("failed to exec");
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        int status;
        waitpid(pid1, &status, 0); // prevents zombie process
        cout << "child process " << pid1 << "exited with status " << WEXITSTATUS(status) << endl;
        // WEXITSTATUS(status) gives the actual return code from the child process

        waitpid(pid2, &status, 0);
        cout << "child process " << pid2 << "exited with status " << WEXITSTATUS(status) << endl;
        // WEXITSTATUS(status) gives the actual return code from the child process

    }

    return 0;
}
