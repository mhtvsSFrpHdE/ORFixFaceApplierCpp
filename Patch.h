#pragma once
#include <wx/wx.h>
#include "ReturnValue.h"

class Patch
{
public:
    static wxString cwd;
    static ReturnValue<> PatchORFix_6_1(wxString iniFileName);
    static ReturnValue<> MakeBackup(wxString iniFileName);
};
