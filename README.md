# Geode URI API

Allows **geode://** to be intercepted by mods.

### Usage Example
```cpp
#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp>

$on_mod(Loaded) {
	URIEvent("alert").listen([](std::string_view path) {
	    FLAlertLayer::create("Custom Alert", std::string(path), "Ok")->show();
	    return true;
	}).leak();
}
```

Going to `geode://alert/Test Message` will show an alert that says "Test Message"