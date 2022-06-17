#ifndef GAMMOU_CONFIGURATION_PAGE_H_
#define GAMMOU_CONFIGURATION_PAGE_H_

#include <string>
#include <DSPJIT/compile_node_class.h>

namespace Gammou
{
    class abstract_configuration_page
    {
    public:
        virtual ~abstract_configuration_page() noexcept = default;
        virtual void compile() =0;
        virtual void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) =0;
        virtual void free_static_memory_chunk(const DSPJIT::compile_node_class& node) =0;
        virtual void display() =0;
        virtual void rename(const std::string& name) =0;
    };
}

#endif