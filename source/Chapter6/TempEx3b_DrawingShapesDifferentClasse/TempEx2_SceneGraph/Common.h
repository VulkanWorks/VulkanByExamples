#pragma once

enum SCENE_GRAPH_STATES
{
    SG_STATE_NONE = -1,

    SG_STATE_SETUP,     // Initial setup
    SG_STATE_RESIZE,    // Resize of window
    SG_STATE_RENDER,    // Scene Rendering

    SG_STATE_CUSTOM
};

enum SHAPE
{
    SHAPE_NONE = -1,

    // BASIC SHAPES GOES HERE
    SHAPE_RECTANGLE_MULTIDRAW,
    SHAPE_RECTANGLE_INSTANCED,

    SHAPE_CIRCLE_MULTIDRAW,
    SHAPE_CIRCLE_INSTANCED,

    SHAPE_COUNT, // TOTAL SHAPE COUNTS

    // CUSTOM SHAPES GOES HERE
    SHAPE_CUSTOM // NOTE: Important: the custom object are comprised of simpler objects hence not a part of model factories
};

struct BoundingRegion
{
    BoundingRegion(float p_X, float p_Y, float p_Width, float p_Height, float p_ZOrder = 0) // For 2D Bounding Box
    {
        m_Position.x = p_X;         m_Position.y = p_Y;        m_Position.z = p_ZOrder;
        m_Dimension.x = p_Width;    m_Dimension.y = p_Height;  m_Dimension.z = p_ZOrder;
    }

    BoundingRegion(float p_X, float p_Y, float p_Z, float p_Width, float p_Height, float p_Depth)
    {
        m_Position.x = p_X;         m_Position.y = p_X;        m_Position.z = p_Z;
        m_Dimension.x = p_Width;    m_Dimension.y = p_Height;  m_Dimension.z = p_Depth;
    }

    glm::vec3 m_Position;
    glm::vec3 m_Dimension;
};
