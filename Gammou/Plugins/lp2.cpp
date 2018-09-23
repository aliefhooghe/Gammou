

#include "plugin_helper.h"

using namespace Gammou::Sound;

class lp2_component : public sound_component {

public:

	lp2_component(const unsigned int channel_count);
	~lp2_component() {}

	void process(const double input[]) override;
	void initialize_process() override;

private:
	multi_channel_array<double> m_x;
	multi_channel_array<double> m_y;
};

lp2_component::lp2_component(const unsigned int channel_count)
	: sound_component("_lp2", 3, 1, channel_count),
	m_x(this, 3),
	m_y(this, 2)
{
	set_input_name("In", 0);
	set_input_name("Freq", 1);
	set_input_name("Q", 2);

	set_output_name("Out", 0);
}

void lp2_component::initialize_process()
{
	m_x[0] = 0.0;
	m_x[1] = 0.0;
	m_x[2] = 0.0;

	m_y[0] = 0.0;
	m_y[0] = 0.0;
}

void lp2_component::process(const double input[])
{
	const double omega = 
		6.28318530718 * 
		constrain_in_range(input[1], 1.0, 0.45 * get_sample_rate());
	const double Q = std::abs(input[2] / 128.0);
	const double dt = get_sample_duration();


	const double a = omega * omega * dt * dt * dt;
	const double tmp = 4.0 * omega * Q * dt;
	const double b = dt * (tmp + 4.0);
	const double c = dt * (tmp - 4.0);

	const double out = 
		(
			a * (m_x[2] + 3.0 * (m_x[1] + m_x[0]) + input[0]) - 
			(
				(a - c) * m_y[1] + 
				(3.0 * a - b) * m_y[0] + 
				(c + 3.0 * a) * m_output[0]))
		/ (a + b);

	m_x[2] = m_x[1];
	m_x[1] = m_x[0];
	m_x[0] = input[0];

	m_y[1] = m_y[0];
	m_y[0] = m_output[0];

	m_output[0] = out;
}



EXPORT_DEFAULT_FACTORY(
	lp2_component,			//		Classe
	"_lp2",					//		Nom
	"Exemple",					//		Categorie
	65245412		//		Factory Id
)