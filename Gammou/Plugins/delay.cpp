

#include "plugin_helper.h"

using namespace Gammou::Sound;

class delay_component : public sound_component {

public:

	delay_component(const unsigned int channel_count);
	~delay_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_queue<double> m_queue;
};

delay_component::delay_component(const unsigned int channel_count)
	: sound_component("delay", 2, 1, channel_count),
	m_queue(this, 100000)	
{
	set_input_name("In", 0);
	set_input_name("Time", 1);

	set_output_name("Output", 0);
}

void delay_component::initialize_process()
{
	m_queue.reset();
}

void delay_component::process(const double input[])
{
	m_queue << input[0];

	const double sample_count = 
		get_sample_rate() * std::abs(input[1]);

	const unsigned int index = 
		static_cast<unsigned int>(sample_count);

	const double a = m_queue[index];
	const double b = m_queue[index + 1];
	const double coef = 
		sample_count - static_cast<double>(index);

	m_output[0] = 
		(1.0 - coef) * a + coef * b;
}


EXPORT_DEFAULT_FACTORY(
	delay_component,			//		Classe
	"delay",					//		Nom
	"Exemple",					//		Categorie
	25681846		//		Factory Id
)