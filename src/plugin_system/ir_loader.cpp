
#include <DSPJIT/ir_helper.h>

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
            auto loaded_module = llvm::parseIRFile(paths[i].generic_string(), error, ctx);

            if (DSPJIT::check_module(*loaded_module, error_string))
                throw std::invalid_argument("[IR loader] IR module is broken: " + error_string);

            // link the modules togethers
            if (i == 0u)
                module = std::move(loaded_module);
            else
                llvm::Linker::linkModules(*module, std::move(loaded_module));
        }

        return module;
    }

}