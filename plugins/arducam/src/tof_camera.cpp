#include <pestacle/memory.h>
#include <pestacle/math/matrix.h>

#include <ArducamTOFCamera.hpp>

#include "tof_camera.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
	Node* self
);


static void
node_update(
	Node* self
);


static NodeOutput
node_output(
	const Node* self
);


static const NodeInputDefinition
node_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


static const ParameterDefinition
node_parameters[] = {
	PARAMETER_DEFINITION_END
};


const NodeDelegate
arducam_tof_camera_node_delegate = {
	"tof-camera",
	NodeType__matrix,
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

struct ToFCameraData {
	Arducam::ArducamTOFCamera tof;
	Matrix out;

	bool init() {
		// Initialize the camera
		if (tof.open(Arducam::Connection::CSI, 0)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"could not open ToF camera\n"
			);
			return false;
		}

		//  Start the camera
		if (tof.start(Arducam::FrameType::DEPTH_FRAME)) {
			SDL_LogError(
				SDL_LOG_CATEGORY_SYSTEM,
				"could not start ToF camera\n"
			);
			return false;
		}

		// Camera setup
		int max_range = 0;
		tof.setControl(Arducam::CameraCtrl::RANGE, 4000);
		tof.getControl(Arducam::CameraCtrl::RANGE, &max_range);

		// Retrieve the camera infos
		Arducam::CameraInfo tof_info = tof.getCameraInfo();
		SDL_Log(
			"ToF camera started => %dx%d, max-range = %d",
			tof_info.width,
			tof_info.height,
			max_range
		);

		// Allocate the output matrix
		Matrix_init(&out, tof_info.height, tof_info.width);

		// Initialise the output matrix
		Matrix_fill(&out, (real_t)0);

		// Job done
		return true;
	}

	~ToFCameraData() {
		tof.stop();
		Matrix_destroy(&out);
	}

	void update() {
		Arducam::ArducamFrameBuffer* frame = tof.requestFrame(200);
		if (frame == nullptr)
			return;

		float* depth_ptr = (float*)frame->getData(Arducam::FrameType::DEPTH_FRAME);

		float* src = depth_ptr;
		float* dst_row = out.data;
		for(size_t i = 0; i < out.row_count; ++i, dst_row += out.col_count) {
			float* dst = dst_row;
			for(size_t j = 0; j < out.col_count; ++j, src += 1, dst += 1)
				*dst = *src;
		}
		
		tof.releaseFrame(frame);
	}
};


static bool
node_setup(
	Node* self
) {
	// Allocate data
	ToFCameraData* data = new ToFCameraData();
	if (!data)
		return false;

	// Initialise data
	if (!data->init()) {
		delete data;
		return false;
	}

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	ToFCameraData* data = (ToFCameraData*)self->data;
	if (data)
		delete data;
}


static void
node_update(
	Node* self
) {
	ToFCameraData* data = (ToFCameraData*)self->data;
	data->update();
}


static NodeOutput
node_output(
	const Node* self
) {
	ToFCameraData* data = (ToFCameraData*)self->data;
	NodeOutput ret = { .matrix = &(data->out) };
	return ret;
}

