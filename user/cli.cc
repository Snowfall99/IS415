#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <utility>
#include <readline/readline.h>
#include <readline/history.h>

const std::string READ = "read";
const std::string WRITE = "write";

const std::string ADD = "add";
const std::string UPDATE = "update";
const std::string DELETE = "delete";

void showHelp();
bool parseCmd(std::string, std::string&, std::string&, std::vector<std::pair<std::string, int> >&, std::pair<std::string, std::string>&);
bool checkOpt(std::vector<std::pair<std::string ,int> >);

int main(int argc, char** argv) {
    while (1) {
        char* line = readline("> ");
        if (!line) break;
        if (*line) add_history(line);
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        std::string lineStr(line);
        free(line);

        if (lineStr == "help") {
            showHelp();
        } else if (lineStr == "exit") {
            break;
        } else {
            std::string cmd;
            std::string exe;
            std::vector<std::pair<std::string, int> > options;
            std::pair<std::string, std::string> type;
            if (!parseCmd(lineStr, cmd, exe, options, type)) {
                continue;
            }
            if (!checkOpt(options)) {
                continue;
            }
            printf("cmd: %s\n", cmd.c_str());
            printf("exe: %s\n", exe.c_str());
            printf("type: %s\n", type.second.c_str());
            for (int i = 0; i < options.size(); i++) {
                printf("%s: %d\n", options[i].first.c_str(), options[i].second);
            }
        }
    }
}

void showHelp() {
    printf("CLI usage Help\n");
    printf("help \"show this message\"\n");
    printf("add <exe filename> \"add privileges of exe\"\n");
    printf("update <exe filename> \"update privileges of exe \"\n");
    printf("delete <exe filename> \"delete privilege of exe\"\n");
    printf("exit \"exit cli\"\n");
}

bool parseCmd(std::string line_, std::string& cmd_, std::string& exe_, std::vector<std::pair<std::string, int> >& options_, std::pair<std::string, std::string>& type_) {
    int len = strlen(line_.c_str());
    bool flag = true;
    int i = 0;

    // trim blank space
    while (line_[i] == ' ') {
        i ++;
    }

    // get the cmd
    while (line_[i] != ' ') {
        cmd_.push_back(line_[i]);
        i ++;
    }
    i ++;

    // check cmd
    if (strcmp(cmd_.c_str(), ADD.c_str()) != 0 && strcmp(cmd_.c_str(), UPDATE.c_str()) != 0 && strcmp(cmd_.c_str(), DELETE.c_str()) != 0) {
        flag = false;
        printf("undefined command: %s\n", cmd_.c_str());
        return flag;
    }

    // get exe filename
    while (line_[i] != ' ') {
        exe_.push_back(line_[i]);
        i ++;
    }

    // get options
    while (i < len) {
        if (line_[i] == ' ') {
            i ++;
            continue;
        }
        if (line_[i] == '-') {
            i ++;
            if (line_[i] == 't') {
                std::pair<std::string, std::string> type;
                type.first = "type";
                i += 2;
                std::string t;
                while (line_[i] != ' ' && i < len) {
                    t.push_back(line_[i]);
                    i ++;
                }
                type.second = t;
                type_ = type;
            } else if (line_[i] == 'w') {
                std::pair<std::string, int> option;
                option.first = "write";
                i += 2;
                option.second = (line_[i] - '0');
                options_.push_back(option);
            } else if (line_[i] == 'r') {
                std::pair<std::string, int> option;
                option.first = "read";
                i += 2;
                option.second = (line_[i] - '0');
                options_.push_back(option);
            } else {
                printf("fail to parse undefined flag\n");
                flag = false;
                break;
            }
            i ++;
            continue;
        }
        flag = false;
        printf("fail to parse undefined flag\n");
        break;
    }
 
    return flag;
}

bool checkOpt(std::vector<std::pair<std::string, int> > options_) {
    int optLen = options_.size();
    bool flag = true;

    for (int i = 0; i < optLen; i++) {
        std::string optKey = options_[i].first;
        int optValue = options_[i].second;
        if (optKey != READ && optKey != WRITE) {
            flag = false;
            printf("Fail to parse %s\n", optKey.c_str());
            break;
        }
        if (optValue != 0 && optValue != 1) {
            flag = false;
            printf("Option value can only be 0 or 1\n");
            break;
        }
    }

    return flag;
}