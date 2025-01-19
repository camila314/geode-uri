#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/EventV2.hpp>
#include <Geode/loader/IPC.hpp>
#include <GeodeURI.hpp>

using namespace geode::prelude;
using namespace geode::event::v2;
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

URIEvent::URIEvent(const std::string& uri) : uri(uri) {}
std::string const& URIEvent::getURI() const { return uri; }

std::string trimSlash(std::string const& str) {
    int start = str.find_first_not_of("/");
    return str.substr(start, str.find_last_not_of("/") + 1 - start);
}

EventHandler<URIEvent> handleURI(std::string const& handlePath, std::function<void(std::string const&)> callback) {
    auto hand = EventHandler<URIEvent>([handlePath](URIEvent* ev) {
        return ev->getURI().find(trimSlash(handlePath) + "/") == 0 || ev->getURI() == trimSlash(handlePath);
    });
    hand.bind([=](URIEvent* ev) {
        auto data = ev->getURI().substr(handlePath.size());

        callback(trimSlash(data));
        return ListenerResult::Stop;
    });
    return hand;
}

void runEvent(std::string const& pathFlag) {
    auto path = percent_decode(pathFlag);
    auto res = URIEvent(path).post();
    if (res == ListenerResult::Propagate) {
        log::info("No handler found for URI: {}", path);
        FLAlertLayer::create(
            "No Handler",
            fmt::format("No mods responded to <cl>geode://{}</c>", path),
            "Ok"
        )->show();
    }

    bringToFront();
}

$on_mod(Loaded) {
    if (auto pathFlag = Loader::get()->getLaunchArgument("uri-path")) {
        runEvent(pathFlag.value());
    }

    listen("handle", [](IPCEvent* ev) -> matjson::Value {
        if (auto str = ev->messageData->asString().ok()) {
            Loader::get()->queueInMainThread([str] { runEvent(str.value()); });
        } else {
            log::error("Invalid IPC Message: {}", ev->messageData);
        }
        return {};
    });
}
