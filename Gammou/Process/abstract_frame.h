#ifndef ABSTRACT_FRAME_H_
#define ABSTRACT_FRAME_H_

#include "abstract_component.h"


#define FRAME_INITIAL_MEM 16

namespace process{


template<class T>
class abstract_frame{


	class instruction{
		enum class type {FETCH_OUTPUT, FETCH_DEFAULT, PROCESS};

	public:

		static instruction fetch_output(abstract_component<T> *component, const unsigned int output_id, const unsigned int mem_pos)
		{
			return instruction(component, output_id, mem_pos, type::FETCH_OUTPUT);
		}

		static instruction fetch_default(const unsigned int mem_pos)
		{
			return instruction(nullptr, 0, mem_pos, type::FETCH_DEFAULT);
		}

		static instruction process(abstract_component<T> *component)
		{
			return instruction(component, 0, 0, type::PROCESS);
		}

		inline void execute(std::vector<T>& memory) const
		{
			if( m_type == type::PROCESS )
				m_component->process(memory.data());
			else if( m_type == type::FETCH_OUTPUT )
				memory[m_mem_pos] = m_component->fetch_output(m_output_id);
			else
				memory[m_mem_pos] = T();	
		}

		~instruction() {};

	private:
		instruction(abstract_component<T> *const component,
				const unsigned int output_id,
				const unsigned int mem_pos,
				const type type)
			: m_component(component), m_output_id(output_id), m_mem_pos(mem_pos), m_type(type) {}


		abstract_component<T> *const m_component;
		const unsigned int m_output_id;
		const unsigned int m_mem_pos;
		const type m_type;
	};


public:
	abstract_frame();
	virtual ~abstract_frame();

	void add_component(abstract_component<T> *component);
	virtual void notify_circuit_change() =0;
	void initialize_components();

protected:
	void next_process_cycle();
	void make_component_current_cycle_program(abstract_component<T> *component);
	void execute_program();

private:
	void clear_program();

	std::vector<instruction> m_program;
	std::vector<abstract_component<T>*> m_component_to_initialize;
	std::vector<T> m_program_memory;

	subject<abstract_frame<T> > m_link_monitor;

	unsigned int m_process_cycle;
};



template<class T>
abstract_frame<T>::abstract_frame()
	: m_program(),
	  m_component_to_initialize(),
	  m_program_memory(FRAME_INITIAL_MEM),
	  m_link_monitor(this),
	  m_process_cycle(42)
{
}

template<class T>
abstract_frame<T>::~abstract_frame()
{

}

template<class T>
void abstract_frame<T>::add_component(abstract_component<T> *component)
{
	if( component->get_frame() != nullptr)
		throw std::domain_error("Can't add component that is already on a frame");

	m_link_monitor.register_observer(&(component->m_frame));
}

template<class T>
void abstract_frame<T>::initialize_components()
{
	for(auto it = m_component_to_initialize.begin();
			it != m_component_to_initialize.end(); ++it)
		(*it)->initialize_process();
}

template<class T>
void abstract_frame<T>::next_process_cycle()
{
	clear_program();
	m_process_cycle++;
}

template<class T>
void abstract_frame<T>::clear_program()
{
	m_program.clear();
	m_component_to_initialize.clear();
}

template<class T>
void abstract_frame<T>::make_component_current_cycle_program(abstract_component<T> *component)
{
	if( component->update_process_cyle(m_process_cycle) ){
		const unsigned int ic = component->get_input_count();

		m_component_to_initialize.push_back(component);

		if( ic != 0){ // if there are inputs
			abstract_component<T> *src_component[ic];
			unsigned int src_output_id[ic];

			//	Update memory size if needed
			if( ic > m_program_memory.size() )
				m_program_memory.resize(ic);

			// Get input connections info
			//	And Make program for all input components process

			for(unsigned int i = 0; i < ic; ++i){
				src_component[i] = component->get_input_src(i, src_output_id[i]);

				if(src_component[i] != nullptr)
					make_component_current_cycle_program(src_component[i]);
			}

			//	Make 'fetch input' program

			for(unsigned int i = 0; i < ic; ++i){
				if( src_component[i] != nullptr )
					m_program.push_back(instruction::fetch_output(src_component[i], src_output_id[i], i));
				else
					m_program.push_back(instruction::fetch_default(i));
			}

		}

		//	Make process program

		m_program.push_back(instruction::process(component));
	}
}

template<class T>
void abstract_frame<T>::execute_program()
{
	for( auto it = m_program.begin();
			it != m_program.end(); ++it)
		it->execute(m_program_memory);
}

} /* Gammou */

#endif
