#ifdef EMSCRIPTEN
#include "compiler/compiler.hpp"
#include "parser/parser.hpp"
#include "printer/bytecode_printer.hpp"
#include "source/string_script_source.hpp"

#include <cinttypes>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

struct CompilationResult
{
    size_t ErrorLine = 0;
    std::string ErrorMessage = "";
    std::vector<uint8_t> ByteCode;
};

CompilationResult compileScript(std::string script)
{
    CompilationResult result;

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    if (parser.hasError())
    {
        auto error = parser.getError();
        result.ErrorLine = std::get<0>(error);
        result.ErrorMessage = std::get<1>(error);
        return result;
    }

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    if (compiler.hasError())
    {
        auto error = compiler.getError();
        result.ErrorLine = std::get<0>(error);
        result.ErrorMessage = std::get<1>(error);
        return result;
    }

    SyncBlink::ByteCodePrinter bcp;
    result.ByteCode = bcp.print(program);

    return result;
}

EMSCRIPTEN_BINDINGS(blinkscript)
{
    class_<CompilationResult>("CompilationResult")
        .property("ErrorLine", &CompilationResult::ErrorLine)
        .property("ErrorMessage", &CompilationResult::ErrorMessage)
        .property("ByteCode", &CompilationResult::ByteCode);
    function("compileScript", &compileScript);
    register_vector<uint8_t>("byteCodeVec");
}

#endif // EMSCRIPTEN