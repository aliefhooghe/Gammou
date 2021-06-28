#include <fstream>

#include "patch_browser.h"
#include "backends/common/configuration.h"

namespace Gammou
{
    std::unique_ptr<View::widget> make_patch_browser(application &app)
    {
        const auto patch_dir_path = configuration::get_patch_path();

        auto preset_name_input = std::make_unique<View::text_input>(110, 21);
        auto save_button = std::make_unique<View::text_push_button>("Save", 80, 21);
        auto update_button = std::make_unique<View::text_push_button>("Update", 195, 21);

        auto filesystem_view = std::make_unique<View::filesystem_view>(patch_dir_path, 140, 90);

        LOG_INFO("[main gui] Using patch path '%s'\n",
                 patch_dir_path.generic_string().c_str());

        preset_name_input->set_text("patch");

        update_button->set_callback(
            [fv = filesystem_view.get()]()
            {
                fv->update();
            });

        filesystem_view->set_value_select_callback(
            [&app, name_input = preset_name_input.get(), patch_dir_path](const auto &preset_path)
            {
                LOG_INFO("[main gui] Loading patch '%s'\n", preset_path.generic_string().c_str());

                nlohmann::json json;
                std::ifstream stream{preset_path, std::ios_base::in};

                if (!stream.good())
                    return;

                try
                {
                    stream >> json;
                }
                catch (const std::exception &e)
                {
                    LOG_ERROR("[main gui] Failed to load json file : %s\n", e.what());
                    return;
                }
                catch (...)
                {
                    LOG_ERROR("[main gui] Failed to load json file : unknown error\n");
                    return;
                }

                if (app.deserialize(json))
                {
                    const auto relative_path =
                        std::filesystem::relative(preset_path, patch_dir_path);
                    name_input->set_text(relative_path.generic_string());
                }
            });

        save_button->set_callback(
            [&app, input = preset_name_input.get(), fv = filesystem_view.get(), patch_dir_path]()
            {
                try
                {
                    const auto preset_path = patch_dir_path / input->get_text();
                    std::ofstream stream{preset_path, std::ios_base::trunc};

                    if (!stream.good())
                        throw std::exception{};

                    const auto json = app.serialize();
                    stream << json.dump();
                    stream.close();
                    fv->update();
                    LOG_INFO("[main gui] Saved patch '%s'\n", preset_path.generic_string().c_str());
                }
                catch (const std::exception &e)
                {
                    LOG_ERROR("[main gui] Failed to save preset : %s\n", e.what());
                }
                catch (...)
                {
                    LOG_ERROR("[main gui] Failed to save preset : unknown error\n");
                }
            });

        auto panel = std::make_unique<View::panel<>>(210, 60);
        panel->insert_widget(5, 5, std::move(preset_name_input));
        panel->insert_widget(5 + 110 + 5, 5, std::move(save_button));
        panel->insert_widget(5, 5 + 21 + 5, std::move(update_button));

        return std::make_unique<View::header>(
            View::make_vertical_layout(
                std::move(panel),
                std::move(filesystem_view)));
    }
}
