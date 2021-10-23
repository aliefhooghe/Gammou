#include <fstream>

#include <DSPJIT/log.h>

#include "patch_browser.h"
#include "helpers/layout_builder.h"

namespace Gammou
{
    std::unique_ptr<View::widget> make_patch_browser(application &app,  const std::filesystem::path& patch_path)
    {
        auto preset_name_input = std::make_unique<View::text_input>();
        auto save_button = std::make_unique<View::text_push_button>("Save");
        auto update_button = std::make_unique<View::text_push_button>("Update");
        auto remove_button = std::make_unique<View::text_push_button>("Remove");

        auto filesystem_view = std::make_unique<View::filesystem_view>(patch_path, 140, 250);

        LOG_INFO("[main gui] Using patch path '%s'\n", patch_path.generic_string().c_str());

        preset_name_input->set_text("patch");

        update_button->set_callback(
            [fv = filesystem_view.get()]()
            {
                fv->update();
            });

        remove_button->set_callback(
            [fv = filesystem_view.get()]()
            {
                fv->visit_selected_item(
                    [fv](const auto& item)
                    {
                        using item_type = std::decay_t<decltype(item)>;
                        if constexpr (std::is_same_v<item_type, std::filesystem::path>) {
                            std::remove(item.generic_string().c_str());
                            fv->update();
                        }
                    }
                );
            });

        filesystem_view->set_value_select_callback(
            [&app, name_input = preset_name_input.get(), patch_path, fv = filesystem_view.get()](const auto &preset_path)
            {
                LOG_INFO("[main gui] Loading patch '%s'\n", preset_path.generic_string().c_str());

                try
                {
                    nlohmann::json json;
                    std::ifstream stream{preset_path, std::ios_base::in};

                    if (!stream.good())
                        throw std::invalid_argument("Unable to open patch file");
                    else
                        stream >> json;

                    // Deserialize application state
                    app.deserialize(json);

                    // Update the patch browser view
                    const auto relative_path = std::filesystem::relative(preset_path, patch_path);
                    name_input->set_text(relative_path.generic_string());
                }
                catch (const std::exception &e)
                {
                    LOG_ERROR("[main gui] Failed to load state : %s\n", e.what());
                    fv->reset_selection();
                }
            });

        save_button->freeze_size();
        save_button->set_callback(
            [&app, input = preset_name_input.get(), fv = filesystem_view.get(), patch_path]()
            {
                const auto preset_path = patch_path / input->get_text();
                LOG_INFO("[main gui] Saving patch '%s'\n", preset_path.generic_string().c_str());

                try
                {
                    // First: try to serialize
                    const auto json = app.serialize();

                    // Write file if serialize is successfull: avoid removing a valid state if
                    // serialization fail
                    std::ofstream stream{preset_path, std::ios_base::trunc};
                    if (!stream.good())
                        throw std::invalid_argument("Unable to open patch file for write");

                    stream << json.dump();
                    stream.close();
                    fv->update();
                    LOG_INFO("[main gui] Saved patch '%s'\n", preset_path.generic_string().c_str());
                }
                catch (const std::exception &e)
                {
                    LOG_ERROR("[main gui] Failed to save state : %s\n", e.what());
                }
            });

        const View::layout_builder builder{};

        return builder.header(
                builder.vertical(
                    builder.horizontal(
                        std::move(preset_name_input), std::move(save_button)),
                    std::move(update_button),
                    std::move(remove_button),
                    std::move(filesystem_view)
                ));
    }
}
