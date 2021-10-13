#include "lib/github/api.hpp"

lib::gh::api::api(const lib::http_client &http_client)
	: http_client(http_client)
{
}

void lib::gh::api::contributors(const std::string &owner, const std::string &repo,
	lib::callback<std::vector<lib::gh::contributor>> &callback)
{
	http_client.get(lib::fmt::format("https://api.github.com/repos/{}/{}/contributors",
		owner, repo), lib::headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			callback(std::vector<lib::gh::contributor>());
			return;
		}
		callback(nlohmann::json::parse(response));
	});
}
