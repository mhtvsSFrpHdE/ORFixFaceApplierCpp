#include "Patch.h"
#include <wx/textfile.h>
#include <wx/filename.h>
#include <vector>
#include <wx/file.h>

enum class IniFaceVersion {
    Unknown,
    Pre6_1,    // Before version 6.1
    OutdatedNews102425,    // Gamebanana news how to fix face, but outdated, will not work
    AgmgPinned110725    // AGMG discord pinned message how to fix face, passed test
};

IniFaceVersion GuessIniFaceVersion(std::vector<wxString>& lines, size_t faceConfigGroupBeginLineNumber) {
    bool pre6_1 = false;
    bool outdatedNews102425 = false;
    bool agmgPinned110725 = false;
    for (size_t i = faceConfigGroupBeginLineNumber; i < lines.size(); i++) {
        auto line = lines[i];
        auto lowerLine = line.Lower();

        if (lowerLine.Contains("ps-t0")) {
            pre6_1 = true;
        }
        if (lowerLine.Contains("this") && lowerLine.Contains("facehead")) {
            outdatedNews102425 = true;
        }
        if (lowerLine.Contains("resource\\gimi\\")) {
            agmgPinned110725 = true;
        }

        bool endFaceConfigGroup = lowerLine.StartsWith("[");
        if (endFaceConfigGroup) {
            break;
        }
    }

    IniFaceVersion result = IniFaceVersion::Unknown;
    if (outdatedNews102425) {
        result = IniFaceVersion::OutdatedNews102425;
    }
    else if (pre6_1) {
        result = IniFaceVersion::Pre6_1;
    }
    else if (agmgPinned110725) {
        result = IniFaceVersion::AgmgPinned110725;
    }
    return result;
}

void PatchPre6_1(std::vector<wxString>& lines, size_t faceConfigGroupBeginLineNumber) {
    std::cout << "Ini file version seems is Pre6.1, processing" << std::endl;

    // Patch exist line
    for (size_t i = faceConfigGroupBeginLineNumber; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();

        // Diffuse map
        if (lowerLine.Contains("ps-t0")) {
            auto splittedString = wxSplit(line, '=');
            wxString diffuseMapName = splittedString.Last().Trim().Trim(false);

            auto textBeginIndex = lowerLine.Find("ps-t0");
            auto newLineWithIndent = line.substr(0, textBeginIndex);
            newLineWithIndent = newLineWithIndent + "Resource\\GIMI\\Diffuse = ref " + diffuseMapName;
            lines[i] = newLineWithIndent;
            std::cout << "Line " << i + 1 << " changed to: " << newLineWithIndent << std::endl;
        }

        // Light map
        if (lowerLine.Contains("ps-t1")) {
            auto splittedString = wxSplit(line, '=');
            wxString lightMapName = splittedString.Last().Trim().Trim(false);

            auto textBeginIndex = lowerLine.Find("ps-t1");
            auto newLineWithIndent = line.substr(0, textBeginIndex);
            newLineWithIndent = newLineWithIndent + "Resource\\GIMI\\LightMap = ref " + lightMapName;
            lines[i] = newLineWithIndent;
            std::cout << "Line " << i + 1 << " changed to: " << newLineWithIndent << std::endl;
        }

        bool endFaceConfigGroup = lowerLine.StartsWith("[");
        if (endFaceConfigGroup) {
            break;
        }

    }

    // Insert new line
    bool isResourceSection = false;
    wxString newLineWithIndent = wxEmptyString;
    for (size_t i = faceConfigGroupBeginLineNumber; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (isResourceSection) {
            bool endResourceSection = lowerLine.Contains("resource\\gimi\\") == false;
            if (endResourceSection) {
                isResourceSection = false;

                auto newLine = newLineWithIndent + "run = CommandList\\GIMI\\SetTextures";
                lines.insert(lines.begin() + i, newLine);
                std::cout << "New line at " << i + 1 << ": " << newLine << std::endl;
            }
        }
        else {
            isResourceSection = lowerLine.Contains("resource\\gimi\\");
            auto textBeginIndex = lowerLine.Find("resource\\gimi\\");
            newLineWithIndent = line.substr(0, textBeginIndex);
        }
    }
}

bool IsFaceConfigGroup(wxString lowerLine) {
    // [TextureOverrideMizukiFaceHeadDiffuse]
    // Standard syntax
    bool result = lowerLine.StartsWith("[textureoverride") && lowerLine.EndsWith("faceheaddiffuse]");
    // [CommandListRaidenShogunFaceHeadDiffuse]
    // Some weebovz mods use this config group
    result = result || (lowerLine.StartsWith("[commandlist") && lowerLine.EndsWith("faceheaddiffuse]"));

    if (lowerLine.StartsWith("[commandlist") && lowerLine.EndsWith("faceheaddiffuse]")) {
        __debugbreak;
    }

    return result;
}

void UndoOutdatedNews102425ToPre6_1(std::vector<wxString>& lines, size_t faceConfigGroupBeginLineNumber) {
    std::cout << "Ini file version seems is OutdatedNews102425, processing" << std::endl;

    bool isFaceConfigGroup = false;
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (isFaceConfigGroup) {
            bool isPsT0 = lowerLine.Contains("this");
            if (isPsT0) {
                line.Replace("this", "ps-t0");
                lines[i] = line;
                std::cout << "Line " << i + 1 << " changed to: " << line << std::endl;
            }
            bool endFaceConfigGroup = lowerLine.StartsWith("[");
            if (endFaceConfigGroup) {
                isFaceConfigGroup = false;
            }
        }
        else {
            isFaceConfigGroup = IsFaceConfigGroup(lowerLine);
        }
    }

    std::cout << "Downgraded ini file version back to Pre6_1" << std::endl;
}

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
    bool groupIsPatched = false;
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (isFaceConfigGroup && groupIsPatched == false) {
            auto iniFaceVersion = GuessIniFaceVersion(lines, i);
            if (iniFaceVersion == IniFaceVersion::Pre6_1) {
                PatchPre6_1(lines, i);
                groupIsPatched = true;
            }
            if (iniFaceVersion == IniFaceVersion::OutdatedNews102425) {
                UndoOutdatedNews102425ToPre6_1(lines, i);
                PatchPre6_1(lines, i);
                groupIsPatched = true;
            }
            if (iniFaceVersion == IniFaceVersion::AgmgPinned110725) {
                std::cout << "Ini file version seems is AgmgPinned110725, nothing to do" << std::endl;
            }
            if (iniFaceVersion == IniFaceVersion::Unknown) {
                std::cout << "Your ini status is unknown, please fix it manually" << std::endl
                    << "or maybe it doesn't need to be fixed" << std::endl
                    << "Face fix tutorial: https://github.com/mhtvsSFrpHdE/ORFixFaceApplierCpp/wiki/How-to-fix-faces" << std::endl;
            }

            bool endFaceConfigGroup = lowerLine.StartsWith("[");
            if (endFaceConfigGroup) {
                isFaceConfigGroup = false;
            }
        }
        else {
            isFaceConfigGroup = IsFaceConfigGroup(lowerLine);
            if (isFaceConfigGroup) {
                groupIsPatched = false;
                std::cout << line << std::endl;
            }
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
