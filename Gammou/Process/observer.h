
#ifndef OBSERVER_H_
#define OBSERVER_H_

namespace Gammou {

	namespace Process{

		template<class ResourceType, class NotificationTagType = unsigned int> class subject;


		/*
				Observer 
		*/

		template<class ResourceType, class NotificationTagType = unsigned int>
		class observer{
			friend class subject<ResourceType, NotificationTagType>;

		public:
			observer();
			observer(const observer<ResourceType, NotificationTagType>& src) noexcept;
			observer(observer<ResourceType, NotificationTagType>&& old) noexcept;

			virtual ~observer();

			ResourceType *get_subject_resource() const;
			void disconnect();

		protected:
			virtual void on_subject_destruction() {}
			virtual void on_notify(const NotificationTagType notification_tag) {};

		private:
			void notify_subject_destruction();
			void notify(const NotificationTagType notification_tag);

			subject<ResourceType, NotificationTagType> *m_subject;
			observer<ResourceType, NotificationTagType> *m_previous_observer;
			observer<ResourceType, NotificationTagType> *m_next_observer;
		};

		/*
			Subject
		*/

		template<class ResourceType, class NotificationTagType>
		class subject{
			friend class observer<ResourceType, NotificationTagType>;

		public:
			subject(ResourceType * const source);
			virtual ~subject();

			void register_observer(observer<ResourceType, NotificationTagType> *link);
			void notify_observers(const NotificationTagType notification_tag);

		private:
			ResourceType * const m_resource;
			observer<ResourceType, NotificationTagType> *m_first_observer;
		};



		/*
			Observer Implementation
		*/

		template<class ResourceType, class NotificationTagType>
		observer<ResourceType, NotificationTagType>::observer() :
			m_subject(nullptr), 
			m_previous_observer(nullptr), 
			m_next_observer(nullptr)
		{
		}

		template<class ResourceType, class NotificationTagType>
		observer<ResourceType, NotificationTagType>::observer(const observer<ResourceType, NotificationTagType>& src) noexcept :
			m_subject(src.m_subject),
			m_previous_observer(nullptr),
			m_next_observer(nullptr)
		{			
			if (m_subject != nullptr)
				m_subject->register_observer(this);
		}

		template<class ResourceType, class NotificationTagType>
		observer<ResourceType, NotificationTagType>::observer(observer<ResourceType, NotificationTagType>&& old) noexcept :
			m_subject(old.m_subject),
			m_previous_observer(old.m_previous_observer),
			m_next_observer(old.m_next_observer)
		{
			old.m_subject = nullptr;
			old.m_previous_observer = nullptr;
			old.m_next_observer = nullptr;
		}

		template<class ResourceType, class NotificationTagType>
		observer<ResourceType, NotificationTagType>::~observer()
		{
			disconnect();
		}

		template<class ResourceType, class NotificationTagType>
		ResourceType *observer<ResourceType, NotificationTagType>::get_subject_resource() const
		{
			if( m_subject == nullptr )
				return nullptr;
			else
				return m_subject->m_resource;
		}

		template<class ResourceType, class NotificationTagType>
		void observer<ResourceType, NotificationTagType>::disconnect()
		{
			if( m_subject != nullptr ){

				if( m_previous_observer == nullptr ){
					m_subject->m_first_observer = m_next_observer;

					if( m_next_observer != nullptr )
						m_next_observer->m_previous_observer = nullptr;
				}
				else{
					m_previous_observer->m_next_observer = m_next_observer;

					if( m_next_observer != nullptr )
						m_next_observer->m_previous_observer = m_previous_observer;
				}

			}

			m_subject = nullptr;
			m_previous_observer = nullptr;
			m_next_observer = nullptr;
		}


		template<class ResourceType, class NotificationTagType>
		void observer<ResourceType, NotificationTagType>::notify_subject_destruction()
		{
			if( m_next_observer != nullptr )
				m_next_observer->notify_subject_destruction();
			disconnect();
			on_subject_destruction();
		}

		template<class ResourceType, class NotificationTagType>
		void observer<ResourceType, NotificationTagType>::notify(const NotificationTagType notification_tag)
		{
			if( m_next_observer != nullptr )
				m_next_observer->notify(notification_tag);
			on_notify(notification_tag);
		}

		/*
		*	Subject Implementation
		*/

		template<class ResourceType, class NotificationTagType>
		subject<ResourceType, NotificationTagType>::subject(ResourceType * const source) :
			m_resource(source), m_first_observer(nullptr)
		{
		}

		template<class ResourceType, class NotificationTagType>
		subject<ResourceType, NotificationTagType>::~subject()
		{
			//DEBUG_PRINT("SUBJECT DTOR\n");
			if( m_first_observer != nullptr )
				m_first_observer->notify_subject_destruction();
		}

		template<class ResourceType, class NotificationTagType>
		void subject<ResourceType, NotificationTagType>::register_observer(observer<ResourceType, NotificationTagType> *link)
		{
			link->disconnect();
			link->m_subject = this;

			if( m_first_observer != nullptr ){
				link->m_next_observer = m_first_observer;
				m_first_observer->m_previous_observer = link;
			}

			m_first_observer = link;
		}

		template<class ResourceType, class NotificationTagType>
		void subject<ResourceType, NotificationTagType>::notify_observers(const NotificationTagType notification_tag)
		{
			if( m_first_observer != nullptr )
				m_first_observer->notify(notification_tag);
		}


	} /* Process */

} /*Gammou */


#endif /* OBSERVER */
