#ifndef USER_SOUND_COMPONENT_H_
#define USER_SOUND_COMPONENT_H_

#include <sound_component/abstract_sound_component.h>
#include <sound_component/sound_component_manager.h>
#include <gammou_process.h>

#include "user_component_gui_circuit.h"


namespace Gammou {

    namespace Gui {

        class user_component_manager :
            public Sound::abstract_sound_component_manager {

            public:
                user_component_manager();
                ~user_component_manager();

                const unsigned int *get_current_working_channel_ref() const override;
                double get_current_sample_rate() const override;

                void set_current_working_channel_ref(const unsigned int *channel_ref);
                void set_current_samplerate(const double sample_rate);

            private:
                const unsigned int *m_channel_ref{};
                double m_current_sample_rate;
        };

        class user_sound_component_output :
            public Process::abstract_component<double> {

            public:
                user_sound_component_output(const unsigned int output_count);
                ~user_sound_component_output() {}

                //  abstract component override
                double fetch_output(const unsigned int output_id) override;
                void process(const double input[]) override;
                void initialize_process() override;

                //  allow inputs name change
                using abstract_component<double>::set_input_name;
        };

        class user_sound_component :
            public Sound::abstract_sound_component {

            public:
                user_sound_component(
                        const std::string& name,
                        const unsigned int input_count,
                        const unsigned int output_count,
                        const unsigned int channel_count,
                        gui_component_main_factory& factory);
                ~user_sound_component() {}

                void add_sound_component(abstract_sound_component *component);

                //  abstract_component<double> override
                double fetch_output(const unsigned int output_id) override;
                void process(const double input[]) override;
                void initialize_process() override;

                //  abstract_sound_component override
                void set_working_channel_ref(const unsigned int *channel_ref) override;
                void on_sample_rate_change(const double samplerate) override;
                unsigned int save_state(Sound::data_output_stream& data) override;
                unsigned int get_channel_count() const override;

                //
                Process::abstract_component<double>& get_input();
                Process::abstract_component<double>& get_output();

                //  Editing
                View::widget& get_gui_circuit();
                bool load_state(Sound::data_input_stream& data);
                void reset_content();

            private:
                const unsigned int m_channel_count;

                Process::bytecode_frame_processor<double> m_processor;
                Process::abstract_process_frame<double> m_frame;

                Process::vector_fetcher_component<double> m_input_component;
                user_sound_component_output m_output_component;

                user_component_manager m_component_manager;
                user_component_gui_circuit m_gui_circuit;
        };

    }   /*  Gui*/

} /* Gammou */


#endif  /*  USER_SOUND_COMPONENT */
