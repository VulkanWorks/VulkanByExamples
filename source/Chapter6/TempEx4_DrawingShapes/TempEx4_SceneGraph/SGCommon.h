#pragma once

#define M_PI (3.14)
#define M_PI_2 (3.14 * 2)

enum SHAPE
{
	SHAPE_NONE = -1,

	// BASIC SHAPES GOES HERE
	SHAPE_CUBE,
	SHAPE_RECTANGLE,

	SHAPE_COUNT, // TOTAL SHAPE COUNTS

	// CUSTOM SHAPES GOES HERE
	SHAPE_CUSTOM // NOTE: Important: the custom object are comprised of simpler objects hence not a part of model factories
};

enum RENDER_SCEHEME_TYPE
{
    RENDER_SCEHEME_NONE = -1,
    RENDER_SCEHEME_INSTANCED, // Single Command buffer, Single Draw call
    RENDER_SCEHEME_MULTIDRAW, // Single Command buffer, Multiple Draw call
    RENDER_SCEHEME_COUNT
};