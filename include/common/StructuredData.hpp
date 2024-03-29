#ifndef STRUCTUREDDATA_HPP_
#define STRUCTUREDDATA_HPP_

#include <string>
#include "Config.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

class StructuredData
{
    using json = nlohmann::ordered_json;
    public:
        StructuredData(const std::string &file);
        StructuredData(StructuredData&&) = default;
        const Config::Object &GetConfig() const;
    private:
        Config::Object GetObject(const json &object);
        Config::Array GetArray(const json &array);
        Config::NodePtr GetPrimitive(const json &primitive);
        Config::Object _config;
};
#endif /* !STRUCTUREDDATA_HPP_ */
