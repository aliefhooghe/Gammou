

#include "plugin_helper.h"

using namespace Gammou::Sound;

class Arctan_component : public sound_component {

public:
	Arctan_component(const unsigned int channel_count);
	~Arctan_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_variable<double> m_frequence_integral;
};

Arctan_component::Arctan_component(const unsigned int channel_count)
	: sound_component("Arctan", 1, 1, channel_count),
	m_frequence_integral(this)
{
}

void Arctan_component::process(const double input[])
{
	m_output[0] = atan(input[0]);
}

void Arctan_component::initialize_process()
{
	m_frequence_integral = 0.0;
}

EXPORT_DEFAULT_FACTORY(
	Arctan_component, 
	"Arctan", 
	"Calculus", 
	636363635
)