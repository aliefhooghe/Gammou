#ifndef ABSTRACT_PLUGIN_FACTORY_H_
#define ABSTRACT_PLUGIN_FACTORY_H_

#include <string>

#include "../../debug.h"
#include "request_form.h"
#include "../sound_component/sound_component.h"
#include "data_stream.h"

namespace Gammou {

    namespace Sound {

		class abstract_plugin_factory {

		public:
			abstract_plugin_factory(
				const std::string& name, 
				const std::string& category, 
				unsigned int factory_id);
            virtual ~abstract_plugin_factory() {}

			const std::string& get_name() const;
			const std::string& get_category() const;
			unsigned int get_factory_id() const;

			const request_form& get_request_form();
			virtual abstract_sound_component *get_new_sound_component(data_input_stream& source, const unsigned int channel_count) = 0;
			virtual abstract_sound_component *get_new_sound_component(const answer_form& answers, const unsigned int channel_count) = 0;

			virtual void delete_sound_component(abstract_sound_component*component) const = 0;

		protected:
			void stamp_sound_component(abstract_sound_component *component) const;
			virtual std::unique_ptr<request_form> create_plugin_request_form();

		private:
			std::unique_ptr<request_form> m_plugin_request_form;
	
			const std::string m_name;
			const std::string m_category;
			const unsigned int m_factory_id;
		};

        class plugin_factory : public abstract_plugin_factory {

        public:
			plugin_factory(const std::string& name, const std::string& category, unsigned int factory_id);
			virtual ~plugin_factory() {}

			abstract_sound_component *get_new_sound_component(data_input_stream& source, const unsigned int channel_count) override;
			abstract_sound_component *get_new_sound_component(const answer_form& answer, const unsigned int channel_count) override;

			void delete_sound_component(abstract_sound_component*component) const override;

		protected:
			virtual abstract_sound_component *create_sound_component(data_input_stream& source, const unsigned int channel_count) = 0;
			virtual abstract_sound_component *create_sound_component(const answer_form& answer_form, const unsigned int channel_count) = 0;

        };

		// Default implementation

		template<class sound_component_type>
		class default_plugin_factory : public plugin_factory {
		public:
			default_plugin_factory(
				const std::string& name, 
				const std::string &category, 
				unsigned int factory_id)
				: plugin_factory(name, category, factory_id) {}
			virtual ~default_plugin_factory() {}

		protected:
			virtual abstract_sound_component *create_sound_component(
				data_input_stream& source, 
				const unsigned int channel_count) override
			{
				return new sound_component_type(channel_count);
			}

			virtual abstract_sound_component *create_sound_component(
				const answer_form& answer_form, 
				const unsigned int channel_count) override
			{
				return new sound_component_type(channel_count);
			}

		};
		
    } /* Sound */

} /* Gammou */



#endif
