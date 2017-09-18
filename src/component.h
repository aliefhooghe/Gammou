#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <vector>
#include <string>

#include "gammou_exception.h"
#include "processor.h"



namespace Gammou {



template<class T> class component;

template<class T>
class component_link {
	friend class component<T>;

public:
	component_link() throw ();

private:
	void add_to_link_list(component_link *link) throw ();
	void notify_src_destruction() throw ();

	component<T> *m_src;
	unsigned int m_out_id;
	component_link *m_next_link;
	component_link *m_previous_link;
};


template<class T>
class component {

	class component_error : public error{
	public:
		virtual const char *what() const throw ()
		{
			return "Component generic error";
		}
	};


	class invalid_id : public component_error{
	public:
		invalid_id(const unsigned int id=0) throw () : m_id(id) {}
		virtual const char *what() const throw ()
		{
			return "Invalid component input/output id";
		}
	private:
		const unsigned int m_id;
	};

	class impossible_plug_pop :public component_error{
	public:
		virtual const char *what() const throw ()
		{
			return "No more Input/Output to pop";
		}
	};



public:
	component(const std::string& name, const unsigned int input_count,
			const unsigned int output_count);
	virtual ~component();

	const unsigned int get_input_count() const throw ();
	const unsigned int get_output_count() const throw ();

	const std::string get_name() const;
	const std::string get_input_name(const unsigned int input_id) const throw (invalid_id);
	const std::string get_output_name(const unsigned int output_id) const throw (invalid_id);

	void connect_to(const unsigned int output_id, component<T>* dst,
			const unsigned int dst_input_id) throw (invalid_id);

	component<T> *get_input_src(const unsigned int input_id,
			unsigned int& output_id) const throw (invalid_id);

protected:
	void pop_input() throw (impossible_plug_pop);
	void push_input(const std::string& name);
	void push_input();

	void set_input_name(const std::string& name, const unsigned int input_id) throw (invalid_id);
	void set_output_name(const std::string& name, const unsigned int output_id) throw (invalid_id);


private:
	void connect_to_link(component_link<T> *link, const unsigned int output_id);
	void disconnect_link(component_link<T> *link);
	const std::string input_default_name(const unsigned int input_id);
	const std::string output_default_name(const unsigned int output_id);

	std::vector<component_link<T>> m_input;
	component_link<T> *m_first_link;

	std::vector<std::string> m_input_name;
	std::vector<std::string> m_output_name;
	std::string m_name;
};





} /* namespace Gammou */

#endif /* COMPONENT_H_ */
