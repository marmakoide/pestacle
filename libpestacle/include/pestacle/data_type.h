#ifndef PESTACLE_DATA_TYPE_H
#define PESTACLE_DATA_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif


enum DataType {
	DataType__invalid = 0, // Used as a debugging help
	DataType__matrix,
	DataType__rgb_surface
}; // enum DataType


typedef struct {
	size_t width;
	size_t height;
} DataTypeMatrixMetadata;


typedef struct {
	size_t width;
	size_t height;
} DataTypeRGBSurfaceMetadata;


typedef struct {
	enum DataType type;
	union {
		DataTypeMatrixMetadata matrix;
		DataTypeRGBSurfaceMetadata rgb_surface;
	};
} DataDescriptor;


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_DATA_TYPE_H */
