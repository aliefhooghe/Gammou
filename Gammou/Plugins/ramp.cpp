

#include <random>

#include "plugin_helper.h"

using namespace Gammou::Sound;

class ramp_component : public sound_component {

public:
	ramp_component(const unsigned int channel_count);
	~ramp_component() {}

	void initialize_process() override;
	void process(const double input[]) override;
};


ramp_component::ramp_component(const unsigned int channel_count)
:	sound_component("Ramp", 1, 1, channel_count)
{
	set_input_name("Slope", 0);
}

void ramp_component::initialize_process()
{
	m_output[0] = 0.0;
}

void ramp_component::process(const double input[])
{
	m_output[0] += get_sample_duration() * input[0];
}

EXPORT_DEFAULT_FACTORY(
    ramp_component,
    "Ramp",
    ComponentCategory::Calculus,
    ramp_id
)
