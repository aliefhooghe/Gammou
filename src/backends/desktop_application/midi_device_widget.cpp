
#include "midi_device_widget.h"

namespace Gammou {

    midi_device_widget::midi_device_widget(desktop_application& app)
    : panel_implementation<>{0, 0 /* dummy sizes */}
    {
        const auto midi_input_count = app._midi_input_count();
        constexpr auto y_start_offset = 0.2f;
        constexpr auto line_height = 1.2f;

        for (auto i = 0u; i < midi_input_count; ++i) {
            const auto y_offset = y_start_offset + i * line_height;
            auto device_checkbox = std::make_unique<View::checkbox>();
            auto device_label = std::make_unique<View::label>(
                app._midi_inputs[i].getPortName(i));

            insert_widget(1.0, y_offset + 0.1, std::move(device_checkbox));
            insert_widget(2.2f, y_offset, std::move(device_label));
        }

        resize(30, line_height * midi_input_count + y_start_offset);
    }

}