

#include "plugin_helper.h"

using namespace Gammou::Sound;

class delay_component : public sound_component {

public:

	delay_component(const unsigned int channel_count);
	~delay_component() {}

	// Doit �tre impl�ment�es
	void process(const double input[]) override;

	// Peuvent �tre impl�ment�es
	void initialize_process() override;
	//void on_sample_rate_change(const double new_sample_rate) override;
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

}

void delay_component::process(const double input[])
{
	m_queue << input[0];

	const unsigned int index = 
		static_cast<unsigned int>(get_sample_rate()	* input[1]);

	m_output[0] = m_queue[index];	
}



EXPORT_DEFAULT_FACTORY(
	delay_component,			//		Classe
	"delay",					//		Nom
	"Exemple",					//		Categorie
	25681846		//		Factory Id
)