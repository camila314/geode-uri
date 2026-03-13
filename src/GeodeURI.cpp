#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/IPC.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/utils/general.hpp>
#include <GeodeURI.hpp>

using namespace geode::prelude;
using namespace ipc;

void bringToFront();

std::string percent_decode(std::string_view str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.size()) {
                result += utils::numFromString<char>(str.substr(i + 1, 2), 16).unwrapOr('?');
                i += 2;
            }
        } else {
            result += str[i];
        }
    }

    return result;
}

void runEvent(std::string_view pathFlag) {
    auto path = percent_decode(pathFlag);

    if (URIEvent().send(path) == ListenerResult::Propagate) {
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

        GameEvent(GameEventType::Loaded).listen([path = *pathFlag] {
            runEvent(path);
        }).leak();
    }

    listen("handle", [](matjson::Value data) -> matjson::Value {
        if (auto str = data.asString().ok()) {
            Loader::get()->queueInMainThread([str] {
                bringToFront();
                runEvent(str.value());
            });
        } else {
            log::error("Invalid IPC Message: {}", data.dump());
        }
        return {};
    });
}
