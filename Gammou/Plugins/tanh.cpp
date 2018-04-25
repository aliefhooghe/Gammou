

#include "plugin_helper.h"

using namespace Gammou::Sound;

class Arctan_component : public sound_component {

public:
	Arctan_component(const unsigned int channel_count);
	~Arctan_component() {}

	void process(const double input[]) override;
};

Arctan_component::Arctan_component(const unsigned int channel_count)
	: sound_component("Arctan", 1, 1, channel_count)
{
}

void Arctan_component::process(const double input[])
{
	m_output[0] = tanh(input[0]);
}

EXPORT_DEFAULT_FACTORY(
	Arctan_component, 
	"Tanh", 
	"Calculus", 
	636363635
)