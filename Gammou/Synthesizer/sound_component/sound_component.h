#ifndef SOUD_MODULE_H_
#define SOUD_MODULE_H_

#include "gammou_process.h"

#include "sound_component_manager.h"
#include "multi_channel_data_container.h"
#include "../plugin/data_stream.h"

/*
 * 		Channels data types
 */

namespace Gammou {

	namespace Sound {

		// Todo verifier tout ca 
		constexpr double DEFAULT_SAMPLE_RATE = 44100.0;
		constexpr double DEFAULT_SAMPLE_DURATION = 1.0 / DEFAULT_SAMPLE_RATE;
		constexpr unsigned int NO_FACTORY = 0xFFFFFFFF;

		class abstract_sound_component : public Process::abstract_component<double>,
											public Process::observer<sound_component_manager, sound_component_notification_tag>  {

			friend class abstract_plugin_factory;

		public:
			abstract_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count);

			virtual ~abstract_sound_component() {}

			unsigned int get_factory_id() const;

		protected:
			virtual void on_sample_rate_change(const double new_sample_rate) {};
			virtual void on_channel_change(const unsigned int new_chanel) {};
			
			double get_sample_duration() const;
			double get_sample_rate() const;
		private:
			void on_notify(const sound_component_notification_tag notification_tag) override;

			double m_sample_rate;
			double m_sample_duration;
			unsigned int m_factory_id;
		};


		// class polyphonic_sound_component
		class polyphonic_sound_component : public abstract_sound_component {

			friend class multi_channel_data;
			

		public:
			polyphonic_sound_component(
								const std::string& name,
								const unsigned int input_count,
								const unsigned int output_count,
								const unsigned int channel_count);

			virtual ~polyphonic_sound_component() {}

			unsigned int get_channel_count() const;
			unsigned int get_current_working_channel() const;
			

			virtual unsigned int save_state(data_destination& data) { return 0; };

		private:
			const unsigned int m_channels_count;
			unsigned int m_current_working_channel;

			double m_sample_rate;
			double m_sample_duration;
			unsigned int m_factory_id;
		};



		////-///

		class sound_component : public polyphonic_sound_component {

		public:

			sound_component(const std::string& name,
					const unsigned int input_count,
					const unsigned int output_count,
					const unsigned int channel_count);
			virtual ~sound_component();


			//	not virtual : if another behaviour is needed, inherit from abstract_sound_component
			double fetch_output(const unsigned int output_id) override;
		protected:
			multi_channel_array<double> m_output;

		};


	} /* Sound */


} /* Gammou */




#endif /* SOUD_MODULE_H_ */
