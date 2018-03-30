

#include "plugin_helper.h"

using namespace Gammou::Sound;

class Test_component : public sound_component {

public:
	Test_component(const unsigned int channel_count);
	~Test_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_variable<double> m_frequence_integral;
};

Test_component::Test_component(const unsigned int channel_count)
	: sound_component("Test", 2, 1, channel_count),
	m_frequence_integral(this)
{
	set_input_name("Freq", 0);
	set_input_name("Phase", 1);
}

void Test_component::process(const double input[])
{
	m_output[0] = sin(6.28318530718 * m_frequence_integral + input[1]);
	m_frequence_integral += (input[0] * get_sample_duration());
}

void Test_component::initialize_process()
{
	m_frequence_integral = 0.0;
}

EXPORT_DEFAULT_FACTORY(
	Test_component, 
	"SuperSin", 
	"Oscilateur", 
	63636363
)