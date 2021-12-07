#include <iostream>
#include <vector>
#include <unistd.h>

#include "command.h"

int main(int argc, char** argv) {
    // check uid, for only root has the privilege to run firmiana
    int uid = getuid();
    if (uid != 0) {
        std::cout << "\033[031merror:\033[0m firmiana should be run as root!" << std::endl << std::endl;
        return 1;
    }
    // set basic information
    Firmiana firmiana("Firmiana");
    firmiana.Version("0.1.0");
    firmiana.Author("Snowfall99");
    firmiana.Description("\t\033[35mProject for IS415 group 2-1\033[0m\n \
\t\033[33mLinux Kernel Version:\033[0m 5.4.0-73-generic\n \
\t\033[33mPlatform\033[0m: x86_64, Aliyun");

    // set flags
    Flag exeFlag("exe");
    exeFlag.SetLongName("exe");
    exeFlag.SetShortName("e");
    exeFlag.SetDescription("exe filename");
    exeFlag.SetIsRequired(true);
    exeFlag.SetNeedValue(true);

    Flag typeFlag("type");
    typeFlag.SetLongName("type");
    typeFlag.SetShortName("t");
    typeFlag.SetDescription("type of file");
    typeFlag.SetNeedValue(true);

    Flag writeFlag("write");
    writeFlag.SetLongName("write");
    writeFlag.SetShortName("w");
    writeFlag.SetDescription("write privilege, default 0");

    Flag readFlag("read");
    readFlag.SetLongName("read");
    readFlag.SetShortName("r");
    readFlag.SetDescription("read privilege, default 0");

    Flag openFlag("open");
    openFlag.SetLongName("open");
    openFlag.SetShortName("o");
    openFlag.SetDescription("open privilege, default 0");

    Flag rmdirFlag("rmdir");
    rmdirFlag.SetLongName("rmdir");
    rmdirFlag.SetDescription("rmidr privilege, default 0");

    Flag mkdirFlag("mkdir");
    mkdirFlag.SetLongName("mkdir");
    mkdirFlag.SetShortName("mk");
    mkdirFlag.SetDescription("mkdir privilege, default 0");

    Flag chmodFlag("chmod");
    chmodFlag.SetLongName("chmod");
    chmodFlag.SetShortName("ch");
    chmodFlag.SetDescription("chmod privilege, default 0");

    Flag creatFlag("creat");
    creatFlag.SetLongName("creat");
    creatFlag.SetShortName("c");
    creatFlag.SetDescription("creat privilege, default 0");

    // set subcommands
    std::vector<Subcommand> subcommands;
    Subcommand addCmd("ADD");
    addCmd.SetDescription("add privilege");
    addCmd.AddFlag(exeFlag);
    addCmd.AddFlag(typeFlag);
    addCmd.AddFlag(writeFlag);
    addCmd.AddFlag(readFlag);
    addCmd.AddFlag(openFlag);
    addCmd.AddFlag(creatFlag);
    addCmd.AddFlag(chmodFlag);
    addCmd.AddFlag(rmdirFlag);
    addCmd.AddFlag(mkdirFlag);
    subcommands.push_back(addCmd);

    Subcommand updateCmd("UPDATE");
    updateCmd.SetDescription("update privilege");
    updateCmd.AddFlag(exeFlag);
    updateCmd.AddFlag(typeFlag);
    updateCmd.AddFlag(writeFlag);
    updateCmd.AddFlag(readFlag);
    updateCmd.AddFlag(openFlag);
    updateCmd.AddFlag(chmodFlag);
    updateCmd.AddFlag(creatFlag);
    updateCmd.AddFlag(mkdirFlag);
    updateCmd.AddFlag(rmdirFlag);
    subcommands.push_back(updateCmd);

    Subcommand deleteCmd("DELETE");
    deleteCmd.SetDescription("delete privilege");
    deleteCmd.AddFlag(exeFlag);
    deleteCmd.AddFlag(typeFlag);
    deleteCmd.AddFlag(writeFlag);
    deleteCmd.AddFlag(readFlag);
    deleteCmd.AddFlag(openFlag);
    deleteCmd.AddFlag(chmodFlag);
    deleteCmd.AddFlag(creatFlag);
    deleteCmd.AddFlag(mkdirFlag);
    deleteCmd.AddFlag(rmdirFlag);
    subcommands.push_back(deleteCmd);

    Subcommand listCmd("LIST");
    listCmd.SetDescription("list all privileges");
    subcommands.push_back(listCmd);

    Subcommand infoCmd("INFO");
    infoCmd.SetDescription("show firmiana info");
    subcommands.push_back(infoCmd);

    firmiana.AddSubcommands(subcommands);

    // print information, if no subcommand is provided
    if (argc == 1) {
        firmiana.Information();
        return 0;
    }
    // check validity of subcommand
    std::string subcommand(argv[1]);
    if (!firmiana.Valid(subcommand)) {
        firmiana.Error(subcommand);
        return 1;
    }
    if (subcommand == "help") {
        firmiana.Information();
        return 0;
    }
    // get flags
    Subcommand model = firmiana.FindSubcommand(subcommand);
    Subcommand sc(model.GetName());
    sc.SetDescription(model.GetDescription());
    for (int i = 2; i < argc; ) { 
        std::string _flag(argv[i]);
        Flag flag("");
        if (_flag[0] == '-' && _flag[1] == '-') {
            if (!model.ValidLongName(_flag.substr(2))) {
                firmiana.Error(_flag.substr(2));
                return 1;
            }
            flag = model.FindByLongName(_flag.substr(2));
        } else if (_flag[0] == '-') {
            if (!model.ValidShortName(_flag.substr(1))) {
                firmiana.Error(_flag.substr(1));
                return 1;
            }
            flag = model.FindByShortName(_flag.substr(1));
        } else {
            firmiana.Error(_flag);
            return 1;
        }
        if (i+1 < argc && argv[i+1][0] != '-') {
            i ++;
            flag.SetValue(argv[i]);
        }
        sc.AddFlag(flag);
        i ++;
    }
    firmiana.Handler(sc);
    return 0;
}
