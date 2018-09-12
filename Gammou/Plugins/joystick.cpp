
#include <SDL2/SDL.h>
#include "plugin_helper.h"

using namespace Gammou::Sound;

    /*
     *  
     *      Joystick Component Implementation     
     * 
     */

class joystick_component : public sound_component {

public:
    joystick_component(const unsigned int channel_count);
    ~joystick_component() override;

    virtual void process(const double input[]) override;

private:
    std::vector<double> m_axis;
};

joystick_component::joystick_component(const unsigned int channel_count)
    : sound_component("Joystick", 0, 0, channel_count)
{

}

joystick_component::~joystick_component()
{
}

void joystick_component::process(const double input[])
{
    const unsigned int axis_count = axis.size();

    for (unsigned int i = 0; i < axis_count; ++i)
        m_output[i] = m_axis[i];
}


    /*
     *  
     *      Joystick Component Implementation     
     * 
     */

//  Assume that there is only one factory instance

class joystick_factory : public plugin_factory {

    public:
        joystick_factory();
        ~joystick_factory() {}

    private:
        std::unique_ptr<request_form> create_plugin_request_form() override;

        abstract_sound_component *create_sound_component(
            data_input_stream& source,
            const unsigned int channel_count) override;

        abstract_sound_component *create_sound_component(
            const answer_form& answer_form,
            const unsigned int channel_count) override;

        static void joystick_event_loop();
        bool m_event_loop_running;
};


joystick_factory::joystick_factory()
    :   plugin_factory("joystick", "", joystick_id),
        m_event_loop_running(false)
{
}

std::unique_ptr<request_form> joystick_factory::create_plugin_request_form()
{
    return create_empty_request_form();
}

abstract_sound_component *joystick_factory::create_sound_component(
    data_input_stream& source,
    const unsigned int channel_count)
{
    return nullptr; // todo
}

abstract_sound_component *joystick_factory::create_sound_component(
    const answer_form& answer_form,
    const unsigned int channel_count)
{
    return nullptr;
}

void joystick_factory::joystick_event_loop()
{

}


EXPORT_FACTORY(joystick_factory)
