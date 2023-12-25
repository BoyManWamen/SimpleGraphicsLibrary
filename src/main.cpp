#include <limits>
#include "model.h"
#include "our_gl.h"

constexpr int outputWidth  = 800; // output image size
constexpr int outputHeight = 800;
constexpr vector3 lightDirection{1,1,1}; // light source
constexpr vector3 cameraPosition{1,1,3}; // camera position
constexpr vector3 cameraDirection{0,0,0}; // camera direction
constexpr vector3 cameraUp{0,1,0}; // camera up vector

extern matrix<4,4> ModelView; // "OpenGL" state matrices
extern matrix<4,4> Projection;

struct CustomShader : IShader {
    const Model &customModel;
    vector3 uniformLightDirection;       // light direction in view coordinates
    matrix<2,3> varyingUV;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    matrix<3,3> varyingNormal; // normal per vertex to be interpolated by FS
    matrix<3,3> viewTriangle;    // triangle in view coordinates

    CustomShader(const Model &m) : customModel(m) {
        uniformLightDirection = proj<3>((ModelView*embed<4>(lightDirection, 0.))).normalized(); // transform the light vector to view coordinates
    }

    virtual void vertex(const int iface, const int nthvert, vector4& gl_Position) {
        varyingUV.set_col(nthvert, customModel.get_texture_coord(iface, nthvert));
        varyingNormal.set_col(nthvert, proj<3>((ModelView).invert_transpose()*embed<4>(customModel.get_normal(iface, nthvert), 0.)));
        gl_Position= ModelView*embed<4>(customModel.get_vertex(iface, nthvert));
        viewTriangle.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection*gl_Position;
    }

    virtual bool fragment(const vector3 bar, TGAColor &gl_FragColor) {
        vector3 bn = (varyingNormal*bar).normalized(); // per-vertex normal interpolation
        vector2 uv = varyingUV*bar; // tex coord interpolation

        // for the math refer to the tangent space normal mapping lecture
        // https://github.com/ssloy/tinyrenderer/wiki/Lesson-6bis-tangent-space-normal-mapping
        matrix<3,3> AI = matrix<3,3>{ {viewTriangle.col(1) - viewTriangle.col(0), viewTriangle.col(2) - viewTriangle.col(0), bn} }.invert();
        vector3 i = AI * vector3{varyingUV[0][1] - varyingUV[0][0], varyingUV[0][2] - varyingUV[0][0], 0};
        vector3 j = AI * vector3{varyingUV[1][1] - varyingUV[1][0], varyingUV[1][2] - varyingUV[1][0], 0};
        matrix<3,3> B = matrix<3,3>{ {i.normalized(), j.normalized(), bn} }.transpose();

        vector3 n = (B * customModel.get_normal(uv)).normalized(); // transform the normal from the texture to the tangent space
        double diff = std::max(0., n*uniformLightDirection); // diffuse light intensity
        vector3 r = (n*(n*uniformLightDirection)*2 - uniformLightDirection).normalized(); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        double spec = std::pow(std::max(-r.z, 0.), 5+sample2D(customModel.specular(), uv)[0]); // specular intensity, note that the camera lies on the z-axis (in view), therefore simple -r.z

        TGAColor c = sample2D(customModel.diffuse(), uv);
        for (int i : {0,1,2})
            gl_FragColor[i] = std::min<int>(10 + c[i]*(diff + spec), 255);
    }
};
