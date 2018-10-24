

#include <random>

#include "plugin_helper.h"

using namespace Gammou::Sound;

class noise_component : public sound_component {

public:
	noise_component(const unsigned int channel_count);
	~noise_component() {}

	void process(const double input[]) override;

private:
	static std::default_random_engine m_random_engine;
	std::uniform_real_distribution<double> m_distribution;
};


std::default_random_engine noise_component::m_random_engine{};

noise_component::noise_component(const unsigned int channel_count)
:	sound_component("Noise", 0, 1, channel_count),
	m_distribution(-1.0, 1.0)
{
}

void noise_component::process(const double[])
{
	m_output[0] =
		m_distribution(m_random_engine);
}

EXPORT_DEFAULT_FACTORY(
    noise_component,
    "Noise",
    ComponentCategory::Noise,
    noise_id
)
