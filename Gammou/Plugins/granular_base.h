

#include "plugin_helper.h"

namespace Gammou {

    namespace Sound {

        struct grain
        {
            double phase;
            double output_signal_position;
        };

        class granular_base
        {

        public:
            granular_base(polyphonic_sound_component *owner, const unsigned int grain_count);
            virtual ~granular_base() {}

            void init();

            double compute_step(
                const double phase_seed,
                const double phase_width,
                const double radius,
                const double env_attack,
                const double env_release,
                const double pitch,
                const double dt);

        protected:
            virtual double grain_value(const double phase) = 0;

        private:
            double grain_env(
                const double t,
                const double env_attack,
                const double env_release);

            double gen_grain_phase(
                const double phase_seed,
                const double phase_width);

            multi_channel_array<grain> m_grain;
            multi_channel_variable<double> m_time;
            multi_channel_variable<double> m_first_grain_time;
        };

    }   /* Sound */

}   /* Gammou */



