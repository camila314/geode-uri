#pragma once

#include <Geode/loader/Event.hpp>
#include <string_view>

class URIEvent : public geode::Event<URIEvent, void(std::string_view)> {
    using Event::Event;
};
