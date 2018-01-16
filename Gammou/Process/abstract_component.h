#ifndef ABSTRACT_COMPONENT_H_
#define ABSTRACT_COMPONENT_H_

#include <vector>
#include <string>
#include <stdexcept>

#include "observer.h"

namespace Gammou {

	namespace Process{

		template<class T> class abstract_frame;
		template<class T> class abstract_component;


		/*! \class component_link
		*  \brief Describe a component input as a link to another component output
		*/
		template<class T>
		class component_link: public observer<abstract_component<T> > {

		public:
			component_link();
			~component_link() {}

			void set_src_output_id(const unsigned int output_id);
			unsigned int get_src_output_id() const;
			void on_notify(const unsigned int popped_output_id) override;

		private:
			unsigned int m_src_output_id;
		};

		/*!	\class frame_link
		* 	\brief Describe the component's link to his frame
		*/
		template<class T>
		class frame_observer: public observer<abstract_frame<T> >{

		public:
			frame_observer(abstract_component<T> *owner);
			~frame_observer() {}

		private:
			void on_subject_destruction();
			abstract_component<T> *m_owner;
		};


		/*! \class abstract_component
		*  \brief Describe a connected component that process data
		*/
		template<class T>
		class abstract_component{

			friend class abstract_frame<T>;

		public:
			abstract_component(const std::string& name, const unsigned int input_count,
					const unsigned int output_count);
			virtual ~abstract_component();

			const unsigned int get_input_count() const noexcept;
			const unsigned int get_output_count() const noexcept;

			const std::string& get_name() const noexcept;
			const std::string& get_input_name(const unsigned int input_id) const;
			const std::string& get_output_name(const unsigned int output_id) const;

			void connect_to(const unsigned int output_id, abstract_component<T>* dst,
					const unsigned int dst_input_id);
			void disconnect_input(const unsigned int input_id);

			abstract_frame<T> *get_frame() const;

			// To be implemented
			virtual T fetch_output(const unsigned int output_id) =0;
			virtual void process(const T input[]) =0;
			virtual void initialize_process() {};

		protected:
			abstract_component<T> *get_input_src(const unsigned int input_id,
					unsigned int& output_id);
			abstract_component<T> *get_input_src(const unsigned int input_id);

			void set_input_name(const std::string& name, const unsigned int input_id);
			void set_output_name(const std::string& name, const unsigned int output_id);

			void push_input();
			void push_input(const std::string& name);
			void pop_input();

			void push_output();
			void push_output(const std::string& name);
			void pop_output();

			virtual void on_input_connection(const unsigned int input_id) {};
		private:
			bool update_process_cyle(const unsigned int cycle) noexcept;

			const std::string default_input_name(const unsigned int input_id);
			const std::string default_output_name(const unsigned int output_id);

			subject<abstract_component<T> > m_component_subject;
			std::vector<component_link<T> > m_input;

			std::vector<std::string> m_input_name;
			std::vector<std::string> m_output_name;
			std::string m_name;

			frame_observer<T> m_frame;

			unsigned int m_process_cycle;
		};


		/*
		*		Component link implementation
		*/

		template<class T>
		component_link<T>::component_link()
			: observer<abstract_component<T> >(), m_src_output_id(0)
		{
		}

		template<class T>
		void component_link<T>::set_src_output_id(const unsigned int output_id)
		{
			m_src_output_id = output_id;
		}

		template<class T>
		unsigned int component_link<T>::get_src_output_id() const
		{
			return m_src_output_id;
		}

		template<class T>
		void component_link<T>::on_notify(const unsigned int popped_output_id)
		{
			if( popped_output_id == m_src_output_id )
				observer<abstract_component<T> >::disconnect();
		}
		/*
		* 		Frame link implementation
		*/

		template<class T>
		frame_observer<T>::frame_observer(abstract_component<T> *owner)
			: m_owner(owner)
		{
		}

		template<class T>
		void frame_observer<T>::on_subject_destruction()
		{
			const unsigned int ic = m_owner->get_input_count();

			for(unsigned int i = 0; i < ic; ++i)
				m_owner->disconnect_input(i);
		}

		/*
		* 		Abstract Component implementation
		*/

		template<class T>
		abstract_component<T>::abstract_component(const std::string& name,
				const unsigned int input_count, const unsigned int output_count)
				: m_component_subject(this),
				m_input(input_count),
				m_input_name(input_count),
				m_output_name(output_count),
				m_name(name),
				m_frame(this),
				m_process_cycle(0)
		{
			// Not with iterator because we need id for name creation
			for(unsigned int i = 0; i < input_count; ++i)
				m_input_name[i] = default_input_name(i);
			for(unsigned int i = 0; i < output_count; ++i)
				m_output_name[i] = default_output_name(i);

		}

		template<class T>
		abstract_component<T>::~abstract_component()
		{
			abstract_frame<T> *const frame = get_frame();

			if( frame != nullptr )
				frame->notify_circuit_change();
		}

