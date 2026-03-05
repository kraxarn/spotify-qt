#include "lib/spotify/void.hpp"

auto Void::fromJson([[maybe_unused]] const QJsonDocument &json) -> Void
{
	return {};
}
