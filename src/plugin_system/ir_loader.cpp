
#include <DSPJIT/ir_helper.h>
#include <DSPJIT/log.h>

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker/Linker.h>

#include "ir_loader.h"

namespace Gammou {

    std::unique_ptr<llvm::Module> load_ir_modules(llvm::LLVMContext& ctx, const std::vector<std::filesystem::path>& paths)
    {
        llvm::SMDiagnostic error;
        std::unique_ptr<llvm::Module> module{};
        std::string error_string;

        for (auto i = 0u; i < paths.size(); ++i) {
            const auto path = paths[i].generic_string();
            auto loaded_module = llvm::parseIRFile(path, error, ctx);

            if (!loaded_module)
            {
                LOG_ERROR("[IR loader] Failed to load module %s\n", path.c_str());
                LOG_ERROR("[IR loader] Error: %s, at line %d\n",
                    error.getMessage().str().c_str(), error.getLineNo());
                throw std::invalid_argument("[IR loader] Load failure");
            }
            if (DSPJIT::check_module(*loaded_module, error_string))
            {
                LOG_ERROR("[IR loader] Modume verification error for %s: %s\n",
                    path.c_str(), error_string.c_str());
                throw std::invalid_argument("[IR loader] IR module is broken");
            }

            // link the modules togethers
            if (i == 0u)
                module = std::move(loaded_module);
            else
                llvm::Linker::linkModules(*module, std::move(loaded_module));
        }

        return module;
    }

}