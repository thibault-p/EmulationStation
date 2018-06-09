#pragma once

#include "Log.h"
#include <pistache/http.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>

using namespace Pistache;

class WebService
{
public:
	WebService(Address addr) : httpEndPoint(std::make_shared<Http::Endpoint>(addr)), desc("Emulstation API", "0.1")
	{
	}

	void init(size_t threads = 1);

	void start();

	void stop();

private:
	void createDescription();

	void retrieveAllSystems(const Rest::Request &req, Http::ResponseWriter response);

	void retrieveAllGamesForSystem(const Rest::Request &req, Http::ResponseWriter response);

private:
	std::shared_ptr<Http::Endpoint> httpEndPoint;
	Rest::Description desc;
	Rest::Router router;
};