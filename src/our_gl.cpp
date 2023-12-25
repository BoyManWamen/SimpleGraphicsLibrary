#include "our_gl.h"

matrix<4,4> ModelView;
matrix<4,4> Viewport;
matrix<4,4> Projection;

void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void projection(const double f) {
    Projection = {{{1,0,0,0}, {0,-1,0,0}, {0,0,1,0}, {0,0,-1/f,0}}};
}

void lookat(const vector3 eye, const vector3 center, const vector3 up) {
    vector3 z = (center-eye).normalized();
    vector3 x =  (cross(up,z)).normalized();
    vector3 y =  cross(z, x).normalized();
    matrix<4,4> Minv = {{{x.x,x.y,x.z,0},   {y.x,y.y,y.z,0},   {z.x,z.y,z.z,0},   {0,0,0,1}}};
    matrix<4,4> Tr   = {{{1,0,0,-eye.x}, {0,1,0,-eye.y}, {0,0,1,-eye.z}, {0,0,0,1}}};
    ModelView = Minv*Tr;
}

vector3 barycentric(const vector2 tri[3], const vector2 P) {
    matrix<3,3> ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    if (ABC.det()<1e-3) return {-1,1,1};
    return ABC.invert_transpose() * embed<3>(P);
}

void triangle(const vector4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer) {
    vector4 pts[3]  = { Viewport*clip_verts[0],    Viewport*clip_verts[1],    Viewport*clip_verts[2]    };
    vector2 pts2[3] = { proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]) };

    int bboxmin[2] = {image.width()-1, image.height()-1};
    int bboxmax[2] = {0, 0};
    for (int i=0; i<3; i++)
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(pts2[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(pts2[i][j]));
        }
#pragma omp parallel for
    for (int x=std::max(bboxmin[0], 0); x<=std::min(bboxmax[0], image.width()-1); x++) {
        for (int y=std::max(bboxmin[1], 0); y<=std::min(bboxmax[1], image.height()-1); y++) {
            vector3 bc_screen = barycentric(pts2, {static_cast<double>(x), static_cast<double>(y)});
            vector3 bc_clip   = {bc_screen.x/pts[0][3], bc_screen.y/pts[1][3], bc_screen.z/pts[2][3]};
            bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z);
            double frag_depth = vector3{clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]}*bc_clip;
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || frag_depth > zbuffer[x+y*image.width()]) continue;
            TGAColor color;
            if (shader.fragment(bc_clip, color)) continue;
            zbuffer[x+y*image.width()] = frag_depth;
            image.set(x, y, color);
        }
    }
}
