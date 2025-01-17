#include <Geode/Geode.hpp>
#include <Geode/loader/IPC.hpp>
#include <GeodeURLHandler.h>
#include <sys/stat.h>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
using namespace ipc;

#if defined(GEODE_IS_MACOS)
#define CommentType CommentType2
#include <Cocoa/Cocoa.h>
#undef CommentType

void platformSetup() {
    auto saveDir = Mod::get()->getSaveDir();
    auto appDir = saveDir / "Geode URL Handler.app";

    std::string gdPath = [[NSBundle mainBundle] bundlePath].UTF8String;
    if (auto err = file::writeString(Mod::get()->getSaveDir() / ".gd-loc", gdPath).err()) {
        log::error("Failed to write gd path: {}", err);
        return;
    } 

    if (!std::filesystem::exists(appDir)) {
        auto zipPath = Mod::get()->getTempDir() / "package.zip";
        std::vector<uint8_t> zipData = std::vector<uint8_t>(GeodeURLHandler, GeodeURLHandler + GeodeURLHandler_len);

        auto zipFile = file::Unzip::create(zipData);
        if (auto err = zipFile.err()) {
            log::error("Failed to create zip file: {}", err);
            return;
        }

        if (auto err = zipFile.unwrap().extractAllTo(saveDir).err()) {
            log::error("Failed to extract zip file: {}", err);
            return;
        }

        auto execDir = appDir / "Contents" / "MacOS" / "Geode URL Handler";
        if (!std::filesystem::exists(execDir)) {
            log::error("Failed to find file: {}", execDir);
            return;
        }

        if (chmod(execDir.c_str(), 0777) == -1) {
            log::error("Failed to chmod file: {}", execDir);
            return;
        }
    }
}

void bringToFront() {
    [NSApp activateIgnoringOtherApps:YES];
    [[NSApp mainWindow] makeKeyAndOrderFront:nil];
}
#elif defined(GEODE_IS_WINDOWS)
#include <windows.h>
#include <codecvt>

constexpr HKEY HNULL = (HKEY)NULL;

std::string from_wstring(std::wstring const& x) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x);
}

HKEY createKey(HKEY parent, char const* name) {
    if (!parent)
        return HNULL;

    HKEY hKey;
    if (RegCreateKeyExA(parent, name, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return HNULL;

    return hKey;
}

bool setKeyValue(HKEY key, char const* name, char const* value) {
    return RegSetValueExA(key, name, 0, REG_SZ, (const BYTE*)value, strlen(value)) == ERROR_SUCCESS;
}

void platformSetup() {
    auto exePath = string::wideToUtf8(Mod::get()->getSaveDir() / "GeodeURLHandler.exe");

    auto exeData = std::vector<uint8_t>(GeodeURLHandler, GeodeURLHandler + sizeof(GeodeURLHandler));
    if (auto err = file::writeBinary(exePath, exeData).err()) {
        log::error("Failed to write exe file: {}", err);
        return;
    }

    char gdPath[MAX_PATH];
    GetModuleFileNameA(NULL, gdPath, MAX_PATH);

    if (auto err = file::writeString(Mod::get()->getSaveDir() / ".gd-loc", gdPath).err()) {
        log::error("Failed to write gd path: {}", err);
        return;
    } 

    HKEY geodeKey = createKey(HKEY_CURRENT_USER, "Software\\Classes\\geode");
    HKEY iconKey = createKey(geodeKey, "DefaultIcon");
    HKEY shellKey = createKey(geodeKey, "shell");
    HKEY openKey = createKey(shellKey, "open");
    HKEY commandKey = createKey(openKey, "command");

    bool success = setKeyValue(geodeKey, "URL Protocol", "")
                && setKeyValue(iconKey, nullptr, "GeodeURLHandler.exe,1")
                && setKeyValue(commandKey, nullptr, fmt::format("\"{}\" \"%1\"", exePath).c_str());

    if (!success) {
        log::error("Failed to set registry keys");
        if (geodeKey != HNULL) {
            RegDeleteTree(HKEY_CLASSES_ROOT, "geode");
        }
    }

    if (geodeKey) RegCloseKey(geodeKey);
    if (iconKey) RegCloseKey(iconKey);
    if (shellKey) RegCloseKey(shellKey);
    if (openKey) RegCloseKey(openKey);
}

void bringToFront() {
    HWND hwnd = WindowFromDC(wglGetCurrentDC());
    if (hwnd) {
        ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
    }
}
#endif

std::string percent_decode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.size()) {
                int value;
                std::istringstream iss(str.substr(i + 1, 2));
                iss >> std::hex >> value;
                result += static_cast<char>(value);
                i += 2;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

$on_mod(Loaded) {
    platformSetup();

    if (auto pathFlag = Loader::get()->getLaunchArgument("url-path")) {
        auto path = percent_decode(pathFlag.value());
        log::info("WOW a url!! {}", path);
        bringToFront();
    }

    listen("handle", [](IPCEvent* ev) -> matjson::Value {
        if (auto str = ev->messageData->asString()) {
            log::info("WOW a url!! {}", str.unwrap());
            bringToFront();
        } else {
            log::error("Invalid IPC Message: {}", ev->messageData);
        }
        return {};
    });
}
