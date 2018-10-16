#ifndef GAMMOU_SYNTHESIZER_PERSISTENCE_H_
#define GAMMOU_SYNTHESIZER_PERSISTENCE_H_

#include <cstdint>
#include <cstring>

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

		// Current Gammou Format
        constexpr uint32_t gammou_format_version_id = 1u;
        constexpr auto gammou_magic = "GAMMOU";

        //  Data structures

		// Size = 16 Bytes
		PACKED(
        struct link_state
		{
			uint32_t src_record_id;
			uint32_t output_id;
			uint32_t dst_record_id;
			uint32_t input_id;
		}
		);

		// Size = 16 Bytes
		PACKED(
        struct component_state_header{
			uint32_t factory_id;
			int32_t gui_x_pos;
			int32_t gui_y_pos;
			uint32_t data_size;
		}
		);

		// Size = 8 Bytes
		PACKED(
        struct circuit_state_header {
			uint32_t component_count;
			uint32_t link_count;
		}
		);

		// Size = 12 Bytes
		PACKED(
        struct synthesizer_state_header {
			char magic[6];
			uint32_t format_version_id;
			uint32_t parameter_count;
		}
		);

        PACKED(
        struct gammou_file_header {
            char magic[6];
            uint16_t format_version_id;
            uint16_t content_type;
        }
        );


        //------

        template<class content>
        struct content_type {};

        template<class content>
        class gammou_file {

            public:
                static void load(Sound::data_input_stream& source, content& state)
                {
                    //  Reading header
                    gammou_file_header header;
                    source.read(header);

                    if (!(check_header(header) && header.content_type == content_type<content>::value))
                        throw std::runtime_error("Invalid gammou file header");

                    //  Loading content
                    state.load(source);
                }

                static void save(Sound::data_output_stream& dest, content& state)
                {
                    //  Filling header
                    gammou_file_header header;
                    std::memcpy(header.magic, gammou_magic, 6);
                    header.format_version_id = gammou_format_version_id;
                    header.content_type = content_type<content>::value;

                    //  Writing header
                    dest.write(header);

                    // Writing Content
                    state.save(dest);
                }

            private:
                static bool check_header(const gammou_file_header& header){
                    return
                        std::strncmp(header.magic, gammou_magic, 6) == 0 &&
                        header.format_version_id == gammou_format_version_id;
                }
        };

        //-------

        struct component_state {

            public:
                explicit component_state(Sound::data_input_stream& source);
                component_state() = default;

                uint32_t factory_id;
                int32_t x_pos;
                int32_t y_pos;
                std::vector<uint8_t> data;

                void load(Sound::data_input_stream& source);
                void save(Sound::data_output_stream& dest);
        };

        struct circuit_state {

            public:
                explicit circuit_state(Sound::data_input_stream& source);
                circuit_state() = default;

                std::vector<component_state> components;
                std::vector<link_state> links;

                void load(Sound::data_input_stream& source);
                void save(Sound::data_output_stream& dest);
        };

        struct gammou_state {

            public:
                explicit gammou_state(Sound::data_input_stream& source);
                gammou_state() = default;

                std::vector<double> parameters;
                double master_volume;
                bool polyphonic_keyboard;
                circuit_state master_circuit;
                circuit_state polyphonic_circuit;

                void load(Sound::data_input_stream& source);
                void save(Sound::data_output_stream& dest);
        };

		constexpr auto gammou_state_reserved_size = 64u;

        template<>
        struct content_type<gammou_state> {
            static const unsigned int value = 0;
        };

		//--------

		class buffer_output_stream :
				public Sound::data_output_stream {

		public:
			buffer_output_stream(
				std::vector<uint8_t>& buffer,
				const unsigned int start_cursor = 0u);
			buffer_output_stream(buffer_output_stream&) = delete;
			~buffer_output_stream();

			bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int write(void *data, const unsigned int size) override;

		private:
			unsigned int m_cursor;
			std::vector<uint8_t>& m_buffer;
		};

		//------

		class buffer_input_stream :
				public Sound::data_input_stream {

			public:
				buffer_input_stream(
					const std::vector<uint8_t>& buffer,
					const unsigned int start_cursor = 0u);
				buffer_input_stream(buffer_input_stream&) = delete;
				~buffer_input_stream();

				bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
				unsigned int tell() override;
				unsigned int read(void *data, const unsigned size) override;

			private:
				unsigned int m_cursor;
                const std::vector<uint8_t>& m_buffer;
		};

		//------
		
        class constrained_input_stream : public Sound::data_input_stream {

		public:
            constrained_input_stream(Sound::data_input_stream& data, const unsigned int max_forward_offset);
			
			bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int read(void *data, const unsigned int size) override;

		private:
			const unsigned int m_start_offset;
			const unsigned int m_max_forward_offset;
			Sound::data_input_stream& m_data;
		};

		//-----

		// Dummy factory Id fo Internals Components
		const unsigned int INTERNAL_FACTORY_ID = 0x00000000;

	} /* Persistence */


} /* Gammou */


#endif
