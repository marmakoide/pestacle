#include <assert.h>

#include <pestacle/macros.h>
#include <pestacle/memory.h>

#include "window/display.h"
#include "window/mouse_motion.h"

#include "window/scope.h"
#include "window_manager.h"


// --- Interface --------------------------------------------------------------

static bool
window_scope_setup(
	Scope* self
);


#define WIDTH_PARAMETER    0
#define HEIGHT_PARAMETER   1
#define TITLE_PARAMETER    2
#define BORDERED_PARAMETER 3

static const ParameterDefinition
window_scope_parameters[] = {
	{
		ParameterType__integer,
		"width",
		{ .int64_value = 320 }
	},
	{
		ParameterType__integer,
		"height",
		{ .int64_value = 240 }
	},
	{
		ParameterType__string,
		"title",
		{ .string_value = "pestacle" }
	},
	{
		ParameterType__bool,
		"bordered",
		{ .bool_value = true }
	},
	PARAMETER_DEFINITION_END
}; // window_scope_parameters


const ScopeDelegate
window_scope_delegate = {
	"window",
	window_scope_parameters,
	{
		window_scope_setup,
		0
	},
}; // window_scope_delegate


// --- Implementation ---------------------------------------------------------

static bool
window_scope_setup(
	Scope* self
) {
	// Retrieve the parameters
	size_t width = (size_t)self->parameters[WIDTH_PARAMETER].int64_value;
	size_t height = (size_t)self->parameters[HEIGHT_PARAMETER].int64_value;
	const char* title = self->parameters[TITLE_PARAMETER].string_value;
	bool bordered = self->parameters[BORDERED_PARAMETER].bool_value;

	// Retried the window manager
	WindowManager* window_manager =
		(WindowManager*)self->delegate_scope->data;

	// Create the window
	Window* window = 
		WindowManager_add_window(
			window_manager,
			title,
			width,
			height
		);

	if (!window)
		goto failure;

	Window_set_bordered(window, bordered);

	// Keep a pointer on the window object
	self->data = window;

	// Add the 'display' node
	Node* display_node = 
		Node_new(display_node_delegate.name, &display_node_delegate, self);

	if ((!display_node) || (!Scope_add_node(self, display_node)))
		goto failure;

	// Add the 'mouse-motion' delegate
	Scope_add_node_delegate(self, &mouse_motion_node_delegate);

	// Job done
	return true;

failure:
	if (window)
		WindowManager_remove_window(window_manager, window);

	return false;
}
