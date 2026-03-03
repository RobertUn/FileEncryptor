#pragma once

#include <string>
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

struct Item {
	int id;
	std::string name;
	std::string description;
	std::string url;

	Item() = default;
	Item(int id, const std::string& name, const std::string& description, const std::string& url)
		: id(id), name(name), description(description), url(url) {
	}
};

// Способ 0: фиксированный порядок
inline void to_json(ordered_json& j, const Item& item) {
    j["id"] = item.id;
    j["name"] = item.name;
    j["description"] = item.description;
    j["url"] = item.url;
}

inline void from_json(const ordered_json& j, Item& item) {
    j.at("id").get_to(item.id);
    j.at("name").get_to(item.name);
    j.at("description").get_to(item.description);
    j.at("url").get_to(item.url);
}

// Способ 1: макрос после определения структуры
/*
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Item, id, name, description, url)
*/

// Или Способ 2 (альтернатива):
/*
namespace nlohmann {
    template<>
    struct adl_serializer<Item> {
        static Item from_json(const json& j) {
            return Item{
                j.at("id").get<int>(),
                j.at("name").get<std::string>(),
                j.at("description").get<std::string>(),
                j.at("url").get<std::string>()
            };
        }

        static void to_json(json& j, const Item& item) {
            j = json{
                {"id", item.id},
                {"name", item.name},
                {"description", item.description},
                {"url", item.url}
            };
        }
    };
}
*/