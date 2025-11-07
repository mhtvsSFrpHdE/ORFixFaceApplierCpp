#include "Patch.h"
#include <wx/textfile.h>
#include <wx/filename.h>
#include <vector>
#include <wx/file.h>

enum class IniFaceVersion {
    Unknown,
    Pre6_1,    // Before version 6.1
    OutdatedNews102425,    // Gamebanana news how to fix face, but outdated, will not work
    OutdatedNews102425_OrNnOnFace,    // Applied Fix on face (Shouldn't)
    AgmgPinned110725    // AGMG discord pinned message how to fix face, passed test
};

IniFaceVersion GuessIniFaceVersion(std::vector<wxString>& lines, size_t faceConfigGroupContentFirstLineNumber) {
    bool pre6_1 = false;
    bool outdatedNews102425 = false;
    bool outdatedNews102425_OrNnOnFace = false;
    bool agmgPinned110725 = false;
    for (size_t i = faceConfigGroupContentFirstLineNumber; i < lines.size(); i++) {
        auto line = lines[i];
        auto lowerLine = line.Lower();

        if (lowerLine.Contains("ps-t0") || lowerLine.Contains("ps-t1")) {
            pre6_1 = true;
        }
        if (lowerLine.Contains("this") && lowerLine.Contains("facehead")) {
            outdatedNews102425 = true;
        }
        if (lowerLine.Contains("run") && (lowerLine.Contains("orfix") || lowerLine.Contains("nnfix"))) {
            outdatedNews102425_OrNnOnFace = true;
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
    else if (outdatedNews102425_OrNnOnFace) {
        result = IniFaceVersion::OutdatedNews102425_OrNnOnFace;
    }
    else if (pre6_1) {
        result = IniFaceVersion::Pre6_1;
    }
    else if (agmgPinned110725) {
        result = IniFaceVersion::AgmgPinned110725;
    }
    return result;
}

void PatchPre6_1(std::vector<wxString>& lines, size_t faceConfigGroupContentFirstLineNumber) {
    std::cout << "Ini file version seems is Pre6.1, processing" << std::endl;

    // Patch exist line
    for (size_t i = faceConfigGroupContentFirstLineNumber; i < lines.size(); i++)
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
    for (size_t i = faceConfigGroupContentFirstLineNumber; i < lines.size(); i++)
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

size_t GetGroupKeyIndex(std::vector<wxString>& lines, size_t lineIndex) {
    for (int i = lineIndex; i >= 0; i--)
    {
        auto line = lines[i];
        if (line.StartsWith("[")) {
            return i;
        }
    }
}

bool LineIsInFaceConfigGroup(std::vector<wxString>& lines, size_t lineIndex) {
    auto line = lines[lineIndex];
    auto lowerLine = line.Lower();

    bool result = lowerLine.StartsWith("[") == false
        && lowerLine.Contains("resource") && lowerLine.Contains("faceheaddiffuse");
    return result;
}

void UndoOutdatedNews102425_OrNnOnFaceToPre6_1(std::vector<wxString>& lines, size_t faceConfigGroupContentFirstLineNumber) {
    std::cout << "Ini file version seems is OutdatedNews102425_OrNnOnFace" << std::endl
        << "Please open ini file manually" << std::endl
        << "Find config group \"[TextureOverrideDehyaFaceHeadDiffuse]\"" << std::endl
        << "Remove line looks like \"run = CommandList\\global\\ORFix\\ORFix\"" << std::endl
        << "or \"run = CommandList\\global\\ORFix\\NNFix\"" << std::endl
        << "If there is \"run = CommandListDehyaFace\"" << std::endl
        << "Find config group \"[CommandListDehyaFace]\" and clean redundant ORFix and NNFix lines" << std::endl
        << std::endl
        << "Failed to downgraded ini file version back to Pre6_1" << std::endl
        << "Press enter to exit" << std::endl;
    std::string ignore;
    std::getline(std::cin, ignore);
    exit(1);
}

void UndoOutdatedNews102425ToPre6_1(std::vector<wxString>& lines, size_t faceConfigGroupContentFirstLineNumber) {
    std::cout << "Ini file version seems is OutdatedNews102425, processing" << std::endl;

    for (size_t i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (LineIsInFaceConfigGroup(lines, i)) {
            bool isThis = lowerLine.Contains("this");
            if (isThis) {
                line.Replace("this", "ps-t0");
                lines[i] = line;
                std::cout << "Line " << i + 1 << " changed to: " << line << std::endl;
            }
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

    bool groupIsPatched = false;
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        auto lowerLine = line.Lower();
        if (LineIsInFaceConfigGroup(lines, i) && groupIsPatched == false) {
            auto groupKeyIndex = GetGroupKeyIndex(lines, i);
            std::cout << lines[groupKeyIndex] << std::endl;

            auto iniFaceVersion = GuessIniFaceVersion(lines, groupKeyIndex + 1);
            if (iniFaceVersion == IniFaceVersion::Pre6_1) {
                PatchPre6_1(lines, groupKeyIndex + 1);
                groupIsPatched = true;
            }
            if (iniFaceVersion == IniFaceVersion::OutdatedNews102425) {
                UndoOutdatedNews102425ToPre6_1(lines, groupKeyIndex + 1);
                i--;
                continue;
            }
            if (iniFaceVersion == IniFaceVersion::OutdatedNews102425_OrNnOnFace) {
                UndoOutdatedNews102425_OrNnOnFaceToPre6_1(lines, groupKeyIndex + 1);
                i--;
                continue;
            }
            if (iniFaceVersion == IniFaceVersion::AgmgPinned110725) {
                std::cout << "Ini file version seems is AgmgPinned110725, nothing to do" << std::endl;
            }
            if (iniFaceVersion == IniFaceVersion::Unknown) {
                std::cout << "Your ini status is unknown, please fix it manually" << std::endl
                    << "or maybe it doesn't need to be fixed" << std::endl
                    << "Face fix tutorial: https://github.com/mhtvsSFrpHdE/ORFixFaceApplierCpp/wiki/How-to-fix-faces" << std::endl;
            }
        }
        bool endFaceConfigGroup = lowerLine.StartsWith("[");
        if (endFaceConfigGroup) {
            groupIsPatched = false;
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
