#include <Geode/Geode.hpp>
#include <Geode/loader/IPC.hpp>
#include <GeodeURLHandler.h>
#include <sys/stat.h>

using namespace geode::prelude;
using namespace ipc;

#if defined(GEODE_IS_MACOS)
void platformSetup() {
    auto saveDir = Mod::get()->getSaveDir();
    auto appDir = saveDir / "Geode URL Handler.app";

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
#elif defined(GEODE_IS_WINDOWS)
#include <windows.h>

constexpr HKEY HNULL = (HKEY)NULL;

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
    auto iconPath = (Mod::get()->getResourcesDir() / "geode.ico").string() + ",1";
    auto exePath = Mod::get()->getSaveDir() / "GeodeURLHandler.exe";

    if (!std::filesystem::exists(exePath)) {
        auto exeData = std::vector<uint8_t>(GeodeURLHandler, GeodeURLHandler + sizeof(GeodeURLHandler));
        if (auto err = file::writeBinary(exePath, exeData).err()) {
            log::error("Failed to write exe file: {}", err);
            return;
        }
    }

    HKEY geodeKey = createKey(HKEY_CURRENT_USER, "Software\\Classes\\geode");
    HKEY iconKey = createKey(geodeKey, "DefaultIcon");
    HKEY shellKey = createKey(geodeKey, "shell");
    HKEY openKey = createKey(shellKey, "open");
    HKEY commandKey = createKey(openKey, "command");

    bool success = setKeyValue(geodeKey, nullptr, "URL:Geode Protocol")
                && setKeyValue(geodeKey, "URL Protocol", "")
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
#endif

$on_mod(Loaded) {
    platformSetup();

    listen("handle", [](IPCEvent* ev) -> matjson::Value {
        if (auto str = ev->messageData->asString()) {
            log::info("WOW a url!! {}", str.unwrap());
        } else {
            log::error("Invalid IPC Message: {}", ev->messageData);
        }
        return {};
    });
}
