#ifndef DIRECTORY_MODEL_H_
#define DIRECTORY_MODEL_H_

#include <map>
#include <memory>
#include <variant>

namespace Gammou {

	namespace View {

		//  TODO add Observers

		template<class Key, class Value>
		class directory_model {

			public:
				using directory =
					std::unique_ptr<
						directory_model<
							Key,
							Value> >;

				using item =
					std::variant<
						directory,
						Value>;

				using node =
					std::pair<const Key, item>;

				virtual ~directory_model() {}
				
				virtual unsigned int get_item_count() const = 0;
				virtual item* get_item(const Key& key) = 0;
				virtual node& get_node(const unsigned int index) = 0;
				
				//	todo move sementic + add_dir(key, directory)
				virtual directory_model<Key, Value>& add_directory(const Key& key) = 0;
				virtual void add_value(const Key& key, const Value& value) = 0;
				virtual bool contains(const Key& k) const = 0;

				bool is_directory(const item& item)
				{
					return std::holds_alternative<directory>(item);
				}

				bool is_directory(const node& node)
				{
					return is_directory(node.second);
				}
		};


		//--------------------------------------------------------------------

		template<class Key, class Value>
		class storage_directory_model : public directory_model<Key, Value> {

			public:
				using typename directory_model<Key, Value>::item;
				using typename directory_model<Key, Value>::node;
				using typename directory_model<Key, Value>::directory;

				storage_directory_model();
				storage_directory_model(storage_directory_model&) = delete;
				storage_directory_model(storage_directory_model &&other);
				~storage_directory_model();

				// abstract_directory_model ovrride
				virtual unsigned int get_item_count() const override;
				virtual item* get_item(const Key& key) override;
				virtual node& get_node(const unsigned int index) override;
				virtual bool contains(const Key& k) const override;

				virtual storage_directory_model<Key, Value>& add_directory(const Key& key) override;
				virtual void add_value(const Key& key, const Value& value) override;

			private:
				std::map<Key, item> m_child;
		};

		template<class Key, class Value>
		storage_directory_model<Key, Value>::storage_directory_model()
		{
		}

		template<class Key, class Value>
		storage_directory_model<Key, Value>::storage_directory_model(storage_directory_model && other)
		:   m_child(std::move(other.m_child))
		{
		}

		template<class Key, class Value>
		storage_directory_model<Key, Value>::~storage_directory_model()
		{
		}

		template<class Key, class Value>
		unsigned int storage_directory_model<Key, Value>::get_item_count() const
		{
			return m_child.size();
		}

		template<class Key, class Value>
		bool storage_directory_model<Key, Value>::contains(const Key& k) const
		{
			return (m_child.find(k) != m_child.end());
		}

		template<class Key, class Value>
		typename storage_directory_model<Key, Value>::item *
		storage_directory_model<Key, Value>::get_item(const Key& key)
		{
			auto it = m_child.find(key);

			if (it == m_child.end())
				return nullptr;
			else
				return &(it->second);
		}

		template<class Key, class Value>
		typename storage_directory_model<Key, Value>::node&
		storage_directory_model<Key, Value>::get_node(const unsigned int index)
		{
			auto it = m_child.begin();
			std::advance(it, index);
			return *it;
		}

		template<class Key, class Value>
		storage_directory_model<Key, Value>& storage_directory_model<Key, Value>::add_directory(const Key& key)
		{
			auto dir =
				std::make_unique<storage_directory_model<Key, Value> >();
			auto dir_ptr = dir.get();
			auto ret = m_child.emplace(key, std::move(dir));

			if (!ret.second)
				throw std::runtime_error("storage directory model : this directory already exist");

			return *dir_ptr;
		}

		template<class Key, class Value>
		void storage_directory_model<Key, Value>::add_value(const Key& key, const Value& value)
		{
			m_child.insert(std::make_pair(key, value));
		}

	} /* View */

}   /*  Gammou */

#endif
