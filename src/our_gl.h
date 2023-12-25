#include "tgaimage.h"
#include "geometry.h"

void viewport(const int x, const int y, const int w, const int h);
void projection(const double coeff=0);
void lookat(const vector3 eye, const vector3 center, const vector3 up);

struct IShader {
    static TGAColor sample2D(const TGAImage &img, vector2 &uvf) {
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const vector3 bar, TGAColor &color) = 0;
};

void triangle(const vector4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer);
