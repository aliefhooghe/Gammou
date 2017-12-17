#ifndef REQUEST_FORM_H_
#define REQUESGT_FORM_H_

#include <string>
#include <vector>

namespace Gammou {

	namespace Sound {

		enum class request_form_field_type {FILE_PATH, INTEGER, ENUM};

		class request_descriptor {
		public:
			request_descriptor(const std::string& name, const std::string& description);
			virtual ~request_descriptor() {}

			const std::string& get_name() const;
			const std::string& get_description() const;
		private:
			const std::string m_name;
			const std::string m_description;
		};

		class file_request_descriptor : public request_descriptor {

		public:
			file_request_descriptor(const std::string& name, const std::string& description, const std::string& extension);

			const std::string& get_extension() const;
		private:
			const std::string m_extension;
		};

		class integer_request_descriptor : public request_descriptor {

		public:
			integer_request_descriptor(const std::string& name, const std::string& description, const int min, const int max);

			int get_min() const;
			int get_max() const;
		private:
			const int m_min;
			const int m_max;
		};

		// todo class enum_request_descriptor

		
		/////

		class request_form_descriptor {

		public:
			request_form_descriptor();

			unsigned int get_request_count() const;
			void add_file_request(const std::string& name, const std::string& description, const std::string& ext);
			void add_int_request(const std::string& name, const std::string& description, const int min, const int max);

			const request_form_field_type get_field_type(const unsigned int index) const;
			const file_request_descriptor& get_file_request(const unsigned int index) const;
			const integer_request_descriptor& get_integer_request(const unsigned int index) const;

		private:
			std::vector<std::pair<request_form_field_type, unsigned int> > m_index;
			std::vector<file_request_descriptor> m_file_request;
			std::vector<integer_request_descriptor> m_integer_request;
		};



		
	} /* Sound */

} /* Gammou */

#endif 