
#include "ir_loader.h"
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Linker/Linker.h>

namespace Gammou {

    std::unique_ptr<llvm::Module> load_ir_modules(llvm::LLVMContext& ctx, const std::vector<std::filesystem::path>& paths)
    {
        llvm::SMDiagnostic error;
        std::unique_ptr<llvm::Module> module{};

        for (auto i = 0u; i < paths.size(); ++i) {
            auto loaded_module = llvm::parseIRFile(paths[i].generic_string(), error, ctx);

            if (i == 0u)
                module = std::move(loaded_module);
            else
                llvm::Linker::linkModules(*module, std::move(loaded_module));
        }

        return module;
    }

}