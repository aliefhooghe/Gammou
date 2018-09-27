#include "adsr_env.h"

#define M_DEFAULT_GATE_FACT 1.001

namespace Gammou {
    namespace Sound {
        namespace Builtin{

            adsr_env::adsr_env(const unsigned int channel_count)
                : sound_component("AdsrEnv", 5, 1, channel_count),
					m_gate_fact(this),
					m_tau_index(this)
            {
                set_input_name("Attack", 0);
				set_input_name("Decay", 1);
				set_input_name("Sustain", 2);
				set_input_name("Release", 3);
				set_input_name("Gate", 4);
            }

            void adsr_env::initialize_process()
            {
                m_gate_fact = M_DEFAULT_GATE_FACT; // 
				m_output[0] = 0.0;
				m_tau_index = 0; // attack
            }

            void adsr_env::process(const double input[])
            {
                if (input[4] > 0.5) { // gate on
						if (m_tau_index == 0){ // on attack
							if (m_output[0] >= 1.0) { // attack -> decay
								m_tau_index = 1;
								m_gate_fact = input[2]; // sustain
							}
						}
						else if (m_tau_index == 3) { // on release
							m_tau_index = 0; // goto attack
							m_gate_fact = M_DEFAULT_GATE_FACT;
						}
				}
				else if(m_tau_index != 3 ){
					m_tau_index = 3; // goto release
				}

				const double gate = m_gate_fact * input[4];
				const double tau = input[m_tau_index];
				const double fact = tau / get_sample_duration();
					
				m_output[0] = (gate + fact * m_output[0]) / (1.0 + fact);
            }
        }
    }
}
