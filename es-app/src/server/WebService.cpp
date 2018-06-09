#include "WebService.h"
#include <pistache/serializer/rapidjson.h>
#include "SystemData.h"
#include "dtos/SystemDto.h"
#include <vector>
#include <cstdio>
#include <string>
#include <algorithm>

using namespace rapidjson;

void WebService::init(size_t threads)
{
	auto opts = Http::Endpoint::options()
					.threads(threads)
					.flags(Tcp::Options::InstallSignalHandler);
	httpEndPoint->init(opts);
	createDescription();
}

void WebService::start()
{
	router.initFromDescription(desc);

	httpEndPoint->setHandler(router.handler());
	httpEndPoint->serve();
}

void WebService::stop()
{
	LOG(LogInfo) << "Shuting down webservice";
	httpEndPoint->shutdown();
}

void WebService::createDescription()
{
	desc.info().license("Apache", "http://www.apache.org/licenses/LICENSE-2.0");

	auto backendResponseError = desc.response(Http::Code::Internal_Server_Error, "An error occured with backend");

	desc
		.schemes(Rest::Scheme::Http)
		.basePath("/v1")
		.produces(MIME(Application, Json))
		.consumes(MIME(Application, Json));

	auto versionPath = desc.path("/v1");
	auto systemsPath = versionPath.path("/systems");

	systemsPath
		.route(desc.get("/"))
		.bind(&WebService::retrieveAllSystems, this)
		.produces(MIME(Application, Json))
		.response(backendResponseError);

	systemsPath
		.route(desc.get("/:system/games/"))
		.bind(&WebService::retrieveAllGamesForSystem, this)
		.parameter<Rest::Type::String>(":system", "The name of the system to list games")
		.produces(MIME(Application, Json))
		.response(backendResponseError);
}

void WebService::retrieveAllSystems(const Rest::Request &req, Http::ResponseWriter response)
{
	LOG(LogInfo) << "Retrieving systems...";
	auto systems = SystemData::sSystemVector;
	std::vector<Dto::SystemDto> dtos;

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);

	writer.StartArray();
	for (auto sIt = systems.cbegin(); sIt != systems.cend(); sIt++)
	{
		auto sys = *sIt;
		Dto::SystemDto dto(sys->getFullName(), sys->getName(), sys->getGameCount());
		dto.serialize(writer);
		LOG(LogInfo) << "System: " << sys->getFullName();
	}
	writer.EndArray();

	response.send(Http::Code::Ok, sb.GetString());
}

void WebService::retrieveAllGamesForSystem(const Rest::Request &req, Http::ResponseWriter response)
{
	auto system = req.param(":system").as<std::string>();
	LOG(LogInfo) << "Retrieving games for system: '" << system << "'...";

	auto systems = SystemData::sSystemVector;
	auto sIt = std::find_if(systems.begin(), systems.end(), [system](SystemData *s) -> bool { return system.compare(s->getName()) == 0; });

	if (sIt == systems.end())
	{
		response.send(Http::Code::Not_Found);
		return;
	}

	LOG(LogInfo) << "Found system !";
	auto sys = (*sIt);
	auto games = sys->getRootFolder()->getChildren();
	for (auto gIt = games.cbegin(); gIt != games.cend(); ++gIt)
	{
		auto game = (*gIt);
		LOG(LogInfo) << "Game: " << game->getName();
	}

	response.send(Http::Code::Ok);
}