		template<class T>
		const unsigned int abstract_component<T>::get_input_count() const noexcept
		{
			return static_cast<unsigned int>(m_input_name.size());
		}

		template<class T>
		const unsigned int abstract_component<T>::get_output_count() const noexcept
		{
			return m_output_name.size();
		}

		template<class T>
		const std::string& abstract_component<T>::get_name() const noexcept
		{
			return m_name;
		}

		template<class T>
		const std::string& abstract_component<T>::get_input_name(const unsigned int input_id) const
		{
			if(input_id >= get_input_count())
				throw std::out_of_range("Invalid input id");
			return m_input_name[input_id];
		}

		template<class T>
		const std::string& abstract_component<T>::get_output_name(const unsigned int output_id) const
		{
			if(output_id >= get_output_count())
				throw std::out_of_range("Invalid output id");
			return m_output_name[output_id];
		}

		template<class T>
		void abstract_component<T>::connect_to(const unsigned int output_id, abstract_component<T>* dst, const unsigned int dst_input_id)
		{
			abstract_frame<T> *const frame = get_frame();

			if (frame == nullptr || frame != dst->get_frame())
				throw std::domain_error("Component are not on the same frame");

			if (output_id >= get_output_count() )
				throw std::out_of_range("Invalid output id");
			if (dst_input_id >= dst->get_input_count() )
				throw std::out_of_range("Invalid input id");

			m_component_subject.register_observer(&(dst->m_input[dst_input_id]));
			dst->m_input[dst_input_id].set_src_output_id(output_id);
			dst->on_input_connection(dst_input_id);
			frame->notify_circuit_change();
		}

		template<class T>
		void abstract_component<T>::disconnect_input(const unsigned int input_id)
		{
			abstract_frame<T> *const frame = get_frame();

			if (input_id >= get_input_count() )
				throw std::out_of_range("Invalid input id");

			m_input[input_id].disconnect();

			if (frame != nullptr )
				frame->notify_circuit_change();
		}

		template<class T>
		abstract_component<T> *abstract_component<T>::get_input_src(const unsigned int input_id, unsigned int& output_id)
		{
			if (input_id >= get_input_count() )
				throw std::out_of_range("Invalid input id");

			output_id = m_input[input_id].get_src_output_id();
			return m_input[input_id].get_subject_resource();
		}

		template<class T>
		abstract_component<T> *abstract_component<T>::get_input_src(const unsigned int input_id)
		{
			if( input_id >= get_input_count() )
				throw std::out_of_range("Invalid input id");

			return m_input[input_id].get_subject_resource();
		}

		template<class T>
		abstract_frame<T> *abstract_component<T>::get_frame() const
		{
			return m_frame.get_subject_resource();
		}

		template<class T>
		bool abstract_component<T>::update_process_cyle(const unsigned int cycle) noexcept
		{
			if( m_process_cycle != cycle ){
				m_process_cycle = cycle;
				return true;
			}
			else{
				return false;
			}
		}


		// Protected

		template<class T>
		void abstract_component<T>::set_input_name(const std::string& name, const unsigned int input_id)
		{
			if( input_id >= get_input_count() )
				throw std::out_of_range("Invalid input id");
			m_input_name[input_id] = name;
		}

		template<class T>
		void abstract_component<T>::set_output_name(const std::string& name, const unsigned int output_id)
		{
			if( output_id >= get_output_count() )
				throw std::out_of_range("Invalid output id");
			m_output_name[output_id] = name;
		}

		template<class T>
		void abstract_component<T>::push_input()
		{
			const unsigned int new_input_id = get_input_count();
			m_input.push_back(component_link<T>());
			m_input_name.push_back(default_input_name(new_input_id));
		}

		template<class T>
		void abstract_component<T>::push_input(const std::string& name)
		{
			m_input.push_back(component_link<T>());
			m_input_name.push_back(name);
		}

		template<class T>
		void abstract_component<T>::pop_input()
		{
			if( get_input_count() == 0 )
				throw std::domain_error("No input to pop");
			m_input.pop_back();
			m_input_name.pop_back();
		}

		template<class T>
		void abstract_component<T>::push_output()
		{
			const unsigned int new_output_id = get_output_count();
			m_output_name.push_back(default_output_name(new_output_id));
		}

		template<class T>
		void abstract_component<T>::push_output(const std::string& name)
		{
			m_output_name.push_back(name);
		}

		template<class T>
		void abstract_component<T>::pop_output()
		{
			if( get_output_count() == 0 )
				throw std::domain_error("No output to pop");
			m_output_name.pop_back();
			m_component_subject.notify_observers(get_output_count());
		}

		// private

		template<class T>
		const std::string abstract_component<T>::default_input_name(const unsigned int input_id)
		{
			return "In-" + std::to_string(input_id);
		}

		template<class T>
		const std::string abstract_component<T>::default_output_name(const unsigned int output_id)
		{
			return "Out-" + std::to_string(output_id);
		}

	} /* Process */


} /* namespace Gammou */


#endif /* COMPONENT_H_ */
