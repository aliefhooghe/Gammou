#ifndef DATA_STREAM_H_
#define DATA_STREAM_H_

namespace Gammou {

	namespace Sound {

		// C style read-write 
		
        class abstract_data_stream {
			
		public:
			enum class seek_mode { CURRENT, SET, END };

            virtual ~abstract_data_stream() {}
			virtual bool seek(const int offset, seek_mode mode = seek_mode::CURRENT) =0;
			virtual unsigned int tell() =0;
		};

        class data_input_stream : public virtual abstract_data_stream {
		public:
            virtual ~data_input_stream() {}
			virtual unsigned int read(void *data, const unsigned int size) =0;
		};

        class data_output_stream : public virtual abstract_data_stream {
		public:
            virtual ~data_output_stream() {}
			virtual unsigned int write(void *data, const unsigned int size) =0;
		};


        class data_stream :
                public data_input_stream,
                public data_output_stream {};


	} /* Sound */

} /* Gammou */

#endif
