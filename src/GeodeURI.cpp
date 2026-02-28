#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/IPC.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <GeodeURI.hpp>

using namespace geode::prelude;
using namespace ipc;

void bringToFront();

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

void runEvent(std::string const& pathFlag) {
    auto path = percent_decode(pathFlag);
    auto res = URIEvent().send(path);
    if (res == ListenerResult::Propagate) {
        log::info("No handler found for URI: {}", path);
        FLAlertLayer::create(
            "No Handler",
            fmt::format("No mods responded to <cl>geode://{}</c>", path),
            "Ok"
        )->show();
    }
}

$on_mod(Loaded) {
    if (auto pathFlag = Mod::get()->getLaunchArgument("path")) {
        bringToFront();
        GameEvent(GameEventType::Loaded).listen([pathFlag] {
            runEvent(pathFlag.value());
        }).leak();
    }

    listen("handle", [](matjson::Value messageData) -> matjson::Value {
        if (auto str = messageData.asString().ok()) {
            Loader::get()->queueInMainThread([str] {
                bringToFront();
                runEvent(str.value());
            });
        } else {
            log::error("Invalid IPC Message: {}", messageData.dump());
        }
        return {};
    });
}
