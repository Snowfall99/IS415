#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <utility>
#include <readline/readline.h>
#include <readline/history.h>
#include "cmdline.h"

void showHelp();
// char* parseCmd(char* , std::vector<std::pair<std::string, int> >&, std::pair<std::string, std::string>&);

int main(int argc, char** argv) {
    while (1) {
        char* line = readline("> ");
        if (!line) break;
        if (*line) add_history(line);
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        // char* cmd;
        // std::vector<std::pair<std::string, int> > options;
        // std::pair<std::string, std::string> type;
        // cmd = parseCmd(line, options, type);

        // if (strcmp(line, "help") == 0) {
        //     showHelp();
        // }

        free(line);
    }
}

void showHelp() {
    printf("CLI usage Help\n");
    printf("help \"show this message\"\n");
    printf("add <exe filename> \"add privileges of exe\"\n");
    printf("update <exe filename> \"update privileges of exe \"\n");
    printf("delete <exe filename> \"delete privilege of exe\"\n");
}

// char* parseCmd(char* line_, std::vector<std::pair<std::string, int> >& options_, std::pair<std::string, std::string>& type_) {
//     int len = strlen(line_);
//     char* cmd = (char*)malloc(sizeof(char) * 128);

//     for (int i = 0; i < len && line_[i] != ' '; i++) {
        
//     }
// }