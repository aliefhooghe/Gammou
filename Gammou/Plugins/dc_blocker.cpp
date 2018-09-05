

#include "plugin_helper.h"

using namespace Gammou::Sound;

class dc_blocker_component : public sound_component {

public:

	dc_blocker_component(const unsigned int channel_count);
	~dc_blocker_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_variable<double> m_last_input;
};

dc_blocker_component::dc_blocker_component(const unsigned int channel_count)
	: sound_component("dc_blocker", 1, 1, channel_count),
	m_last_input(this)
{
	set_input_name("In", 0);
	set_output_name("Out", 0);
}

void dc_blocker_component::initialize_process()
{
	m_last_input = 0.0;
}

void dc_blocker_component::process(const double input[])
{
	const double R = 0.995;
	const double out = 
		input[0] - m_last_input + 
		R * m_output[0];
		
	m_last_input = input[0];
	m_output[0] = out;
}


EXPORT_DEFAULT_FACTORY(
	dc_blocker_component,			//		Classe
	"DCblocker",				//		Nom
	"Exemple",					//		Categorie
	85262561252		//		Factory Id
)