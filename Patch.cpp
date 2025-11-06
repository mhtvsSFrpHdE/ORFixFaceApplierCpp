#include "Patch.h"
#include <wx/textfile.h>
#include <wx/filename.h>
#include <vector>
#include <wx/file.h>

ReturnValue<> Patch::PatchORFix_6_1(wxString iniFileName) {
    ReturnValue<> result;
    wxFileName basePath(iniFileName);
    basePath.MakeRelativeTo(Patch::cwd);
    std::cout << "Processing " << basePath.GetFullPath() << std::endl;

    wxTextFile iniFile;
    iniFile.Open(iniFileName);

    std::vector<wxString> lines;
    for (auto line = iniFile.GetFirstLine(); !iniFile.Eof(); line = iniFile.GetNextLine())
    {
        lines.push_back(line);
    }

    bool isFaceConfigGroup = false;
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (isFaceConfigGroup) {
            bool isPsT0 = lowerLine.StartsWith("ps-t0");
            if (isPsT0) {
                line.Replace("ps-t0", "this");
                lines[i] = line;
                std::cout << "Line " << i + 1 << " changed to: " << line << std::endl;
            }
            bool endFaceConfigGroup = lowerLine.StartsWith("[");
            if (endFaceConfigGroup) {
                isFaceConfigGroup = false;
            }
        }
        else {
            isFaceConfigGroup = lowerLine.StartsWith("[textureoverride") && lowerLine.EndsWith("faceheaddiffuse]");
            if (isFaceConfigGroup) { std::cout << line << std::endl; }
        }
    }

    iniFile.Clear();
    for (size_t i = 0; i < lines.size(); i++)
    {
        iniFile.AddLine(lines[i]);
    }
    iniFile.Write();

    std::cout << "Saved" << std::endl << std::endl;
    result.success = true;
    return result;
}

ReturnValue<> Patch::MakeBackup(wxString iniFileName)
{
    ReturnValue<> result;

    auto backupFileName = iniFileName + ".ORFixApplierCpp.bak.txt";
    if (wxCopyFile(iniFileName, backupFileName, true))
    {
        result.success = true;
    }
    return result;
}

wxString Patch::cwd;
