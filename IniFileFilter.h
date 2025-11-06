#pragma once
#include <wx/dir.h>

/// <summary>
/// get the names of all files in the array
///     wxArrayString files;
///     IniFileFilter iff(files);
///     wxDir dir(dirname);
///     dir.Traverse(iff);
/// </summary>
class IniFileFilter : public wxDirTraverser
{
public:
    wxDirTraverseResult OnFile(const wxString& filename) override
    {
        if (filename.Lower().EndsWith(".ini")) {
            iniFiles.Add(filename);
        }
        return wxDIR_CONTINUE;
    }

    wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) override
    {
        return wxDIR_CONTINUE;
    }

    wxArrayString iniFiles;
};
