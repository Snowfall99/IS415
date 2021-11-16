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

struct Privilege {
    std::string exe;
    std::string type;
    int write;
    int read;

    Privilege(std::string exe_, std::string type_) : exe(exe_), type(type_), write(0), read(0) {}
    Privilege(std::string exe_, std::string type_, int write_=0, int read_=0) : exe(exe_), type(type_), write(write_), read(read_) {} 
};

void showHelp();
bool parseCmd(std::string, std::string&, std::string&, std::vector<std::pair<std::string, int> >&, std::pair<std::string, std::string>&);
bool checkOpt(std::vector<std::pair<std::string ,int> >);
void listPrivileges();
Privilege genPrivilege(std::string, std::string, std::vector<std::pair<std::string, int> >);

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
        } else if (lineStr == "list") {
            listPrivileges();
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
            Privilege privilege = genPrivilege(exe, type.second, options);
            printf("cmd: %s\n", cmd.c_str());
            printf("exe: %s\n", privilege.exe.c_str());
            printf("type: %s\n",privilege.type.c_str());
            printf("write: %d\n", privilege.write);
            printf("read: %d\n", privilege.read);
        }
    }
}

void showHelp() {
    // TODO alignment
    printf("CLI usage Help\n");
    printf("help \"show this message\"\n");
    printf("add <exe filename> <-t type>\"add privileges of exe\"\n");
    printf("update <exe filename> <-t type>\"update privileges of exe \"\n");
    printf("delete <exe filename> <-t type>\"delete privilege of exe\"\n");
    printf("exit \"exit cli\"\n");
}

// TODO 
// return a privilege
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

Privilege genPrivilege(std::string exe_, std::string type_, std::vector<std::pair<std::string, int> > options_) {
    int optLen = options_.size();
    int write = 0;
    int read = 0;

    for (int i = 0; i < optLen; i++) {
        if (options_[i].first == READ) {
            read = options_[i].second;
        } else if (options_[i].first == WRITE) {
            write = options_[i].second;
        }
    }

    Privilege privilege(exe_, type_, write, read);
    return privilege;
}

void listPrivileges() {
    // TODO
    // send list message to kernel
    // wait for kernel return privileges
}