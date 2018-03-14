#ifndef DATA_STREAM_H_
#define DATA_STREAM_H_

namespace Gammou {

	namespace Sound {

		// C style read-write 
		
		class data_stream {
			
		public:
			enum class seek_mode { CURRENT, SET, END };

			virtual ~data_stream() {}
			virtual bool seek(const int offset, seek_mode mode = seek_mode::CURRENT) =0;
			virtual unsigned int tell() =0;
		};

		class data_source : public virtual data_stream {
		public:
			virtual ~data_source() {}
			virtual unsigned int read(void *data, const unsigned int size) =0;
		};

		class data_sink : public virtual data_stream {
		public:
			virtual ~data_sink() {}
			virtual unsigned int write(void *data, const unsigned int size) =0;
		};

		// todo : bidirectionel ? (Maybe useless in Sound)

	} /* Sound */

} /* Gammou */

#endif