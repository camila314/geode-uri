#pragma once

#include <Geode/loader/Event.hpp>
#include <string_view>

class URIEvent : public geode::Event<URIEvent, bool(std::string_view), std::string> {
    using Event::Event;
};
