
#include "synthesizer_parameter.h"

using namespace ossia;

namespace Gammou {

    synthesizer_ossia_parameter::synthesizer_ossia_parameter(
        net::node_base& node,
        Sound::synthesizer *synthesizer,
        const unsigned int parameter_id)
    :   net::parameter_base(node),
        m_synthesizer(synthesizer),
        m_parameter_id(parameter_id),
        m_domain(init_domain(val_type::FLOAT))
    {
        set_repetition_filter(repetition_filter::ON);
        set_value(0.0);
    }

    void synthesizer_ossia_parameter::pull_value()
    {
        get_protocol().pull(*this);
    }

    ossia::value synthesizer_ossia_parameter::value() const
    {
        return m_synthesizer->get_parameter_value(m_parameter_id);
    }

    net::parameter_base& synthesizer_ossia_parameter::push_value(const ossia::value& val)  
    { 
        set_value(val);
        get_protocol().push(*this); 
        return *this; 
    }

    net::parameter_base& synthesizer_ossia_parameter::push_value(ossia::value&& val) 
    { 
        return push_value(val); 
    }

    net::parameter_base& synthesizer_ossia_parameter::push_value() 
    {     
        get_protocol().push(*this); 
        return *this; 
    }

    net::parameter_base& synthesizer_ossia_parameter::set_value(const ossia::value& val) 
    {
        if (val.valid()) {
            send(val);
            m_synthesizer->
                set_parameter_value(val.get<float>(), m_parameter_id);
        }
        
        return *this;
    }

    net::parameter_base& synthesizer_ossia_parameter::set_value(ossia::value&& val)
    {
        return set_value(val);
    }

    val_type synthesizer_ossia_parameter::get_value_type() const 
    {
        return val_type::FLOAT;
    }

    access_mode synthesizer_ossia_parameter::get_access() const 
    {   
        return access_mode::SET;
    }

    const domain& synthesizer_ossia_parameter::get_domain() const  
    {
        return m_domain;
    }

    bounding_mode synthesizer_ossia_parameter::get_bounding() const 
    {   
        return bounding_mode::FREE;
    }

} /* Gammou */
