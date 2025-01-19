#pragma once

#include <Geode/loader/EventV2.hpp>
#include <string>

#ifdef URI_TL
    #ifdef GEODE_IS_MACOS
        #define __declspec(x) __attribute__((visibility("default")))
    #endif

    #define URI_DLL __declspec(dllexport)
#else
    #ifdef GEODE_IS_MACOS
        #define __declspec(x)
    #endif

    #define URI_DLL __declspec(dllimport)
#endif


class URI_DLL URIEvent : public geode::Event {
protected:
    std::string uri;
public:
    URIEvent(const std::string& uri);
    std::string const& getURI() const;
};

geode::event::v2::EventHandler<URIEvent> URI_DLL handleURI(
    std::string const& handlePath,
    std::function<void(std::string const&)> callback
);
