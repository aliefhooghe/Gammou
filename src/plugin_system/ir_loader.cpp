
#include "ir_loader.h"
#include "log.h"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker/Linker.h>
#include <iostream>

namespace Gammou {

    std::unique_ptr<llvm::Module> load_ir_modules(llvm::LLVMContext& ctx, const std::vector<std::filesystem::path>& paths)
    {
        llvm::SMDiagnostic error;
        std::unique_ptr<llvm::Module> module{};
        llvm::raw_os_ostream stream{std::cout};

        for (auto i = 0u; i < paths.size(); ++i) {
            auto loaded_module = llvm::parseIRFile(paths[i].generic_string(), error, ctx);

            if (llvm::verifyModule(*loaded_module, &stream))
                throw std::invalid_argument("IR module is broken");

            // link the modules togethers
            if (i == 0u)
                module = std::move(loaded_module);
            else
                llvm::Linker::linkModules(*module, std::move(loaded_module));
        }

        return module;
    }

}