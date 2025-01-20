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

/***
 * Handles a URI. If an application attempts to open
 * a URI starting with `geode://`, this will intercept
 * the event. 
 * 
 * Ex. with a handle path of `level`, the URI `geode://level/128
 * will call the callback with the string `128`.
 */
inline geode::event::v2::EventHandler<URIEvent>* handleURI(
    std::string const& handlePath,
    std::function<void(std::string const&)> callback
) {
    auto hand = new geode::event::v2::EventHandler<URIEvent>([handlePath](URIEvent* ev) {
        return geode::utils::string::startsWith(ev->getURI(), handlePath + "/");
    });
    hand->bind([=](URIEvent* ev) {
        callback(ev->getURI().substr(handlePath.size() + 1));
        return geode::ListenerResult::Stop;
    });

    return hand;
}
