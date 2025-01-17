#include <geode-ipc.hpp>
#include <windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	if (__argc != 2)
		return 1;

	std::string urlString = __argv[1];

	if (urlString.find("geode://") == 0) {
		urlString = urlString.substr(8);
	} else if (urlString.find("geode:") == 0) {
		urlString = urlString.substr(6);
	} else {
		return 1;
	}

	if (!GeodeIPC::send("camila314.custom-uri", "handle", urlString)) {
		MessageBox(NULL, "Failed to communicate. Please make sure Geometry Dash is open.", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	return 0;
}