#ifndef DATA_STREAM_H_
#define DATA_STREAM_H_

namespace Gammou {

	namespace Sound {

		class data_source {
		public:
			virtual unsigned int read(void *data, const unsigned int size) = 0;
		};

		class data_destination {
		public:
			virtual unsigned int write(const void *data, const unsigned int size) = 0;
		};

	} /* Sound */

} /* Gammou */

#endif