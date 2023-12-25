#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vector {
    double data[n] = {0};
    double& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
};

template<int n> double operator*(const vector<n>& lhs, const vector<n>& rhs) {
    double ret = 0;
    for (int i=n; i--; ret+=lhs[i]*rhs[i]);
    return ret;
}

template<int n> vector<n> operator+(const vector<n>& lhs, const vector<n>& rhs) {
    vector<n> ret = lhs;
    for (int i=n; i--; ret[i]+=rhs[i]);
    return ret;
}

template<int n> vector<n> operator-(const vector<n>& lhs, const vector<n>& rhs) {
    vector<n> ret = lhs;
    for (int i=n; i--; ret[i]-=rhs[i]);
    return ret;
}

template<int n> vector<n> operator*(const double& rhs, const vector<n> &lhs) {
    vector<n> ret = lhs;
    for (int i=n; i--; ret[i]*=rhs);
    return ret;
}

template<int n> vector<n> operator*(const vector<n>& lhs, const double& rhs) {
    vector<n> ret = lhs;
    for (int i=n; i--; ret[i]*=rhs);
    return ret;
}

template<int n> vector<n> operator/(const vector<n>& lhs, const double& rhs) {
    vector<n> ret = lhs;
    for (int i=n; i--; ret[i]/=rhs);
    return ret;
}

template<int n1,int n2> vector<n1> embed(const vector<n2> &v, double fill=1) {
    vector<n1> ret;
    for (int i=n1; i--; ret[i]=(i<n2?v[i]:fill));
    return ret;
}

template<int n1,int n2> vector<n1> proj(const vector<n2> &v) {
    vector<n1> ret;
    for (int i=n1; i--; ret[i]=v[i]);
    return ret;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vector<n>& v) {
    for (int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

template<> struct vector<2> {
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    double  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
    vector<2> normalized() { return (*this)/norm(); }
};

template<> struct vector<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
    vector<3> normalized() { return (*this)/norm(); }
};

typedef vector<2> vector2;
typedef vector<3> vector3;
typedef vector<4> vector4;
vector3 cross(const vector3 &v1, const vector3 &v2);

template<int n> struct dt;

template<int nrows,int ncols> struct matrix {
    vector<ncols> rows[nrows] = {{}};

          vector<ncols>& operator[] (const int idx)       { assert(idx>=0 && idx<nrows); return rows[idx]; }
    const vector<ncols>& operator[] (const int idx) const { assert(idx>=0 && idx<nrows); return rows[idx]; }

    vector<nrows> col(const int idx) const {
        assert(idx>=0 && idx<ncols);
        vector<nrows> ret;
        for (int i=nrows; i--; ret[i]=rows[i][idx]);
        return ret;
    }

    void set_col(const int idx, const vector<nrows> &v) {
        assert(idx>=0 && idx<ncols);
        for (int i=nrows; i--; rows[i][idx]=v[i]);
    }

    static matrix<nrows,ncols> identity() {
        matrix<nrows,ncols> ret;
        for (int i=nrows; i--; )
            for (int j=ncols;j--; ret[i][j]=(i==j));
        return ret;
    }

    double det() const {
        return dt<ncols>::det(*this);
    }

    matrix<nrows-1,ncols-1> get_minor(const int row, const int col) const {
        matrix<nrows-1,ncols-1> ret;
        for (int i=nrows-1; i--; )
            for (int j=ncols-1;j--; ret[i][j]=rows[i<row?i:i+1][j<col?j:j+1]);
        return ret;
    }

    double cofactor(const int row, const int col) const {
        return get_minor(row,col).det()*((row+col)%2 ? -1 : 1);
    }

    matrix<nrows,ncols> adjugate() const {
        matrix<nrows,ncols> ret;
        for (int i=nrows; i--; )
            for (int j=ncols; j--; ret[i][j]=cofactor(i,j));
        return ret;
    }

    matrix<nrows,ncols> invert_transpose() const {
        matrix<nrows,ncols> ret = adjugate();
        return ret/(ret[0]*rows[0]);
    }

    matrix<nrows,ncols> invert() const {
        return invert_transpose().transpose();
    }

    matrix<ncols,nrows> transpose() const {
        matrix<ncols,nrows> ret;
        for (int i=ncols; i--; ret[i]=this->col(i));
        return ret;
    }
};

template<int nrows,int ncols> vector<nrows> operator*(const matrix<nrows,ncols>& lhs, const vector<ncols>& rhs) {
    vector<nrows> ret;
    for (int i=nrows; i--; ret[i]=lhs[i]*rhs);
    return ret;
}

template<int R1,int C1,int C2>matrix<R1,C2> operator*(const matrix<R1,C1>& lhs, const matrix<C1,C2>& rhs) {
    matrix<R1,C2> result;
    for (int i=R1; i--; )
        for (int j=C2; j--; result[i][j]=lhs[i]*rhs.col(j));
    return result;
}

template<int nrows,int ncols>matrix<nrows,ncols> operator*(const matrix<nrows,ncols>& lhs, const double& val) {
    matrix<nrows,ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]*val);
    return result;
}

template<int nrows,int ncols>matrix<nrows,ncols> operator/(const matrix<nrows,ncols>& lhs, const double& val) {
    matrix<nrows,ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]/val);
    return result;
}

template<int nrows,int ncols>matrix<nrows,ncols> operator+(const matrix<nrows,ncols>& lhs, const matrix<nrows,ncols>& rhs) {
    matrix<nrows,ncols> result;
    for (int i=nrows; i--; )
        for (int j=ncols; j--; result[i][j]=lhs[i][j]+rhs[i][j]);
    return result;
}

template<int nrows,int ncols>matrix<nrows,ncols> operator-(const matrix<nrows,ncols>& lhs, const matrix<nrows,ncols>& rhs) {
    matrix<nrows,ncols> result;
    for (int i=nrows; i--; )
        for (int j=ncols; j--; result[i][j]=lhs[i][j]-rhs[i][j]);
    return result;
}

template<int nrows,int ncols> std::ostream& operator<<(std::ostream& out, const matrix<nrows,ncols>& m) {
    for (int i=0; i<nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct dt {
    static double det(const matrix<n,n>& src) {
        double ret = 0;
        for (int i=n; i--; ret += src[0][i]*src.cofactor(0,i));
        return ret;
    }
};

template<> struct dt<1> {
    static double det(const matrix<1,1>& src) {
        return src[0][0];
    }
};
