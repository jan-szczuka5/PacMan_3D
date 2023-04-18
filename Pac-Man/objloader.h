#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <vector>
#include <glm/glm.hpp>

bool loadOBJ(const char*,
    std::vector < glm::vec3 >& out_vertices,
    std::vector < glm::vec2 >& out_uvs,
    std::vector < glm::vec3 >& out_normals,
    int licznik);

#endif /* LOADOBJ_H */