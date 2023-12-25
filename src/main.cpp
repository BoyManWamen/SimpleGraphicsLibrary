#include <limits>
#include "model.h"
#include "our_gl.h"

constexpr int outputWidth  = 800;
constexpr int outputHeight = 800;
constexpr vector3 lightDirection{1,1,1};
constexpr vector3 cameraPosition{1,1,3};
constexpr vector3 cameraDirection{0,0,0};
constexpr vector3 cameraUp{0,1,0};

extern matrix<4,4> ModelView;
extern matrix<4,4> Projection;

struct CustomShader : IShader {
    const Model &customModel;
    vector3 uniformLightDirection;
    matrix<2,3> varyingUV;
    matrix<3,3> varyingNormal;
    matrix<3,3> viewTriangle;

    CustomShader(const Model &m) : customModel(m) {
        uniformLightDirection = proj<3>((ModelView*embed<4>(lightDirection, 0.))).normalized();
    }

    virtual void vertex(const int iface, const int nthvert, vector4& gl_Position) {
        varyingUV.set_col(nthvert, customModel.get_texture_coord(iface, nthvert));
        varyingNormal.set_col(nthvert, proj<3>((ModelView).invert_transpose()*embed<4>(customModel.get_normal(iface, nthvert), 0.)));
        gl_Position= ModelView*embed<4>(customModel.get_vertex(iface, nthvert));
        viewTriangle.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection*gl_Position;
    }

    virtual bool fragment(const vector3 bar, TGAColor &gl_FragColor) {
        vector3 bn = (varyingNormal*bar).normalized();
        vector2 uv = varyingUV*bar;

        matrix<3,3> AI = matrix<3,3>{ {viewTriangle.col(1) - viewTriangle.col(0), viewTriangle.col(2) - viewTriangle.col(0), bn} }.invert();
        vector3 i = AI * vector3{varyingUV[0][1] - varyingUV[0][0], varyingUV[0][2] - varyingUV[0][0], 0};
        vector3 j = AI * vector3{varyingUV[1][1] - varyingUV[1][0], varyingUV[1][2] - varyingUV[1][0], 0};
        matrix<3,3> B = matrix<3,3>{ {i.normalized(), j.normalized(), bn} }.transpose();

        vector3 n = (B * customModel.get_normal(uv)).normalized();
        double diff = std::max(0., n*uniformLightDirection);
        vector3 r = (n*(n*uniformLightDirection)*2 - uniformLightDirection).normalized();
        double spec = std::pow(std::max(-r.z, 0.), 5+sample2D(customModel.specular(), uv)[0]);

        TGAColor c = sample2D(customModel.diffuse(), uv);
        for (int i : {0,1,2})
            gl_FragColor[i] = std::min<int>(10 + c[i]*(diff + spec), 255);
    }
};
