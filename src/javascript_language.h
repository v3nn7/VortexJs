#pragma once

#include <godot_cpp/classes/script_language_extension.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <quickjs.h>

using namespace godot;

class JavaScriptLanguage : public ScriptLanguageExtension {
    GDCLASS(JavaScriptLanguage, ScriptLanguageExtension);

protected:
    static void _bind_methods() {}

public:
    String _get_name() const override { return "JavaScript"; }
    String _get_type() const override { return "JavaScript"; }
    String _get_extension() const override { return "js"; }

    Error _execute_file(const String &path) override {
        JSRuntime *rt = JS_NewRuntime();
        JSContext *ctx = JS_NewContext(rt);

        String code = FileAccess::get_file_as_string(path);
        JSValue result = JS_Eval(ctx, code.utf8().get_data(), code.length(),
                                 path.utf8().get_data(), JS_EVAL_TYPE_GLOBAL);

        if (JS_IsException(result)) {
            JSValue exception = JS_GetException(ctx);
            const char *err = JS_ToCString(ctx, exception);
            UtilityFunctions::print(String("[JS Error] ") + err);
            JS_FreeCString(ctx, err);
            JS_FreeValue(ctx, exception);
        } else {
            const char *str = JS_ToCString(ctx, result);
            UtilityFunctions::print(String("[JS Result] ") + str);
            JS_FreeCString(ctx, str);
        }

        JS_FreeValue(ctx, result);
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
        return OK;
    }
};
