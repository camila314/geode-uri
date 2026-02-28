#pragma once

#include <Geode/loader/Event.hpp>
#include <Geode/utils/string.hpp>
#include <functional>
#include <string>

class URIEvent : public geode::Event<URIEvent, bool(std::string uri)> {
public:
    using Event::Event;
};

/***
 * Handles a URI. If an application attempts to open
 * a URI starting with `geode://`, this will intercept
 * the event. 
 * 
 * Ex. with a handle path of `level`, the URI `geode://level/128
 * will call the callback with the string `128`.
 */
inline geode::ListenerHandle handleURI(
    std::string const& handlePath,
    std::function<void(std::string const&)> callback
) {
    return URIEvent().listen([handlePath, callback](const std::string& uri) {
        if (geode::utils::string::startsWith(uri, handlePath + "/")) {
            callback(uri.substr(handlePath.size() + 1));
            return geode::ListenerResult::Stop;
        }
        return geode::ListenerResult::Propagate;
    });
}