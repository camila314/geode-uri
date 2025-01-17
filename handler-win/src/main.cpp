#include <geode-ipc.hpp>
#include <Windows.h>
#include <stdlib.h>
#include <format>
#include <filesystem>
#include <fstream>
#include <shlwapi.h>

std::string wideToUtf8(std::wstring const& wstr) {
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring utf8ToWide(std::string const& str) {
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (__argc != 2)
        return 1;

    std::string urlString = __argv[1];

    if (urlString.find("geode://") == 0) {
        urlString = urlString.substr(8);
    } else if (urlString.find("geode:") == 0) {
        urlString = urlString.substr(6);
    } else {
        return 1;
    }

    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::filesystem::path exeDir = std::filesystem::path(path).parent_path();

    if (!GeodeIPC::send("geode.loader", "ipc-test", {})) {
        auto strPath = exeDir / ".gd-loc";
        if (std::filesystem::exists(strPath)) {
            std::ifstream file(strPath);
            std::string gdPath_;
            std::getline(file, gdPath_);
            file.close();
            if (gdPath_.empty()) return 1;

            auto gdPath = std::filesystem::path(gdPath_);
            auto gdDir = gdPath.parent_path();

            char urlUnescaped[MAX_PATH] = {0};
            char urlEscaped[MAX_PATH] = {0};
            UrlUnescapeA((char*)urlString.c_str(), urlUnescaped, nullptr, NULL);
            UrlEscapeA(urlUnescaped, urlEscaped, nullptr, NULL);
            urlString = urlEscaped;

            std::filesystem::current_path(gdDir);
            ShellExecuteW(NULL, L"open", gdPath.wstring().c_str(), utf8ToWide(std::format("--geode:url-path={}", urlString)).c_str(), gdDir.wstring().c_str(), TRUE);
            return 0;
        }

        MessageBoxA(NULL, "Unable to find Geometry Dash.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (!GeodeIPC::send("camila314.custom-uri", "handle", urlString)) {
        MessageBoxA(NULL, "Failed to communicate.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
