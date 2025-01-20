#pragma once

#include <Geode/loader/EventV2.hpp>
#include <Geode/utils/string.hpp>
#include <functional>
#include <string>

class URIEvent : public geode::Event {
protected:
    std::string uri;
public:
    inline URIEvent(const std::string& uri) : uri(uri) {}
    inline std::string const& getURI() const { return uri; }
};

inline geode::event::v2::EventHandler<URIEvent> handleURI(
    std::string const& handlePath,
    std::function<void(std::string const&)> callback
) {
    auto hand = geode::event::v2::EventHandler<URIEvent>([handlePath](URIEvent* ev) {
        return geode::utils::string::startsWith(ev->getURI(), handlePath + "/");
    });
    hand.bind([=](URIEvent* ev) {
        callback(ev->getURI().substr(handlePath.size() + 1));
        return geode::ListenerResult::Stop;
    });

    return hand;
}
