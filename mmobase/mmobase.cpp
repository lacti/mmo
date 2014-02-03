// mmobase.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mmobase.h"
#include <iostream>
#include <strsafe.h>
#include <DbgHelp.h>

// This is an example of an exported variable
MMOBASE_API int nmmobase=0;

// This is an example of an exported function.
MMOBASE_API int fnmmobase(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see mmobase.h for the class definition
Cmmobase::Cmmobase()
{
    return;
}


////////////////////////////////////////////////////////////////////////////////////////

static char gModulePath[MAX_PATH] = {0,};

void MmoSetModulePath(const char* pszPath)
{
    StringCbCopyA(gModulePath, sizeof(gModulePath), pszPath);
}

BOOL CALLBACK EnumerateLoadedModulesProc( PSTR ModuleName,
                                         ULONG ModuleBase,
                                         ULONG ModuleSize,
                                         PVOID UserContext )
{
    size_t pos = 0;
    std::string str = ModuleName;
    while((pos = str.find("mmobase.dll", pos)) != std::string::npos)
    {
        StringCbCopyA(gModulePath, sizeof(gModulePath), ModuleName);
        break;
    }

    return TRUE;
}

static boost::filesystem::path MmoGetWorkingPath()
{
    using namespace std;

#ifdef WIN32
    char strExePath [MAX_PATH];
    if (gModulePath[0] != TEXT('\0'))
    {
        StringCbCopyA(strExePath, sizeof(strExePath), gModulePath);
    }
    else
    {
        EnumerateLoadedModules(GetCurrentProcess(), (PENUMLOADED_MODULES_CALLBACK) EnumerateLoadedModulesProc, 0);
        if (gModulePath[0] != TEXT('\0'))
        {
            StringCbCopyA(strExePath, sizeof(strExePath), gModulePath);
        }
        else
        {
            GetModuleFileNameA(NULL, strExePath, MAX_PATH);
        }
    }

    cout << "Module File Name : " << strExePath << endl;
#else
    std::string strExePath = "";
    pid_t pid = getpid();
    char buf[10];
    sprintf(buf,"%d",pid);
    std::string _link = "/proc/";
    _link.append( buf );
    _link.append( "/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != -1) { 
        proc[ch] = 0; 
        strExePath = proc; 
        std::string::size_type t = strExePath.find_last_of("/");
        strExePath = strExePath.substr(0,t);
    }
#endif

    boost::filesystem::path p (strExePath);
    //boost::filesystem::path p("E:\\mmo4\\");
    p = p.remove_filename();
    while(!p.empty() && !boost::filesystem::is_directory(p / "resources")) {
        p = p.parent_path();
    }

    return p;
}

static bool MmoIsStringEndWith(const char* c, const char* end_with, bool case_insensitive = true)
{
    size_t end_with_len = strlen(end_with);
    size_t len = strlen(c);

    if (len >= end_with_len)
    {
        if (case_insensitive)
        {
            if (_stricmp(c + (len - end_with_len), end_with) == 0)
                return true;
        }
        else
        {
            if (strcmp(c + (len - end_with_len), end_with) == 0)
                return true;
        }
    }

    return false;
}

MMOBASE_API int mmo_resolve_resource_path(char* out_path, int out_path_max_len, const char* name)
{
    if (!out_path)
        return -1;
    if (out_path_max_len <= 0)
        return -2;
    if (!name)
        return -3;

    boost::filesystem::path name_as_path(name);
    std::string resource_name = name_as_path.filename().generic_string();

    static boost::filesystem::path WORKING_PATH(MmoGetWorkingPath());

    if (WORKING_PATH.empty() || !boost::filesystem::is_directory(WORKING_PATH / "resources"))
    {
        std::cerr << "[CRITICAL ERROR] Resources path cannot be found in the current working path.\n";
        return -4;
    }

    const char* resource_dir = nullptr;

    if (MmoIsStringEndWith(name, ".vert") || MmoIsStringEndWith(name, ".frag"))
    {
        resource_dir = "shaders";
    }
    else if (MmoIsStringEndWith(name, ".png"))
    {
        resource_dir = "images";
    }
    else if (MmoIsStringEndWith(name, ".wav"))
    {
        resource_dir = "audio";
    }
    else if (MmoIsStringEndWith(name, ".mp3"))
    {
        resource_dir = "audio";
    }
    else if (MmoIsStringEndWith(name, ".ttf") || MmoIsStringEndWith(name, ".otf"))
    {
        resource_dir = "fonts";
    }
    else if (MmoIsStringEndWith(name, ".xml"))
    {
        resource_dir = "data";
    }
    else if (MmoIsStringEndWith(name, ".obj") || MmoIsStringEndWith(name, ".fbx"))
    {
        resource_dir = "models";
    }
    else if (MmoIsStringEndWith(name, ".dds"))
    {
        resource_dir = "textures";
    }
    else if (MmoIsStringEndWith(name, ".rml"))
    {
        resource_dir = "rocket";
    }

    if (resource_dir)
    {
        strncpy(out_path, (WORKING_PATH / "resources" / resource_dir / resource_name).generic_string().c_str(), out_path_max_len);
        return 0;
    }

    return -5;
}

resource_path_resolver::resource_path_resolver(const char* name)
{
    mmo_resolve_resource_path(resource_path_buf, sizeof(resource_path_buf), name);
};

void GetMmoResourcePath(char* path, int maxpath, const char* name)
{
    StringCbCopyA(path, maxpath, resource_path_resolver(name).get());
}
