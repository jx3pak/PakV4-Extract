// PakV4_Extract.cpp : 剑网3 PakV4 解包程序
//

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>

struct IFile;
struct IFile_vtable;

typedef struct IFile* (G_OpenFile)(const char* a1, unsigned int a2, unsigned int a3);
typedef bool (G_IsFileExist)(const char* a1);
typedef int  (G_FileNameHash)(const char* a1);

typedef int (KG_InitPakV4FileSystem)(const char* a1, const char* a2, void* a3, int a4, int a5, int a6);

struct IFile_vtable {
    size_t(*Read)(struct IFile* __this, void* a1, size_t a2);
    void* f2;
    void* f3;
    void* f4;
    void* f5;
    size_t(*Size)(struct IFile* __this);
    void* f7;
    void* f8;
    void* f9;
    void* f10;
    void* f11;
    void* f12;
    void (*Release)(struct IFile* __this);
    void* f14;
};

struct IFile {
    struct IFile_vtable* _vtable;
    void* __others;
};


int main(int argc, char** argv)
{
    if (argc != 2)
    {

        std::cout << "put [V4XPack.exe] in [bin64] folder.\n";
        std::cout << "run V4XPack.exe with path-list.txt file\n";
        std::cout << "example: V4XPack.exe path.txt\n";
        return -1;
    }else{
        std::fstream fs;
        fs.open(argv[1], std::ios::in);
        if (!fs.is_open()) {
            std::cout << "bad path.txt\n";
            return -1;
        }

        HMODULE h = LoadLibraryA("Engine_Lua5X64.dll");
        if (h == NULL) {
            std::cout << "bad Engine_Lua5X64.dll\n";
            return -1;
        }

        G_OpenFile* g_OpenFile = (G_OpenFile*)GetProcAddress(h, "g_OpenFile");
        G_IsFileExist* g_IsFileExist = (G_IsFileExist*)GetProcAddress(h, "g_IsFileExist");
        G_FileNameHash* g_FileNameHash = (G_FileNameHash*)GetProcAddress(h, "g_FileNameHash");
        KG_InitPakV4FileSystem* g_InitPakV4FileSystem = (KG_InitPakV4FileSystem*)GetProcAddress(h, "KG_InitPakV4FileSystem");

        if (g_OpenFile == NULL || g_IsFileExist == NULL || g_FileNameHash == NULL || g_InitPakV4FileSystem == NULL) {
            std::cout << "bad Engine_Lua5X64.dll\n";
            return -1;
        }
        int v4 = g_InitPakV4FileSystem("../../../PakV4", "Trunk.Dir", 0, 0, 0, 0);
        if (!v4) {
            std::cout << "bad PakV4\n";
            return -1;
        }
        std::string line;
        while (std::getline(fs, line)) {
            const char* file = line.c_str();
            int x = g_FileNameHash(file);
            if (!g_IsFileExist(file)) {
                std::cout << line << ", hash=" << x << ", not exist" << "\n";
                continue;
            }
            struct IFile* f = g_OpenFile(file, 0, 0);
            if (f != NULL) {
                size_t sz = f->_vtable->Size(f);
                char* m = (char*)malloc(sz + 1);
                size_t sz2 = f->_vtable->Read(f, m, sz);
                f->_vtable->Release(f);
                std::fstream fs2;
                std::filesystem::path parent = std::filesystem::path(file).remove_filename();
                if (!std::filesystem::exists(parent)) {
                    std::filesystem::create_directories(parent);
                }
                fs2.open(file, std::ios::out|std::ios::binary);
                fs2.write(m, sz2);
                fs2.close();
                std::cout << line << ", hash=" << x << ", size=" << sz << "\n";
            }
            else {
                std::cout << line << ", hash=" << x << ", not found" << "\n";
            }
        }
        fs.close();
    }
    return 0;
}
