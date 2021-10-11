#ifndef GAMMOU_SERIALIZATION_HELPER_H_
#define GAMMOU_SERIALIZATION_HELPER_H_

#include <nlohmann/json.hpp>
#include <filesystem>
#include <optional>

namespace nlohmann {
    /**
     * Theses specializations are in the nlohmann namespace because we don't want to write
     * in the std namespace.
     *
     * see github.com/nlohmann/json#how-do-i-convert-third-party-types
     */
    template <>
    struct adl_serializer<std::filesystem::path> {
        static void to_json(json& j, const std::filesystem::path& path)
        {
            j = path.generic_string();
        }

        static void from_json(const json& j, std::filesystem::path& path)
        {
            path = j.get<std::string>();
        }
    };

    template <typename T>
    struct adl_serializer<std::optional<T>> {

        static void to_json(json& j, const std::optional<T>& opt)
        {
            if (opt.has_value())
                j = opt.value();
            else
                j = nullptr;
        }

        static void from_json(const json& j, std::optional<T>& opt)
        {
            if (j.is_null())
                opt.reset();
            else
                opt.emplace(j.get<T>());
        }
    };
}

namespace Gammou {

    template <typename Tkey, typename Tfield>
    static void optional_field_get_to(const nlohmann::json& json, const Tkey& key, Tfield& field)
    {
        const auto it = json.find(key);
        if (it != json.end())
            it->get_to(field);
    }

}

#endif