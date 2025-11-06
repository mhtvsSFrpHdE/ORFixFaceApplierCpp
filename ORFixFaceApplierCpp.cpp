#include <iostream>
#include <wx/dir.h>
#include "IniFileFilter.h"
#include "Patch.h"

int main()
{
    Patch::cwd = wxGetCwd();
    wxDir workingDirectory(Patch::cwd);
    if (!workingDirectory.IsOpened())
    {
        return 1;
    }
    IniFileFilter iniFileFilter;
    workingDirectory.Traverse(iniFileFilter);
    for (auto& iniFile : iniFileFilter.iniFiles) {
        Patch::MakeBackup(iniFile);
        Patch::PatchORFix_6_1(iniFile);
    }

    std::cout << "Done, press enter to exit" << std::endl;
    std::string ignore;
    std::getline(std::cin, ignore);
}
