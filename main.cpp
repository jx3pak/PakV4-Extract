// PakV4-Extract.cpp : 剑网3 PakV4 解包程序
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <psapi.h>
#ifdef _WIN64
#define ENGINE "Engine_Lua5X64.dll"
#define V2NAME "?KGPack_LoadPackageFiles@@YAHQEBDW4KGPACK_FILE_PRIORITY@@W4KGPACK_MANAGER_TYPE@@@Z"
#define V3NAME "?KGInitPackFileV3@@YAHPEB_W0W4KGPACK_FILE_V3_READ_PRIORITY@@@Z"
#define R_PATH "../"
#else
#define ENGINE "Engine_Lua5.dll"
#define V2NAME "?KGPack_LoadPackageFiles@@YAHQBDW4KGPACK_FILE_PRIORITY@@W4KGPACK_MANAGER_TYPE@@@Z"
#define V3NAME "?KGInitPackFileV3@@YAHPB_W0W4KGPACK_FILE_V3_READ_PRIORITY@@@Z"
#define R_PATH ""
#endif
class IFile;
typedef class IFile* (G_OpenFile)(const char* a1, unsigned int a2, unsigned int a3);
typedef bool (G_IsFileExist)(const char* a1);
typedef int  (G_FileNameHash)(const char* a1);
typedef void  (G_SetFilePath)(const char* a1);
typedef void  (G_SetPackageVersion)(int a1);
typedef int (INITV1)(const char* a1, const char* a2);
typedef int (INITV2)(const char* a1, int a2, int a3);
typedef int (INITV3)(const wchar_t* a1, const wchar_t* a2, int a3);
typedef int (INITV4)(const char* a1, const char* a2, void* a3, int a4, int a5, int a6);
typedef char* __cdecl G_setlocale(int Category, const char* Locale);
class IFile
{
public:
	virtual unsigned long	Read(void* Buffer, unsigned long ReadBytes) = 0;
	virtual unsigned long	Write(const void* Buffer, unsigned long WriteBytes) = 0;
	virtual void*	GetBuffer() = 0;
	virtual long	Seek(long Offset, int Origin) = 0;
	virtual long	Tell() = 0;
	virtual unsigned long	Size() = 0;
	virtual int		IsFileInPak() = 0;
	virtual int		IsPackedByFragment() = 0;
	virtual int		GetFragmentCount() = 0;
	virtual unsigned int	GetFragmentSize(int nFragmentIndex) = 0;
	virtual unsigned long	ReadFragment(int nFragmentIndex, void*& pBuffer) = 0;
	virtual void	Close() = 0;
	virtual void	Release() = 0;
	virtual ~IFile() {};
};
void setlocal936() {
	HMODULE hMods[4096];
	DWORD cbNeeded;
	if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			G_setlocale* __setlocale = (G_setlocale*)GetProcAddress(hMods[i], "setlocale");
			if (__setlocale) {
				__setlocale(0, "chinese_china.936");
			}
		}
	}
}
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "KS PakV1234 Extracter\n";
		std::cout << "                               by Me at 2022.10.10\n";
		std::cout << "usage:\n";
		std::cout << "  1. put \"PakV1234-Extract.exe\" in \"bin64\" folder.\n";
		std::cout << "  2. drop \"path-list.txt\" on \"PakV1234-Extract.exe\".\n";
		std::cout << "example: PakV1234-Extract.exe path.txt\n";
		std::cout << "comment: build x86 for PakV123, build x64 for Pak1234\n";
		return -1;
	}
	else {
		std::fstream fs;
		fs.open(argv[1], std::ios::in);
		if (!fs.is_open()) {
			std::cout << "bad path.txt\n";
			return -1;
		}
		HMODULE h = LoadLibraryA(ENGINE);
		if (h == NULL) {
			std::cout << "bad Engine_Lua5X64.dll\n";
			return -1;
		}
		G_OpenFile* g_OpenFile = (G_OpenFile*)GetProcAddress(h, "g_OpenFile");
		G_IsFileExist* g_IsFileExist = (G_IsFileExist*)GetProcAddress(h, "g_IsFileExist");
		G_FileNameHash* g_FileNameHash = (G_FileNameHash*)GetProcAddress(h, "g_FileNameHash");
		G_SetFilePath* g_SetRootPath = (G_SetFilePath*)GetProcAddress(h, "g_SetRootPath");
		G_SetFilePath* g_SetFilePath = (G_SetFilePath*)GetProcAddress(h, "g_SetFilePath");
		G_SetPackageVersion* g_SetPackageVersion = (G_SetPackageVersion*)GetProcAddress(h, "g_SetPackageVersion");
		INITV1* initV1 = (INITV1*)GetProcAddress(h, "g_LoadPackageFiles");
		INITV2* initV2 = (INITV2*)GetProcAddress(h, V2NAME);
		INITV3* initV3 = (INITV3*)GetProcAddress(h, V3NAME);
		INITV4* initV4 = (INITV4*)GetProcAddress(h, "KG_InitPakV4FileSystem");
		char module[4096];
		GetModuleFileNameA(NULL, module, 4096);
		std::filesystem::current_path(std::filesystem::path(module).remove_filename().string() + R_PATH);
		std::filesystem::path extract = std::filesystem::path(module).replace_extension("extract");
		if (std::filesystem::exists("../../PakV4/Trunk.Dir")) {
			if (g_OpenFile == NULL || g_IsFileExist == NULL || g_FileNameHash == NULL || initV4 == NULL) {
				std::cout << "bad Engine_Lua5X64.dll\n";
				return -1;
			}
			int v4 = initV4("../../PakV4", "Trunk.Dir", 0, 0, 0, 0);
			if (!v4) {
				std::cout << "bad PakV4\n";
				return -1;
			}
		}
		else if (std::filesystem::exists("../../PakV3/Trunk.Dir")) {
			if (g_OpenFile == NULL || g_IsFileExist == NULL || g_FileNameHash == NULL || initV3 == NULL) {
				std::cout << "bad Engine_Lua5X64.dll\n";
				return -1;
			}
			int v3 = initV3(L"../../PakV3", L"Trunk.Dir", 0);
			if (!v3) {
				std::cout << "bad PakV3\n";
				return -1;
			}
		}
		else if (std::filesystem::exists("NewPackageList.txt")) {
			if (g_OpenFile == NULL || g_IsFileExist == NULL || g_FileNameHash == NULL || initV2 == NULL || g_SetRootPath == NULL || g_SetFilePath == NULL || g_SetPackageVersion == NULL) {
				std::cout << "bad Engine_Lua5X64.dll\n";
				return -1;
			}
			g_SetRootPath(NULL);
			g_SetFilePath("");
			g_SetPackageVersion(1);
			int v2 = initV2("NewPackageList.txt", 2, 1);
			if (!v2) {
				std::cout << "bad PakV2\n";
				return -1;
			}
		}
		else if (std::filesystem::exists("package.ini")) {
			if (g_OpenFile == NULL || g_IsFileExist == NULL || g_FileNameHash == NULL || initV1 == NULL || g_SetRootPath == NULL || g_SetFilePath == NULL) {
				std::cout << "bad Engine_Lua5X64.dll\n";
				return -1;
			}
			g_SetRootPath(NULL);
			g_SetFilePath("");
			int v1 = initV1("package.ini", "SO3Client");
			if (!v1) {
				std::cout << "bad PakV1\n";
				return -1;
			}
		}
		else {
			std::cout << "bad PakV???\n";
			return -1;
		}
		setlocal936();
		std::string line;
		while (std::getline(fs, line)) {
			const char* file = line.c_str();
			unsigned int x = g_FileNameHash(file);
			if (!g_IsFileExist(file)) {
				std::cout << line << ", hash=" << x << ", not exist" << "\n";
				continue;
			}
			std::filesystem::path output = extract / file;
			if (std::filesystem::exists(output)) {
				std::cout << line << ", hash=" << x << ", skip exist" << "\n";
				continue;
			}
			class IFile* f = g_OpenFile(file, 0, 0);
			if (f != NULL) {
				unsigned long sz = f->Size();
				char* m = (char*)malloc(sz + 1);
				unsigned long sz2 = f->Read(m, sz);
				f->Release();
				std::fstream fs2;
				std::filesystem::path parent = std::filesystem::path(output).remove_filename();
				if (!std::filesystem::exists(parent)) {
					std::filesystem::create_directories(parent);
				}
				fs2.open(output, std::ios::out | std::ios::binary);
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
