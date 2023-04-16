#pragma once

class AppConfig
{
public:
	static auto useNativeMenuBar() -> bool;

	static auto useClickableSlider() -> bool;

private:
	AppConfig() = default;
};
