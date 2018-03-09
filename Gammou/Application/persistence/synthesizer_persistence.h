#ifndef GAMMOU_SYNTHESIZER_PERSISTENCE_H_
#define GAMMOU_SYNTHESIZER_PERSISTENCE_H_

#include <cstdint>
#include <vector>

#include "../../Synthesizer/plugin_management/data_stream.h"

// Trick from gist.github.com/PhilCK/1534763
#ifdef __GNUC__
#define PACKED(struct_decl) struct_decl __attribute__((__packed__))
#else
#define PACKED(struct_decl) __pragma( pack(push, 1) ) struct_decl __pragma( pack(pop) )
#endif

namespace Gammou {

	namespace Persistence {

		// Size = 16 Bytes
		PACKED(
		struct link_record
		{
			uint32_t src_record_id;
			uint32_t output_id;
			uint32_t dst_record_id;
			uint32_t input_id;
		}
		);

		// Size = 16 Bytes
		PACKED(
		struct component_record_header{
			uint32_t factory_id;
			int32_t gui_x_pos;
			int32_t gui_y_pos;
			uint32_t data_size;
		}
		);

		// Size = 8 Bytes
		PACKED(
		struct circuit_record_header {
			uint32_t component_count;
			uint32_t link_count;
		}
		);


		//--------


		class buffer_data_sink : public Sound::data_sink{

		public:
			buffer_data_sink();
			buffer_data_sink(buffer_data_sink&) = delete;
			~buffer_data_sink();

			bool seek(const unsigned int offset, Sound::data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int write(void *data, const unsigned int size) override;

			void flush_data(Sound::data_sink& target);

		private:
			unsigned int m_cursor;
			unsigned int m_data_size;
			std::vector<uint8_t> m_buffer;
		};

		//-----

		const uint32_t INTERNAL_COMPONENT_ID_MASK = 0xFFFFFF00;

		bool record_id_is_internal_component(const uint32_t component_record_id);
		uint8_t internal_component_id_by_record_id(const uint32_t component_record_id);
		uint32_t record_id_by_internal_component_id(const uint8_t internal_component_id);


	} /* Persistence */

} /* Gammou */


#endif