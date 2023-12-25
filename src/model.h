#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
    std::vector<vector3> vertices{};
    std::vector<vector2> texture_coords{};
    std::vector<vector3> normals{};
    std::vector<int> face_vertices{};
    std::vector<int> face_texture_coords{};
    std::vector<int> face_normals{};
    TGAImage diffusemap{};         // diffuse color texture
    TGAImage normalmap{};          // normal map texture
    TGAImage specularmap{};        // specular map texture
    void load_texture(const std::string filename, const std::string suffix, TGAImage &img);
public:
    Model(const std::string filename);
    int num_vertices() const;
    int num_faces() const;
    vector3 get_normal(const int iface, const int nthvert) const; // per triangle corner normal vertex
    vector3 get_normal(const vector2 &uv) const;                     // fetch the normal vector from the normal map texture
    vector3 get_vertex(const int i) const;
    vector3 get_vertex(const int iface, const int nthvert) const;
    vector2 get_texture_coord(const int iface, const int nthvert) const;
    const TGAImage& diffuse()  const { return diffusemap;  }
    const TGAImage& specular() const { return specularmap; }
};
