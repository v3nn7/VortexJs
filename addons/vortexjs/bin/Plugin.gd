@tool
extends EditorPlugin

func _enter_tree():
    # Called when the plugin is enabled in the editor
    print("VortexJS plugin enabled")

func _exit_tree():
    # Called when the plugin is disabled in the editor
    print("VortexJS plugin disabled")
