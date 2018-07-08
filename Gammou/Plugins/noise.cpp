

#include <cstdlib>

#include "plugin_helper.h"

using namespace Gammou::Sound;

class noise_component : public sound_component {

public:
	noise_component(const unsigned int channel_count);
	~noise_component() {}

	void process(const double input[]) override;
};

noise_component::noise_component(const unsigned int channel_count)
	: sound_component("Noise", 0, 1, channel_count)
{

}

void noise_component::process(const double input[])
{
	m_output[0] = static_cast<double>((rand() % 65536)) / 65535.0; 
}

EXPORT_DEFAULT_FACTORY(
	noise_component,			//		Classe
	"Noise",					//		Nom
	"Exemple",					//		Categorie
	1616232326					//		Factory Id
)