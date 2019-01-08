
#include <random>
#include "granular_base.h"
namespace Gammou
{
    namespace Sound
    {

        granular_base::granular_base(
            polyphonic_sound_component *owner, 
            const unsigned int grain_count)
        :   m_grain(owner, grain_count),
            m_time(owner),
            m_first_grain_time(owner)
        {
        }

        void granular_base::init()
        {
            for (unsigned int i = 0; i < m_grain.size(); i++)
            {
                auto &grain = m_grain[i];
                grain.output_signal_position = -10.0;
                grain.phase = 0.0;
            }

            m_time = 0.0;
            m_first_grain_time = 0.0;
        }

        double granular_base::compute_step(
            const double phase_seed,
            const double phase_width,
            const double radius,
            const double env_attack,
            const double env_release,
            const double pitch,
            const double dt)
        {
            double out = 0.0;

            for (unsigned int i = 0; i < m_grain.size(); i++)
            {
                auto &gr = m_grain[i];

                const double env_cursor =
                    m_time - gr.output_signal_position;

                out += grain_value(gr.phase) * grain_env(env_cursor, env_attack, env_release);
                gr.phase += pitch * dt;

                if (env_cursor > ((double)m_grain.size() * radius * 0.5))
                {
                    m_first_grain_time += radius;
                    gr.output_signal_position = m_first_grain_time;

                    const double phase_org = gen_grain_phase(phase_seed, phase_width);
                    gr.phase = phase_org + (m_time - gr.output_signal_position) * pitch;
                    //  pb : les grains sont moin locaux quand le pitch augmente
                }
            }

            m_time += dt;
            return out;
        }

        double granular_base::grain_env(
            const double t,
            const double env_attack,
            const double env_release)
        {
            if (t < 0.0)
                return exp(-t * t / (2.0 * env_attack * env_attack));
            else
                return exp(-t * t / (2.0 * env_release * env_release));
        }

        double granular_base::gen_grain_phase(
            const double phase_seed,
            const double phase_width)
        {
            static std::default_random_engine m_engine{};
            static std::uniform_real_distribution<double> m_distribution{-1, 1};
            return phase_seed + phase_width * m_distribution(m_engine);
        }

    } /* Sound */
} /* Gammou */