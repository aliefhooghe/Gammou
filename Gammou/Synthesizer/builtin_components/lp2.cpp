
#include "lp2.h"

namespace Gammou {

	namespace Sound {

		lp2::lp2(const unsigned int channel_count)
			: sound_component("Lp2", 3, 1, channel_count),
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
			const double k3 = yd2(in, omega0, Q, y + (dt / 2.0) * yd + ((dt* dt) / 4.0) * k1, yd + (dt / 2.0) * k2);
			const double k4 = yd2(in, omega0, Q, y + dt * yd + ((dt*dt) / 2.0) * k2, yd + dt * k3);

			m_output[0] += dt * yd + ((dt * dt) / 6.0) * (k1 + k2 + k3);
			m_previous_output_deriv += (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
		}

	}
}

