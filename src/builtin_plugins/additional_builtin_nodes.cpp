

#include "additional_builtin_nodes.h"
#include <DSPJIT/graph_compiler.h>

namespace Gammou {

    std::vector<llvm::Value*> logical_not_node::emit_outputs(DSPJIT::graph_compiler& compiler, const std::vector<llvm::Value*>& inputs, llvm::Value*, llvm::Value*) const
    {
        auto& ir_builder = compiler.builder();

        // out = 1.f - in

        return {ir_builder.CreateFSub(
            llvm::ConstantFP::get(ir_builder.getContext(), llvm::APFloat(1.f)),
            inputs[0])};
    }


} /* Gammou */
