

#include "component.h"

namespace Gammou{


  component_link::component_link()
  : m_src(NULL), m_out_id(0),
  m_next_link(NULL), m_previous_link(NULL)
  {

  }

  ///

  component::component(const std::string& name, const unsigned int input_count,
                  const unsigned int output_count)
  : m_name(name), m_input(input_count), m_input_name(input_count),
    m_output_name(m_output_count)
  {
    for(unsigned int i = 0; i < input_count; ++i)
      m_input_name[i] = input_default_name(i);

    for(unsigned int i = 0; i < output_count; ++i)
      m_output_name[i] = output_default_name(i);
  }



}
