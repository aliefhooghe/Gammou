

#include <cstdlib>

#include "plugin_helper.h"

using namespace Gammou::Sound;

class noise_component : public sound_component {

public:
	noise_component(const unsigned int channel_count);
	~noise_component() {}

	void process(const double input[]) override;
};

noise_component::noise_component(const unsigned int channel_count)
	: sound_component("Noise", 0, 1, channel_count)
{

}

void noise_component::process(const double[])
{
	m_output[0] = 
		-1.0 + static_cast<double>((rand() % 65536)) / 32768.0; 
}

EXPORT_DEFAULT_FACTORY(
    noise_component,
    "Noise",
    ComponentCategory::Noise,
    noise_id
)
