#pragma once

#include "utility.h"
#include "common.h"
#include <vector>
#include <string>

BEGIN_GAIA

class Buffer;

struct VertexAttribute
{
    std::string name;
    uint32_t format;
    //uint32_t byteOffset;
};

struct SubVertexBuffer
{
    std::vector<VertexAttribute> vertexAttributes;
    uint32_t byteStride;
    uint32_t vertexCount;
};

struct SubVertexBufferSet
{
    enum {max_vertex_binding_count = 8};
    SubVertexBuffer subVertexBuffers[max_vertex_binding_count];
};

struct SubIndexBuffer
{ 
    IndexType indexType;
    uint32_t byteOffset;
    uint32_t indexCount;
};

struct SubMesh 
{
    uint32_t subVertexBufferSet;
    uint32_t subIndexBuffer;
    uint32_t indexOffset;
    uint32_t vertexOffset;
    uint32_t vertexOrIndexCount;
    PrimitiveTopology topology;
};

class Mesh
{
public:
    std::vector<SubVertexBufferSet> m_subVertexBufferSets;
    std::vector<SubIndexBuffer> m_subIndexBuffers;
    std::vector<SubMesh> m_subMeshes;
    RefPtr<Buffer> m_vertexBuffer;
    RefPtr<Buffer> m_indexBuffer;
};

END_GAIA