

#include "plugin_helper.h"

using namespace Gammou::Sound;

class template_component : public sound_component {

public:

	template_component(const unsigned int channel_count);
	~template_component() {}

	// Doit être implémentées
	void process(const double input[]) override;

	// Peuvent être implémentées
	void initialize_process() override;
	//void on_sample_rate_change(const double new_sample_rate) override;
private:
	multi_channel_variable<double> m_frequence_integral;
};

template_component::template_component(const unsigned int channel_count)
	: sound_component("Template", 2, 1, channel_count), // 2,1 = nb input, nb output
	m_frequence_integral(this)	//	Initialisation d'une variable multicannal
{
	//	Par defaut, In-0, In1 ,...., Out-0, Out-1, ...
	set_input_name("Freq", 0);
	set_input_name("Phase", 1);

	set_output_name("Output", 0);
}

void template_component::initialize_process()
{
	m_frequence_integral = 0.0;
}

void template_component::process(const double input[])
{
	// A ce moment m_output[0] = sortie percedente


	m_output[0] = sin(6.28318530718 * m_frequence_integral + input[1]);
	m_frequence_integral += (input[0] * get_sample_duration());

	// Te = get_sample_duration()
	// Fe = get_sample_rate()		
}



EXPORT_DEFAULT_FACTORY(
	template_component,			//		Classe
	"Template",					//		Nom
	"Exemple",					//		Categorie
	plugins_id::template_id		//		Factory Id
)