#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "netlink.h"

class Flag {
private:
    std::string key;
    std::string value;
    std::string shortName;
    std::string longName;
    std::string description;
    bool isRequired;
    bool needValue;

public:
    Flag() : isRequired(false), needValue(false) {}
    Flag(std::string key_) : key(key_), isRequired(false), needValue(false) {}
    Flag(std::string key_, std::string value_) : key(key_), value(value_) {}

    void ShowHelp() {
        if (longName != "") {
            std::cout << std::left << std::setw(16) << "--" << longName << "\t";
        } else {
            std::cout << std::setw(16) << "" << "\t";
        }
        if (shortName != "") {
            std::cout << std::left << std::setw(8) << "-" << shortName;
        } else {
            std::cout << std::setw(8) << "";
        }
    }

    void SetValue(std::string value_) {
        value = value_;
    }

    void SetLongName(std::string longName_) {
        longName = longName_;
    }

    void SetShortName(std::string shortName_) {
        shortName = shortName_;
    }

    void SetDescription(std::string description_) {
        description = description_;
    }

    void SetIsRequired(bool isRequired_) {
        isRequired = isRequired_;
    }

    void SetNeedValue(bool needValue_) {
        needValue = needValue_;
    }

    std::string GetKey() {
        return key;
    }

    std::string GetValue() {
        return value;
    }

    std::string GetLongName() {
        return longName;
    }

    std::string GetShortName() {
        return shortName;
    }

    std::string GetDescription() {
        return description;
    }

    bool IsRequired() {
        return isRequired;
    }

    bool NeedValue() {
        return needValue;
    }
};


class Subcommand {
private:
    std::string name;
    std::string description;
    std::vector<Flag> flags;

public:
    Subcommand() {}
    Subcommand(std::string name_) : name(name_), description("") {}

    void ShowHelp() {
        std::cout << "\033[32m" << std::left << std::setw(16) << name << "\033[0m";
        if (description != "") {
            std::cout << std::left << description << std::endl;
        } else {
            std::cout << std::endl;
        }
    }

    void AddFlag(struct Flag flag_) {
        flags.push_back(flag_);
    }

    void SetDescription(std::string description_) {
        description = description_;
    }

    std::string GetName() {
        return name;
    }

    std::string GetDescription() {
        return description;
    }

    std::vector<Flag> GetFlags() {
        return flags;
    }

    bool ValidLongName(std::string flag_) {
        for (auto flag : flags) {
            if (flag.GetLongName() == flag_) {
                return true;
            }
        }
        return false;
    }

    bool ValidShortName(std::string flag_) {
        for (auto flag : flags) {
            if (flag.GetShortName() == flag_) {
                return true;
            }
        }
        return false;
    }

    Flag FindByLongName(std::string ln_) {
        for (std::vector<Flag>::iterator it = flags.begin(); it != flags.end(); it++) {
            if (it->GetLongName() == ln_) {
                return *it;
            }
        }
        return Flag();
    }

    Flag FindByShortName(std::string sn_) {
        for (std::vector<Flag>::iterator it = flags.begin(); it != flags.end(); it++) {
            if (it->GetShortName() == sn_) {
                return *it;
            }
        }
        return Flag();
    }

    bool FindByKey(std::string key_) {
        for (std::vector<Flag>::iterator it = flags.begin(); it != flags.end(); it++) {
            if (it->GetKey() == key_) {
                return true;
            }
        }
        return false;
    }
};


class Firmiana {
private:
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<Subcommand> subcommands;

private:
    void addPrivilege(Subcommand sc_) {
        char msg[64];
        std::string exe;
        std::string type;

        exe = sc_.FindByLongName("exe").GetValue();
        type = sc_.FindByLongName("type").GetValue();
        for (auto flag : sc_.GetFlags()) {
            if (flag.GetLongName() == "exe" || flag.GetLongName() == "type") {
                continue;
            } else if (flag.GetLongName() == "mkdir" || flag.GetLongName() == "rmdir") {
                std::sprintf(msg, "%-8s %-8s %-8s %-16s %d", "add", flag.GetLongName().c_str(), exe.c_str(), "all", (flag.GetValue()[0] - '0'));
                my_send_msg(msg);
            } else {
                std::sprintf(msg, "%-8s %-8s %-8s %-16s %d", "add", flag.GetLongName().c_str(), exe.c_str(), type.c_str(), (flag.GetValue()[0] - '0'));
                my_send_msg(msg);
            }
        }
        return;
    }

