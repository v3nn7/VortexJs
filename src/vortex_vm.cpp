#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

extern "C" {
    #undef likely
    #undef unlikely
    #define likely(x)   (x)
    #define unlikely(x) (x)
    #include "quickjs.h"
}

using namespace godot;

class VortexVM : public Node {
    GDCLASS(VortexVM, Node)

private:
    JSRuntime* rt = nullptr;
    JSContext* ctx = nullptr;

public:
    VortexVM() {
        rt = JS_NewRuntime();
        ctx = JS_NewContext(rt);
        UtilityFunctions::print("[VortexJS] QuickJS runtime initialized.");
    }

    ~VortexVM() {
        if (ctx) {
            JS_FreeContext(ctx);
            ctx = nullptr;
        }
        if (rt) {
            JS_FreeRuntime(rt);
            rt = nullptr;
        }
        UtilityFunctions::print("[VortexJS] QuickJS runtime freed.");
    }

    void _ready() {
        UtilityFunctions::print(run_js("1 + 2"));
    }

    String run_js(const String &code) {
        JSValue result = JS_Eval(
            ctx,
            code.utf8().get_data(),
            code.utf8().length(),
            "<input>",
            JS_EVAL_TYPE_GLOBAL
        );

        String output;
        if (JS_IsException(result)) {
            JSValue exception = JS_GetException(ctx);
            const char* error_msg = JS_ToCString(ctx, exception);
            output = String("[JS Error] ") + error_msg;
            JS_FreeCString(ctx, error_msg);
            JS_FreeValue(ctx, exception);
        } else {
            const char* str = JS_ToCString(ctx, result);
            output = str ? String(str) : "undefined";
            JS_FreeCString(ctx, str);
        }

        JS_FreeValue(ctx, result);
        return output;
    }

    String run_js_file(const String &path) {
        Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
        if (!file.is_valid()) {
            return "[VortexJS] Failed to open file: " + path;
        }

        String code = file->get_as_text();
        return run_js(code);
    }

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("run_js", "code"), &VortexVM::run_js);
        ClassDB::bind_method(D_METHOD("run_js_file", "path"), &VortexVM::run_js_file);
    }
};

void initialize_vortexjs_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
    ClassDB::register_class<VortexVM>();
}

void uninitialize_vortexjs_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
}

extern "C" GDExtensionBool GDE_EXPORT gdextension_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization* r_initialization
) {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_vortexjs_module);
    init_obj.register_terminator(uninitialize_vortexjs_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
