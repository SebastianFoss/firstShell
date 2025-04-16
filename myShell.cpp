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

// used to tokenize commands
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

// used to execute pipelines
void execute_pipeline(char* commands[], int num_commands) {
    char tokenMaster[10][20][20]; // up to 10 commands, each with 20 tokens max
    char* args[10][21]; // execvp args per command
    int pipefd[9][2]; // at most 9 pipes for 10 commands
    pid_t pids[10];

    // tokenizes each command into args
    for (int i = 0; i < num_commands; ++i) {
        tokenize_command(commands[i], tokenMaster[i], args[i]);
    }

    // creates pipes for each
    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipefd[i]) < 0) {
            perror("failed to create pipe");
            exit(1);
        }
    }

    // forks each command
    for (int i = 0; i < num_commands; ++i) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("failed to fork");
            exit(1);
        }
        else if (pids[i] == 0) {
            // redirects stdin if not first command
            if (i > 0) {
                dup2(pipefd[i - 1][0], STDIN_FILENO);
            }
            // redirects stdout if not last command
            if (i < num_commands - 1) {
                dup2(pipefd[i][1], STDOUT_FILENO);
            }

            // then closes all pipe ends
            for (int j = 0; j < num_commands - 1; ++j) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }

            execvp(args[i][0], args[i]);
            perror("failed to exec command");
            exit(1);
        }
    }

    // parent closes all pipes
    for (int i = 0; i < num_commands - 1; ++i) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    // waits for all children to avoid zombie processes *zombie noise*
    int status;
    for (int i = 0; i < num_commands; ++i) {
        waitpid(pids[i], &status, 0);
        cout << "child process " << pids[i] << " exits with exit status value " << WEXITSTATUS(status) << endl;
    }
}

int main() {
    while (true) {
        string line;
        cout << "myshell$" << flush;

        getline(cin, line);
        if (line.empty()) continue;
        if (line == "exit") break; // ends if user says exit

        // Splits into commands using '|'
        char* commands[10];
        int num_commands = 0;

        char input[500]; // char for the input characters
        strncpy(input, line.c_str(), 499); // copies the string into the char array
        input[499] = '\0';

        char* token = strtok(input, "|");
        while (token != NULL && num_commands < 10) {
            // Trim leading spaces
            while (*token == ' ') token++;
            commands[num_commands++] = token;
            token = strtok(NULL, "|");
        }

        if (num_commands==1) {
            // runs for single command

            char tokens[20][20]; // 2d array allocating 20 tokens
            char* args[21]; // for 20 tokens and the null pointer
            tokenize_command(input, tokens, args);

            pid_t pid = fork();

            if (pid < 0) {
                perror("failed to fork");
                exit(1);
            }
            else if (pid == 0) { //  this is now the process -- never returns if successful
                execvp(args[0], args);
                perror("failed to exec");
                exit(1);
            }
            else {
                // this is the parent process's action
                int status;
                waitpid(pid, &status, 0); // prevents zombie process
                cout << "process " << pid << " exits with exit status value " << WEXITSTATUS(status) << endl;
                // WEXITSTATUS(status) gives the actual return code from the process
            }
        }

        else {
            execute_pipeline(commands, 10); // for multiple commands
        }

        return 0;
    }
}