    void updatePrivilege(Subcommand sc_) {
        char msg[64];
        std::string exe;
        std::string type;

        exe = sc_.FindByLongName("exe").GetValue();
        type = sc_.FindByLongName("type").GetValue();
        for (auto flag : sc_.GetFlags()) {
            if (flag.GetLongName() == "exe" || flag.GetLongName() == "type") {
                continue;
            } else if (flag.GetLongName() == "mkdir" || flag.GetLongName() == "rmdir") {
                std::sprintf(msg, "%-8s %-8s %-8s %-16s %d", "update", flag.GetLongName(), exe.c_str(), "all", (flag.GetValue()[0] - '0'));
                my_send_msg(msg);
            } else {
                std::sprintf(msg, "%-8s %-8s %-8s %-16s %d", "update", flag.GetLongName(), exe.c_str(), type.c_str(), (flag.GetValue()[0] - '0'));
                my_send_msg(msg);
            }
        }
        return;
    }

    void deletePrivilege(Subcommand sc_) {
        char msg[64];
        std::string exe;
        std::string type;

        exe = sc_.FindByLongName("exe").GetValue();
        type = sc_.FindByLongName("type").GetValue();
        for (auto flag : sc_.GetFlags()) {
            if (flag.GetLongName() == "exe" || flag.GetLongName() == "type") {
                continue;
            } else {
                std::sprintf(msg, "%-8s %-8s %-8s %-16s", "delete", flag.GetLongName().c_str(), exe.c_str(), type.c_str());
                my_send_msg(msg);
            }
        }
        return;
    }

    void listPrivilege() {
        char msg[64];
        std::sprintf(msg, "%-8s %-34s", "list", " ");
        my_send_msg(msg);
        return;
    }

public:
    Firmiana(std::string name_) : name(name_) {}
    
    void Version(std::string version_) {
        version = version_;
    }

    void Author(std::string author_) {
        author = author_;
    }

    void Description(std::string description_) {
        description = description_;
    }

    void AddSubcommand(Subcommand subcommand_) {
        subcommands.push_back(subcommand_);
    }

    void AddSubcommands(std::vector<Subcommand> subcommands_) {
        int len = subcommands_.size();
        for (int i = 0; i < len; i++) {
            subcommands.push_back(subcommands_[i]);
        }
    }

    void Information() {
        std::cout << "\033[32m" << name << "\033[0m" << " " << version << std::endl << std::endl;
        if (author != "") {
            std::cout << author << std::endl << std::endl;
        }
        if (description != "") {
            std::cout << description << std::endl << std::endl;
        }
        
        std::cout << "\033[36m" << "USAGE:" << "\033[0m" << std::endl;
        std::cout << "\t" << name <<  " [SUBCOMMANDS]" << std::endl << std::endl;

        std::cout << "\033[36m" << "SUBCOMMANDS:" << "\033[0m" << std::endl;  
        int len = subcommands.size();
        for (int i = 0; i < len; i++) {
            std::cout << "\t"; 
            subcommands[i].ShowHelp();
        }
        std::cout << "\t" << "\033[32m" << std::left << std::setw(16) << "help" << "\033[0m";
        std::cout << "show this message" << std::endl;
    }

    bool Valid(std::string cmd_) {
        if (cmd_ == "help") {
            return true;
        }
        for (auto cmd : subcommands) {
            if (cmd.GetName() == cmd_) {
                return true;
            }
        }
        return false;
    }

    Subcommand FindSubcommand(std::string cmd_) {
        for (std::vector<Subcommand>::iterator it = subcommands.begin(); it != subcommands.end(); it++) {
            if (it->GetName() == cmd_) {
                return *it;
            }
        }
        return Subcommand("");
    }

    void Error(std::string arg_) {
        std::cout << "\033[31merror\033[0m: Found argument '" << "\033[033m" << arg_ << "\033[0m' which is not expected, or isn't valid in this context" << std::endl << std::endl;  
        std::cout << "USAGE:" << std::endl;
        std::cout << "\t" << name << " [SUBCOMMANDS]" << std::endl << std::endl;
        std::cout << "For more information, try \033[32mhelp\033[0m" << std::endl;
    }

    // Handle subcommand and generate a subcommand send to kernel
    void Handler(Subcommand sc_) {
        // should not search value for non-exist flags
        Subcommand standard = FindSubcommand(sc_.GetName());
        std::vector<Flag> flags = standard.GetFlags();
        for (auto flag : flags) {
            if (flag.IsRequired() && !sc_.FindByKey(flag.GetKey())) {
                std::cout << "\033[031merror\033[0m: " << flag.GetKey() << " is required but not found" << std::endl << std::endl; 
                return;
            }
        }
        for (auto flag : sc_.GetFlags()) {
            if (standard.FindByLongName(flag.GetLongName()).NeedValue() && flag.GetValue() == "") {
                std::cout << "\033[031error\033[0m: " << flag.GetKey() << " need a value but not set" << std::endl << std::endl;
                return;
            }
        }
        if (sc_.GetName() == "ADD") {
            addPrivilege(sc_);
        } else if (sc_.GetName() == "UPDATE") {
            updatePrivilege(sc_);
        } else if (sc_.GetName() == "DELETE") {
            deletePrivilege(sc_);
        } else if (sc_.GetName() == "LIST") {
            listPrivilege();
        }
        return;
    }
};
