#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>


using namespace std;
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
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

    strcpy(input, line.c_str()); // copies the string into the char array

    char tokens[20][20]; // 2d array allocating 20 tokens

    char* args[21]; // for 20 tokens and the null pointer

    char* token = strtok(input, " "); // splits each word into a token

    int counter = 0;
    while (token != NULL && counter < 20) { //
        strcpy(tokens[counter], token);
        args[counter] = tokens[counter];
        counter++;
        token = strtok(NULL, " ");
    }

    args[counter] = NULL; // because exec requires the array to be null-terminated

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

    return 0;
}
