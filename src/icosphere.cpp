#include "icosphere.h"
#include <cassert>
#include <cmath>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

namespace superpunto {

//----------------------------------------------------------------------------
// Helper: create a unit icosahedron.
//
// Returns a pair of vectors: the first contains 12 vertices and the second
// contains 20 triangular faces (each face given as a glm::uvec3).
//----------------------------------------------------------------------------
std::pair<std::vector<Vertex>, std::vector<Face>> icosahedron() {
  std::vector<Vertex> verts;
  float phi = (1.f + std::sqrt(5.f)) / 2.f;
  float norm = std::sqrt(1.f + phi * phi);

  // Create six vertices (normalized so that the icosahedron is unit sized)
  verts.push_back(Vertex(0, 1, phi) / norm);
  verts.push_back(Vertex(0, -1, phi) / norm);
  verts.push_back(Vertex(1, phi, 0) / norm);
  verts.push_back(Vertex(-1, phi, 0) / norm);
  verts.push_back(Vertex(phi, 0, 1) / norm);
  verts.push_back(Vertex(-phi, 0, 1) / norm);

  // Append the negatives of the above to obtain 12 vertices.
  int n = verts.size();
  for (int i = 0; i < n; i++) {
    verts.push_back(-verts[i]);
  }

  // Define the 20 faces (using glm::uvec3 for unsigned indices)
  std::vector<Face> faces = {
      Face(0, 5, 1),  Face(0, 3, 5),  Face(0, 2, 3),  Face(0, 4, 2),
      Face(0, 1, 4),  Face(1, 5, 8),  Face(5, 3, 10), Face(3, 2, 7),
      Face(2, 4, 11), Face(4, 1, 9),  Face(7, 11, 6), Face(11, 9, 6),
      Face(9, 8, 6),  Face(8, 10, 6), Face(10, 7, 6), Face(2, 11, 7),
      Face(4, 9, 11), Face(1, 8, 9),  Face(5, 10, 8), Face(3, 7, 10)};

  return {verts, faces};
}

//----------------------------------------------------------------------------
// Helper: create a unit cube.
//
// Returns a pair of vectors: the first contains 8 vertices and the second
// contains 12 triangular faces (each face given as a glm::uvec3).
//----------------------------------------------------------------------------
std::pair<std::vector<Vertex>, std::vector<Face>> cube() {
  // Define 8 vertices for a unit cube.
  std::vector<Vertex> verts = {{1, 1, 1},    {-1, 1, 1}, {-1, -1, 1},
                               {1, -1, 1},   {1, 1, -1}, {-1, 1, -1},
                               {-1, -1, -1}, {1, -1, -1}};
  // Normalizethe vertices to make them unit length.
  for (auto &v : verts) {
    v = glm::normalize(v);
  }
  std::vector<Face> faces = {Face(0, 1, 2), Face(0, 2, 3), Face(4, 7, 6),
                             Face(4, 6, 5), Face(0, 4, 5), Face(0, 5, 1),
                             Face(3, 2, 6), Face(3, 6, 7), Face(0, 3, 7),
                             Face(0, 7, 4), Face(1, 5, 6), Face(1, 6, 2)};
  return {verts, faces};
}

//----------------------------------------------------------------------------
// generate_icosphere()
//----------------------------------------------------------------------------
// Builds a unit icosahedron and subdivides each face using barycentric
// interpolation with subdivision frequency 'nu'. For each base face, we
// create a local grid of (nu+1)(nu+2)/2 vertices; each vertex is computed
// as a normalized linear combination of the three triangle vertices.
//----------------------------------------------------------------------------
std::tuple<std::vector<Vertex>, std::vector<Face>> generate_icosphere(int nu) {
  // Zero level is a cube
  if (nu == 0) {
    auto [baseVerts, baseFaces] = cube();
    return {baseVerts, baseFaces};
  }
  // First obtain base icosahedron.
  auto [baseVerts, baseFaces] = icosahedron();
  if (nu == 1) {
    return {baseVerts, baseFaces};
  }
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  // For each face of the icosahedron
  for (const auto &f : baseFaces) {
    // Get vertices A, B, C of the current face.
    Vertex A = baseVerts[f.x];
    Vertex B = baseVerts[f.y];
    Vertex C = baseVerts[f.z];

    // Create a local grid on the face.
    // grid[i][j] holds the global index of the vertex computed at that grid
    // point. The barycentric weights are:
    //    wA = (nu - i - j) / nu,  wB = i / nu,  wC = j / nu.
    std::vector<std::vector<int>> grid(nu + 1);
    for (int i = 0; i <= nu; ++i) {
      grid[i].resize(nu - i + 1);
    }
    for (int i = 0; i <= nu; ++i) {
      for (int j = 0; j <= nu - i; ++j) {
        int k = nu - i - j;
        float wA = static_cast<float>(k) / nu;
        float wB = static_cast<float>(i) / nu;
        float wC = static_cast<float>(j) / nu;
        Vertex pt = glm::normalize(A * wA + B * wB + C * wC);
        int idx = vertices.size();
        vertices.push_back(pt);
        grid[i][j] = idx;
      }
    }
    // Now create faces from the grid. Each little “cell” yields up to two
    // triangles.
    for (int i = 0; i < nu; ++i) {
      for (int j = 0; j < nu - i; ++j) {
        int idx0 = grid[i][j];
        int idx1 = grid[i + 1][j];
        int idx2 = grid[i][j + 1];
        faces.push_back(Face(idx0, idx1, idx2));
        if (j < nu - i - 1) { // there is a second triangle in the cell
          int idx3 = grid[i + 1][j + 1];
          faces.push_back(Face(idx1, idx3, idx2));
        }
      }
    }
  }
  return {vertices, faces};
}

} // namespace superpunto
