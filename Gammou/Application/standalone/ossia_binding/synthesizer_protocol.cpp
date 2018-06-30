
#include "synthesizer_protocol.h"
#include "synthesizer_parameter.h"

using namespace ossia;

namespace Gammou {

    synthesizer_ossia_protocol::synthesizer_ossia_protocol(
        Sound::synthesizer *synthesizer)
        : m_synthesizer(synthesizer)
    {
    }

    synthesizer_ossia_protocol::~synthesizer_ossia_protocol()
    {
    }

    void synthesizer_ossia_protocol::set_device(ossia::net::device_base& dev)
    {
        auto& root = dev.get_root_node();  
        const unsigned int parameter_count = 
            m_synthesizer->get_parameter_input_count();

        for (unsigned int i = 0; i < parameter_count; ++i) {
            auto& param_node = 
                net::create_node(root, "param-" + std::to_string(i));
            param_node.set_parameter(
                std::make_unique<synthesizer_ossia_parameter>(
                    param_node,
                    m_synthesizer,
                    i));
        }
    }

    bool synthesizer_ossia_protocol::pull(net::parameter_base& param)
    {
        return true;
    }

    bool synthesizer_ossia_protocol::push(const net::parameter_base& param)
    {
        return true;
    }

    bool synthesizer_ossia_protocol::observe(net::parameter_base& param, bool enable)
    {
        return false;
    }

    bool synthesizer_ossia_protocol::push_raw(const ossia::net::full_parameter_data& data)
    {
        return false;
    }

    bool synthesizer_ossia_protocol::update(ossia::net::node_base&)
    {
        return true;
    }



} /* Gammou */