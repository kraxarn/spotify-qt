#include "lib/github/api.hpp"
#include "lib/fmt.hpp"
#include "lib/log.hpp"

lib::gh::api::api(const lib::http_client &http_client)
	: http_client(http_client)
{
}

auto lib::gh::api::parse(const std::string &response,
	std::string &error_message) -> nlohmann::json
{
	if (response.empty())
	{
		error_message = "Empty response";
		return {};
	}

	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(response);
	}
	catch (const std::exception &e)
	{
		error_message = e.what();
		return {};
	}

	// Error message from api
	if (json.contains("message"))
	{
		json.at("message").get_to(error_message);
		return {};
	}

	return json;
}

void lib::gh::api::contributors(const std::string &owner, const std::string &repo,
	lib::callback<std::vector<lib::gh::contributor>> &callback) const
{
	http_client.get(lib::fmt::format("https://api.github.com/repos/{}/{}/contributors",
		owner, repo), lib::headers(), [callback](const std::string &response)
	{
		std::string message;
		const auto json = parse(response, message);
		if (!message.empty())
		{
			lib::log::warn("/contributors failed: {}", message);
			callback(std::vector<lib::gh::contributor>());
			return;
		}

		callback(json);
	});
}

void lib::gh::api::release(const std::string &owner, const std::string &repo,
	const std::string &tag, lib::callback<lib::gh::release> &callback) const
{
	http_client.get(lib::fmt::format("https://api.github.com/repos/{}/{}/releases/tags/{}",
			owner, repo, tag), lib::headers(),
		[tag, callback](const std::string &response)
		{
			std::string message;
			const auto json = parse(response, message);
			if (!message.empty())
			{
				lib::log::warn("/releases/tags/{} failed: {}", tag, message);
				callback(lib::gh::release());
				return;
			}

			callback(json);
		});
}
