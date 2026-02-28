# Geode URI API

Allows **geode://** to be intercepted by mods.

### Usage Example
```cpp
#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp>

$on_mod(Loaded) {
	handleURI("alert", [](std::string const& path) {
		FLAlertLayer::create("Custom Alert", path, "Ok")->show();
	}).leak();
}
```

Going to `geode://alert/Test Message` will show an alert that says "Test Message"