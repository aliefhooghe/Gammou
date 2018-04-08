

#include "plugin_helper.h"

using namespace Gammou::Sound;

class bit_crusher_component : public sound_component {

public:
	bit_crusher_component(const unsigned int channel_count);
	~bit_crusher_component() {}

	void process(const double input[]) override;
private:
	multi_channel_variable<double> m_frequence_integral;
};

bit_crusher_component::bit_crusher_component(const unsigned int channel_count)
	: sound_component("bit_crusher", 2, 1, channel_count),
	m_frequence_integral(this)
{
	set_input_name("Input", 0);
	set_input_name("Step", 1);
	set_output_name("Output", 0);
}

void bit_crusher_component::process(const double input[])
{
	const double step = constrain_in_range(input[1], 0.000015385, 20000.0);
	const int n = static_cast<unsigned int>(input[0] / step);
	m_output[0] = static_cast<double>(n) * step;
}



EXPORT_DEFAULT_FACTORY(
	bit_crusher_component,			//		Classe
	"BitCrusher",					//		Nom
	"Exemple",					//		Categorie
	248657319		//		Factory Id
)