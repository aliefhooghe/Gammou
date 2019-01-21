
#include "filter_order2.h"

namespace Gammou
{

namespace Sound
{

//	LP2

lp2::lp2(const unsigned int channel_count)
	: sound_component("LP2", 3, 1, channel_count),
	  m_previous_output_deriv(this)
{
	set_input_name("In", 0);
	set_input_name("Cutoff", 1);
	set_input_name("Reso", 2);
}

void lp2::initialize_process()
{
	m_output[0] = 0.0;
	m_previous_output_deriv = 0.0;
}

void lp2::process(const double input[])
{
	// 2nd order low pass, Rung kuta method

	const double in = input[0];
	const double omega0 = 6.28318530718 * constrain_in_range(input[1], 1.0, 16000.0);
	const double Q = constrain_in_range(input[2], 0.6, 100.0);

	const double y = m_output[0];
	const double yd = m_previous_output_deriv;
	const double dt = get_sample_duration();

	const double k1 = yd2(in, omega0, Q, y, yd);
	const double k2 = yd2(in, omega0, Q, y + (dt / 2.0) * yd, yd + (dt / 2.0) * k1);
	const double k3 = yd2(in, omega0, Q, y + (dt / 2.0) * yd + ((dt * dt) / 4.0) * k1, yd + (dt / 2.0) * k2);
	const double k4 = yd2(in, omega0, Q, y + dt * yd + ((dt * dt) / 2.0) * k2, yd + dt * k3);

	m_output[0] += dt * yd + ((dt * dt) / 6.0) * (k1 + k2 + k3);
	m_previous_output_deriv += (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

double lp2::yd2(
	const double in,
	const double omega0, const double Q,
	const double y, const double yd)
{
	return (omega0 * omega0) * (in - ((1.0 / (omega0 * Q)) * yd) - y);
}

//	HP2

hp2::hp2(const unsigned int channel_count)
	: sound_component("HP2", 3, 1, channel_count),
	  m_previous_output_deriv(this),
	  m_previous_input(this, 2)
{
	set_input_name("In", 0);
	set_input_name("Cutoff", 1);
	set_input_name("Reso", 2);
}

void hp2::initialize_process()
{
	m_previous_output_deriv = 0.0;
	m_previous_input[0] = 0.0;
	m_previous_input[1] = 0.0;
}

void hp2::process(const double input[])
{
	const double x = input[0];
	const double omega0 = 6.28318530718 * constrain_in_range(input[1], 1.0, 16000.0);
	const double Q = constrain_in_range(input[2], 0.6, 100.0);

	const double dt = get_sample_duration();
	const double y = m_output[0];
	const double yd = m_previous_output_deriv;
	const double xd2 = (x - 2.0 * m_previous_input[0] + m_previous_input[1]) / (dt * dt);

	const double k1 = yd2(xd2, omega0, Q, y, yd);
	const double k2 = yd2(xd2, omega0, Q, y + (dt / 2.0) * yd, yd + (dt / 2.0) * k1);
	const double k3 = yd2(xd2, omega0, Q, y + (dt / 2.0) * yd + ((dt * dt) / 4.0) * k1, yd + (dt / 2.0) * k2);
	const double k4 = yd2(xd2, omega0, Q, y + dt * yd + ((dt * dt) / 2.0) * k2, yd + dt * k3);

	m_output[0] += dt * yd + ((dt * dt) / 6.0) * (k1 + k2 + k3);
	m_previous_output_deriv += (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
	m_previous_input[1] = m_previous_input[0];
	m_previous_input[0] = x;
}

double hp2::yd2(
	const double xd2,
	const double omega0, const double Q,
	const double y, const double yd)
{
	return xd2 - omega0 * ((yd / Q) + omega0 * y);
}

//	BP2

bp2::bp2(const unsigned int channel_count)
	: sound_component("BP2", 3, 1, channel_count),
	  m_previous_output_deriv(this),
	  m_previous_input(this)
{
	set_input_name("In", 0);
	set_input_name("Cutoff", 1);
	set_input_name("Reso", 2);
}

void bp2::initialize_process()
{
	m_previous_output_deriv = 0.0;
	m_previous_input = 0.0;
}

void bp2::process(const double input[])
{
	const double x = input[0];
	const double omega0 = 6.28318530718 * constrain_in_range(input[1], 1.0, 16000.0);
	const double Q = constrain_in_range(input[2], 0.6, 100.0);

	const double dt = get_sample_duration();
	const double y = m_output[0];
	const double yd = m_previous_output_deriv;
	const double xd = (x - m_previous_input) / dt; //	TODO mieux

	const double k1 = yd2(xd, omega0, Q, y, yd);
	const double k2 = yd2(xd, omega0, Q, y + (dt / 2.0) * yd, yd + (dt / 2.0) * k1);
	const double k3 = yd2(xd, omega0, Q, y + (dt / 2.0) * yd + ((dt * dt) / 4.0) * k1, yd + (dt / 2.0) * k2);
	const double k4 = yd2(xd, omega0, Q, y + dt * yd + ((dt * dt) / 2.0) * k2, yd + dt * k3);

	m_output[0] += dt * yd + ((dt * dt) / 6.0) * (k1 + k2 + k3);
	m_previous_output_deriv += (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
	m_previous_input = x;
}

double bp2::yd2(
	const double xd,
	const double omega0, const double Q,
	const double y, const double yd)
{
	return (omega0 / Q) * (xd - (yd + Q * omega0 * y));
}

} // namespace Sound
} // namespace Gammou
