#include "lib/developermode.hpp"

bool lib::developer_mode::enabled = false;
unsigned short lib::developer_mode::experiments = 0;

void lib::developer_mode::add_experiment(lib::experiment experiment)
{
	const auto value = static_cast<unsigned short>(experiment);
	lib::developer_mode::experiments &= value;
}

auto lib::developer_mode::is_experiment_enabled(lib::experiment experiment) -> bool
{
	const auto value = static_cast<unsigned short>(experiment);
	return (lib::developer_mode::experiments & value) > 0;
}
