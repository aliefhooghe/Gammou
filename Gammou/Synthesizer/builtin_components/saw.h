#ifndef GAMMOU_SAW_H_
#define GAMMOU_SAW_H_


#include <cmath>
#include <limits>
#include <algorithm>

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"

#include "builtin_components_ids.h"

#define F0 21000.0
#define PI_CST 3.14159265359

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			// A netoyer

			class saw : public sound_component {

			public:
				saw(const unsigned int channel_count)
					: 
					sound_component("Saw", 1, 1, channel_count),
					m_time(this),
					m_next_dirac_time(this),
					m_slope(this)
				{
					DEBUG_PRINT("Saw CTOR\n");
					set_input_name("Freq", 0);
				}

				~saw() {}

				void initialize_process() override
				{
					m_next_dirac_time = 0.0; 
					m_slope = 1.0;
					m_time = 0.0;
					m_output[0] = 0.0; 
				}

				void process(const double input[]) override
				{
					const double f = input[0];
					const double f_estim = f;// (f + 1.5 * (f - m_f_prev));
					const double t = m_time;
					const double dt = get_sample_duration();

					update_dirac_time(f_estim);

					const double R = 0.995;
					const double dI = integral(t, dt);
					const double out = dI + R * m_output[0];
					 // y(n) = x(n) - x(n-1) + R * y(n-1)
					 // = y(n) = integral(t, dt) + R * y(n-1)
	
					m_output[0] = out;


					m_time += dt;
				//	m_f_prev = f;
				}

			private:

				double dirac_filtre(const double t, const double f0)
				{
					if (std::abs(t) <= std::numeric_limits<float>::min()) {
						return 2.0 * f0;
					}
					else {
						const double v = PI_CST * t;
						return sin(2.0 * f0 * v) / v;
					}
				}

				void update_dirac_time(const double f)
				{
					if (m_time >= m_next_dirac_time) {
						const double delta_t = m_time - m_next_dirac_time;

						m_time = delta_t;// m_next_dirac_time;
						//m_integral = 0.0;// integral(m_next_dirac_time, delta_t); // recentrage

						m_next_dirac_time = (1.0 / f);
						m_slope = f;// *(delta_integral * 0.5 + 1.0);
					}
				}

				inline double derive(const double t)
				{
					return 2.0  * (m_slope -
							(
							dirac_filtre(t, F0) +
							dirac_filtre(t - m_next_dirac_time, F0)
							));
				}

				inline double integral(const double t, const double T)
				{
					double a, b, m;
					a = derive(t);
					m = derive(t + T * 0.5);
					b = derive(t + T);
					return (T / 6.0) * (a + 4.0 * m + b);
				}

				multi_channel_variable<double> m_time;
				multi_channel_variable<double> m_next_dirac_time;
				multi_channel_variable<double> m_slope;
			//	multi_channel_variable<double> m_integral;
			//	multi_channel_variable<double> m_prev_output;
			};

			class saw_factory : public default_plugin_factory<saw> {

			public:
				saw_factory()
					: default_plugin_factory<saw>("Saw", "Oscillator", saw_component_id) {}
				~saw_factory() {}
			};


		} /* Builtin */

	} /* Sound */

} /* Gammou */








#endif