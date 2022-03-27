#ifndef GAMMOU_ADDITIONAL_BUILTIN_NODES_H_
#define GAMMOU_ADDITIONAL_BUILTIN_NODES_H_

#include <DSPJIT/compile_node_class.h>

namespace Gammou {

    // Logical not (out = 1 - in)
    class logical_not_node : public DSPJIT::compile_node_class {
    public:
        logical_not_node()
        :    DSPJIT::compile_node_class{1u, 1u}
        {}

        std::vector<llvm::Value*> emit_outputs(
            DSPJIT::graph_compiler& compiler,
            const std::vector<llvm::Value*>& inputs,
            llvm::Value*, llvm::Value*) const override;
    };

}

#endif /* GAMMOU_ADDITIONAL_BUILTIN_NODES_H_ */