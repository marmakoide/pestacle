#include <assert.h>
#include <pestacle/data_type.h>


static bool
DataDescriptor_matrix_equals(
	DataDescriptor* x,
	DataDescriptor* y
) {
	assert(x);
	assert(y);
	assert(x->type == DataType__matrix);
	assert(y->type == DataType__matrix);
	
	return
		(x->matrix.width == y->matrix.width) &&
		(x->matrix.height == y->matrix.height);
}

static bool
DataDescriptor_rgb_surface_equals(
	DataDescriptor* x,
	DataDescriptor* y
) {
	assert(x);
	assert(y);
	assert(x->type == DataType__rgb_surface);
	assert(y->type == DataType__rgb_surface);

	return
		(x->rgb_surface.width == y->rgb_surface.width) &&
		(x->rgb_surface.height == y->rgb_surface.height);
}


void
DataDescriptor_set_as_matrix(
	DataDescriptor* self,
	size_t width,
	size_t height
) {
	assert(self);

	self->type = DataType__matrix;
	self->matrix.width = width;
	self->matrix.height = height;
}


void
DataDescriptor_set_as_rgb_surface(
	DataDescriptor* self,
	size_t width,
	size_t height
) {
	assert(self);

	self->type = DataType__rgb_surface;
	self->rgb_surface.width = width;
	self->rgb_surface.height = height;
}


bool
DataDescriptor_equals(
	DataDescriptor* x,
	DataDescriptor* y
) {
	assert(x);
	assert(y);

	if (x->type != y->type)
		return false;

	switch(x->type) {
		case DataType__matrix:
			return DataDescriptor_matrix_equals(x, y);

		case DataType__rgb_surface:
			return DataDescriptor_rgb_surface_equals(x, y);

		case DataType__invalid:
			assert(0);
	}

	return true;
}