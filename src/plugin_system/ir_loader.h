#ifndef GAMMOU_IR_LOADER_H
#define GAMMOU_IR_LOADER_H

#include <filesystem>
#include <llvm/IR/Module.h>

namespace Gammou {

    /**
     * \brief Load and link togethers the specified modules
     */
    std::unique_ptr<llvm::Module> load_ir_modules(llvm::LLVMContext& ctx, const std::vector<std::filesystem::path>& paths);

}

#endif