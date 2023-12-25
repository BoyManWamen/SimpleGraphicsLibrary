#include <iostream>
#include <sstream>
#include "model.h"

Model::Model(const std::string filename) {
    std::ifstream input;
    input.open(filename, std::ifstream::in);
    if (input.fail()) return;
    std::string line;
    while (!input.eof()) {
        std::getline(input, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vector3 vertex;
            for (int i=0;i<3;i++) iss >> vertex[i];
            vertices.push_back(vertex);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vector3 normal;
            for (int i=0;i<3;i++) iss >> normal[i];
            normals.push_back(normal.normalized());
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            vector2 uv;
            for (int i=0;i<2;i++) iss >> uv[i];
            texture_coords.push_back({uv.x, 1-uv.y});
        }  else if (!line.compare(0, 2, "f ")) {
            int f,t,n;
            iss >> trash;
            int count = 0;
            while (iss >> f >> trash >> t >> trash >> n) {
                face_vertices.push_back(--f);
                face_texture_coords.push_back(--t);
                face_normals.push_back(--n);
                count++;
            }
            if (3!=count) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }
    std::cerr << "# v# " << num_vertices() << " f# "  << num_faces() << " vt# " << texture_coords.size() << " vn# " << normals.size() << std::endl;
    load_texture(filename, "_diffuse.tga",    diffusemap );
    load_texture(filename, "_nm_tangent.tga", normalmap  );
    load_texture(filename, "_spec.tga",       specularmap);
}

int Model::num_vertices() const {
    return vertices.size();
}

int Model::num_faces() const {
    return face_vertices.size()/3;
}

vector3 Model::get_vertex(const int i) const {
    return vertices[i];
}

vector3 Model::get_vertex(const int iface, const int nthvert) const {
    return vertices[face_vertices[iface*3+nthvert]];
}

void Model::load_texture(std::string filename, const std::string suffix, TGAImage &img) {
    size_t dot = filename.find_last_of(".");
    if (dot==std::string::npos) return;
    std::string texfile = filename.substr(0,dot) + suffix;
    std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
}

vector3 Model::get_normal(const vector2 &uvf) const {
    TGAColor c = normalmap.get(uvf[0]*normalmap.width(), uvf[1]*normalmap.height());
    return vector3{(double)c[2],(double)c[1],(double)c[0]}*2./255. - vector3{1,1,1};
}

vector2 Model::get_texture_coord(const int iface, const int nthvert) const {
    return texture_coords[face_texture_coords[iface*3+nthvert]];
}

vector3 Model::get_normal(const int iface, const int nthvert) const {
    return normals[face_normals[iface*3+nthvert]];
}
