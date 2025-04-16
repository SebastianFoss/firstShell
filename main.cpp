#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>


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


    char input[500];

    strcpy(input, line.c_str());

    char tokens[20][20];

    char* args[21]; // for 20 tokens and the null pointer

    char* token = strtok(input, " ");

    int counter = 0;

    while (token != NULL && counter < 20) {
        strcpy(tokens[counter], token);
        args[counter] = tokens[counter];
        counter++;
        token = strtok(NULL, " ");
    }


    // wait for command

    // std::string, read the input line into this

    // validate that the input isn't empty

    //

    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.