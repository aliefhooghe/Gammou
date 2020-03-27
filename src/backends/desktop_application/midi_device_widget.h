#ifndef GAMMOU_MIDI_DEVICE_WIDGET_H_
#define GAMMOU_MIDI_DEVICE_WIDGET_H_

#include "desktop_application.h"
namespace Gammou {

    class midi_device_widget : public View::panel_implementation<> {

    public:
        midi_device_widget(desktop_application& app);
        ~midi_device_widget() noexcept = default;
    };

}

#endif