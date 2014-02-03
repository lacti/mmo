// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MMOBASE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MMOBASE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MMOBASE_EXPORTS
#define MMOBASE_API __declspec(dllexport)
#else
#define MMOBASE_API __declspec(dllimport)
#endif

#ifdef __cplusplus

// This class is exported from the mmobase.dll
class MMOBASE_API Cmmobase {
public:
    Cmmobase(void);
    // TODO: add your methods here.
};

extern MMOBASE_API int nmmobase;

MMOBASE_API int fnmmobase(void);

class MMOBASE_API resource_path_resolver
{
public:
    explicit resource_path_resolver(const char* name);
    const char* get() const { return resource_path_buf; }

private:
    resource_path_resolver();
    char resource_path_buf[260]; // MAX_PATH
};

#define user_defined_path_resolver(name) MMORES(name)
#define MMORES(name) resource_path_resolver(name).get()

#endif // #ifdef __cplusplus

#ifdef __cplusplus
extern "C" {
#endif
    MMOBASE_API void GetMmoResourcePath(char* path, int maxpath, const char* name);
    MMOBASE_API void MmoSetModulePath(const char* pszPath);
#ifdef __cplusplus
}
#endif
