#pragma once

/* NOTE:
 *
 *  Include the <vk/dx/gl/mt/wg>-Renderer files before
 *  the BaseRenderer like :
 *
 *    #include "vk_renderer.hpp"
 *    #include "../btm_renderer.hpp"
 *
 */

#include "btm_base.hpp"
#include "btm_window.hpp"

namespace btm
{

//===========================
//= TYPEDEFS
//===========================

using UUID = std::string;

//===========================
//= CONTANTS
//===========================

glm::vec3 constexpr RIGHT = { 1, 0, 0 };
glm::vec3 constexpr UP    = { 0, 1, 0 };
glm::vec3 constexpr FRONT = { 0, 0, 1 };

//===========================
//= ENUMS
//===========================

enum struct Cull
{
    NONE,
    CW,   // Front
    CCW,  // Back
};

enum struct Compare
{
    NONE,
    LESS,
    LESS_EQ,
    GREAT,
    GREAT_EQ
};
using Depth = Compare;

enum struct Samples
{
    _1,
    _2,
    _4,
    _8,
    _16,
    _32,
    _64,
};

//===========================
//= HELPER STRUCTS
//===========================

struct Area2D
{
    glm::vec2 off = {};  // Rect's init-point
    glm::vec2 ext = {};  // Rect's end-point
};

struct Area3D
{
    glm::vec3 off = {};  // Cube's init-point
    glm::vec3 ext = {};  // Cube's end-point
};

//===========================
//= TYPES
//===========================

struct Mesh
{
    std::string name = "";

    struct Vertex
    {
        glm::vec3 pos     = {};  // 3:xyz
        glm::vec2 uv0     = {};  // 2:xy
        glm::vec3 normal  = {};  // 3:xyz
        glm::vec4 tangent = {};  // 4 : xyzw - XYZ:normalized, W:-1|+1 (handeness)
    };

    struct Instance
    {
        glm::mat4 transform;
        glm::vec4 color;
    };

    std::vector<u16>      indices;
    std::vector<Vertex>   vertices;
    std::vector<Instance> instances;
};
using Vertices  = std::vector<Mesh::Vertex>;
using Instances = std::vector<Mesh::Instance>;
using MeshGroup = std::vector<Mesh>;

//===========================
//= BASE RENDERER
//===========================

class BaseRenderer
{
public:
    inline static constexpr i32 sInFlight = 3;

    BaseRenderer(Ref<btm::Window> window);
    virtual ~BaseRenderer() = default;

    virtual void update() { BTM_ASSERT_X(0, "Cast-back to non-base renderer!"); }
    virtual void draw() { BTM_ASSERT_X(0, "Cast-back to non-base renderer!"); }
    virtual void cleanup() { BTM_ASSERT_X(0, "Cast-back to non-base renderer!"); }

    inline bool isInitialized() { return mInit; }

protected:
    inline void markAsInit() { mInit = true; }

    i32 mFrameNumber = 0;

    bool      mInit         = false;
    void     *mWindowHandle = nullptr;
    glm::vec2 mViewportSize = { 1280, 720 };
};

//===========================
//= TOOLS
//===========================

std::vector<Mesh> parseGltf(std::string const &filepath);
std::vector<Mesh> parseGltf(std::span<u8 const> bin);

}  // namespace btm

//
//
//
//
//
//
//
//
//
//

//=====================================
// PRINT HELPERS
//=====================================

// Mesh
template<>
struct fmt::formatter<btm::Mesh>
{
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const btm::Mesh &mesh, FormatContext &ctx) const -> decltype(ctx.out())
    {
        std::string meshStr = "INDICES:\n";
        for (auto const &i : mesh.indices)
            meshStr += BTM_FMT("{}, ", i);

        if (!mesh.indices.empty())
            meshStr.erase(meshStr.end() - 2, meshStr.end());

        meshStr += "\nATTRIBUTES (pos / uv0 / normal / tangent) :\n";

        size_t vn = 0;
        for (auto const &v : mesh.vertices)
            meshStr += BTM_FMT("{}: {} / {} / {} / {}", vn++, v.pos, v.uv0, v.normal, v.tangent) + "\n";

        return fmt::format_to(ctx.out(), "\n{}\n......\n{}", mesh.name, meshStr);
    }
};

// MeshGroup
template<>
struct fmt::formatter<btm::MeshGroup>
{
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const btm::MeshGroup &meshGroup, FormatContext &ctx) const -> decltype(ctx.out())
    {
        std::string meshGroupStr = "";
        for (auto const &mesh : meshGroup)
            meshGroupStr += BTM_FMT("{}", mesh) + "\n";

        return fmt::format_to(ctx.out(), "\nMESHGROUP\n--------\n{}", meshGroupStr);
    }
};
