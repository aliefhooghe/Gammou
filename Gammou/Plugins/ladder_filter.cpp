
#include "plugin_helper.h"

using namespace Gammou::Sound;

class ladder_component : public sound_component {

public:
	ladder_component(const unsigned int channel_count);
	~ladder_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_array<double> m_s;
};

ladder_component::ladder_component(const unsigned int channel_count)
:   sound_component("ladder", 3, 1, channel_count),
    m_s(this, 4)
{
	set_input_name("In", 0);
	set_input_name("Freq", 1);
	set_input_name("K", 2);

	set_output_name("Out", 0);
}

void ladder_component::initialize_process()
{
    for(unsigned int i = 0; i < 3; ++i)
		m_s[i] = 0.0;	
}

void ladder_component::process(const double input[])
{
	const double x = input[0];
	const double k = input[2];
	const double T = get_sample_duration();

	const double f = 3.14 * T * input[1];
	const double v = 1.0 / (1.0 + f);
	const double g = f * v;

	const double G = g * g * g * g;
	const double S = m_s[3] + g * (m_s[2] + g * (m_s[1] + g * m_s[0]));
	const double u = (x - k * S) / (1.0 + k * G);

	double in = u;

	for(unsigned int i = 0; i < 4; ++i){
		in = g * in + v * m_s[i];
		m_s[i] = 2.0 * in - m_s[i];
	}

	m_output[0] = in;
}


EXPORT_DEFAULT_FACTORY(
    ladder_component,
    "ladder",
    ComponentCategory::Filter,
    ladder_filter_id
)
