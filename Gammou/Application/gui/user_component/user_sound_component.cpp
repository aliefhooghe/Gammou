
#include "user_sound_component.h"
#include "gui_properties.h"

namespace Gammou {

	namespace Gui  {

		/*
		 *      user_component_manager implementation
		 */

		user_component_manager::user_component_manager()
		:	m_current_sample_rate(Sound::DEFAULT_SAMPLE_RATE)
		{
		}

		user_component_manager::~user_component_manager()
		{
		}

		const unsigned int *user_component_manager::get_current_working_channel_ref() const
		{
			return m_channel_ref;
		}

		double user_component_manager::get_current_sample_rate() const
		{
			return m_current_sample_rate;
		}

		void user_component_manager::set_current_working_channel_ref(const unsigned int *channel_ref)
		{
			m_channel_ref = channel_ref;
			notify_working_channel_ref_change();
		}

		void user_component_manager::set_current_samplerate(const double sample_rate)
		{
			m_current_sample_rate = sample_rate;
			notify_sample_rate_change();
		}

		/*
		 *      user_component_output implementation
		 */

		user_sound_component_output::user_sound_component_output(
				const unsigned int output_count)
		:	Process::abstract_component<double>(
				"Output", output_count, 0)
		{
		}

		double user_sound_component_output::fetch_output(
				const unsigned int output_id)
		{
            return 0.0; // a stub
		}

		void user_sound_component_output::process(const double input[])
		{
			//	a stub
		}

		void user_sound_component_output::initialize_process()
		{
			//	a stub
		}

        /*
         *      user_sound_component implementation
         */

		user_sound_component::user_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count,
                const unsigned int channel_count,
                gui_component_main_factory& factory)
		:	Sound::abstract_sound_component(
				name, input_count, output_count),
            m_channel_count(channel_count),
			m_frame(m_processor),
			m_input_component("Input", input_count),
			m_output_component(output_count),
            m_gui_circuit(
                factory, *this,
                0, GuiProperties::user_component_editor_toolbox_height,
                GuiProperties::user_component_editor_circuit_width,
                GuiProperties::user_component_editor_circuit_height)
        {
			m_frame.add_component(&m_input_component);
			m_frame.add_component(&m_output_component);
			m_frame.add_component_to_output_list(&m_output_component);

			//	Naming

			for(unsigned int i = 0; i < input_count; ++i)
				m_input_component.set_output_name(
					Process::abstract_component<double>::default_input_name(i), i);
            for (unsigned int i = 0; i < output_count; ++i)
				m_output_component.set_input_name(
					Process::abstract_component<double>::default_output_name(i), i);
		}

		void user_sound_component::add_sound_component(
				abstract_sound_component *component)
        {
			if (component->get_channel_count() != m_channel_count)
				throw std::range_error("user_sound_component : Component channel count doesnt match");

			m_frame.add_component(component);
            m_component_manager.register_sound_component(component);
        }

        double user_sound_component::fetch_output(const unsigned int output_id)
        {
			unsigned int component_output_id;

			//	This can be a little bit confusing :
			//	Here output_id is an input id for the outpout component;
			Process::abstract_component<double> *component =
				m_output_component.get_input_src(output_id, component_output_id);

			if (component == nullptr)
				return 0.0;
			else
				return component->fetch_output(component_output_id);
		}

        void user_sound_component::process(const double input[])
        {
			m_input_component.set_input_buffer_ptr(input);
			m_frame.process();
        }

        void user_sound_component::initialize_process()
        {
            m_frame.initialize_components();
        }

        void user_sound_component::set_working_channel_ref(const unsigned int *channel_ref)
        {
			m_component_manager.set_current_working_channel_ref(channel_ref);
        }

        void user_sound_component::on_sample_rate_change(const double samplerate)
        {
            m_component_manager.set_current_samplerate(samplerate);
        }

        unsigned int user_sound_component::save_state(Sound::data_output_stream& data)
        {
            const unsigned int start = data.tell();

            //  Save input count
            unsigned int count = get_input_count();
            data.write(&count, sizeof(count));

            //  Save output count
            count = get_output_count();
            data.write(&count, sizeof(count));

            //  Save circuit state
            m_gui_circuit.save_state(data);
            return data.tell() - start;
        }

        unsigned int user_sound_component::get_channel_count() const
        {
            return m_channel_count;
        }

		Process::abstract_component<double>& user_sound_component::get_input()
		{
			return m_input_component;
		}

		Process::abstract_component<double>& user_sound_component::get_output()
		{
			return m_output_component;
		}

        View::widget& user_sound_component::get_gui_circuit()
        {
            return m_gui_circuit;
        }

        void user_sound_component::select_component_creation_factory_id(const unsigned int factory_id)
        {
            m_gui_circuit.select_component_creation_factory_id(factory_id);
        }

        bool user_sound_component::load_circuit_state(Sound::data_input_stream& data)
        {
            return m_gui_circuit.load_state(data);
        }

        void user_sound_component::reset_content()
        {
            m_gui_circuit.reset_content();
        }

	}   /*  Gui */

} /* Gammou */
