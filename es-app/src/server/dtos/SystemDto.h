
#pragma once

#include <string>
#include "rapidjson/prettywriter.h"

namespace Dto
{
using namespace rapidjson;
class SystemDto
{
  public:
	SystemDto(std::string fullname,
			  std::string name,
			  int gameCount)
		: fullname(fullname),
		  name(name),
		  gameCount(gameCount)
	{
	}

	inline std::string getName() const { return name; }

	inline int getGameCount() const { return gameCount; }

	template <typename Writer>
	void serialize(Writer &writer) const
	{
		writer.StartObject();

		writer.String("fullname");
#if RAPIDJSON_HAS_STDSTRING
		writer.String(fullname);
#else
		writer.String(fullname.c_str(), static_cast<SizeType>(fullname.length())); // Supplying length of string is faster.
#endif

		writer.String("name");
#if RAPIDJSON_HAS_STDSTRING
		writer.String(name);
#else
		writer.String(name.c_str(), static_cast<SizeType>(name.length())); // Supplying length of string is faster.
#endif

		writer.String("game_count");
		writer.Int(gameCount);

		writer.EndObject();
	}

  private:
	std::string fullname;
	std::string name;
	int gameCount;
};
}