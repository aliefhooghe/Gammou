#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <vector>
#include <string>

namespace Gammou{

  class component_link{

    friend class component;

  public:
    component_link();

  private:
    void add_to_link_list(component_link *link);

    component *m_src;
    unsigned int m_out_id;
    component_link *m_next_link;
    component_link *m_previous_link;
  };


  class component{

  public:
    component(const std::string& name, const unsigned int input_count,
      const unsigned int output_count);

    const unsigned int get_output_count() const;
    const unsigned int get_input_count() const;

    const std::string get_input_name(const unsigned int input_id);
    const std::string get_output_name(const unsigned int output_id);

    void connect_to(const unsigned int src_output_id, component* dst,
                    const unsigned int dst_input_id);

    component *get_input_src(const unsigned int input_id, unsigned int& output_id);

  protected:
    void pop_input();
    void push_input(const std::string& name);

    void pop_output();
    void push_output(const std::string& name);

    void set_input_name(const std::string& name, const unsigned int input_id);
    void set_output_name(const std::string& name, const unsigned int output_id);

  private:
    void connect_to_link(component_link *link);
    const std::string input_default_name(const unsigned int input_id);
    const std::string output_default_name(const unsigned int output_id);

    std::vector<input_link> m_input;

    std::vector<std::string> m_input_name;
    std::vector<std::string> m_output_name;
    std::string m_name;
  };


}

#endif
