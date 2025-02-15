#ifndef ZSPACE_MATH_TYPES_H
#define ZSPACE_MATH_TYPES_H  1

#define fround 
#include <cmath>
#include "engine_types.h"

//round

//float LS[3][7];
static double const Rad = 0.01745329252; // 3.141592654 / 180;    //multiply degrees by Rad to get angle in radians
volatile static float LocalSIN[360];
volatile static float LocalCOS[360];

inline void InitMath()
{
    // Calculate Sin/Cos Tables
    for (int tmp = 0; tmp < 360; tmp++)
    {
        LocalSIN[tmp] = float(sin(tmp * Rad));
        LocalCOS[tmp] = float(cos(tmp * Rad));
    }
}




inline void FixAngle(float& angle)
{
    if (angle > 359) angle = angle - 360;
    if (angle < 0) angle = angle + 360;
}

inline float MyCos(float& fAng)
{
    if (fAng > 359) fAng = fAng - 360;
    if (fAng < 0) fAng = fAng + 360;
    //return LocalCOS[(int)fAng];
    return float(cos(fAng * Rad));

}

inline float MySin(float& fAng)
{
    if (fAng > 359) fAng = fAng - 360;
    if (fAng < 0) fAng = fAng + 360;
    //return LocalSIN[(int)fAng];
    return float(sin(fAng * Rad));
}

inline void matrix_multiply(float mat1[4][4], float mat2[4][4])
{

    float temp[4][4] = { 0 };

    for (unsigned char i = 0; i < 4; i++)
        for (unsigned char j = 0; j < 4; j++)
            for (unsigned char k = 0; k < 4; k++)
                temp[i][j] += (mat1[i][k] * mat2[k][j]);


    for (unsigned char i = 0; i < 4; i++)
    {
        mat1[i][0] = temp[i][0];
        mat1[i][1] = temp[i][1];
        mat1[i][2] = temp[i][2];
        mat1[i][3] = temp[i][3];
    }
}

inline void matrix_rotate(float gtl_matrix[4][4], Vec3& Ang)
{
    /**/
    float rmat[4][4];

   

    // Z
    rmat[0][0] = MyCos(Ang.z);	rmat[0][1] = MySin(Ang.z);	rmat[0][2] = 0;		rmat[0][3] = 0;
    rmat[1][0] = -MySin(Ang.z);	rmat[1][1] = MyCos(Ang.z);	rmat[1][2] = 0;		rmat[1][3] = 0;
    rmat[2][0] = 0;				rmat[2][1] = 0;				rmat[2][2] = 1;		rmat[2][3] = 0;
    rmat[3][0] = 0;				rmat[3][1] = 0;				rmat[3][2] = 0;		rmat[3][3] = 1;
    matrix_multiply(gtl_matrix, rmat);

    // Y
    rmat[0][0] = MyCos(Ang.y);	rmat[0][1] = 0;		rmat[0][2] = -MySin(Ang.y);	rmat[0][3] = 0;
    rmat[1][0] = 0;				rmat[1][1] = 1;		rmat[1][2] = 0;				rmat[1][3] = 0;
    rmat[2][0] = MySin(Ang.y);	rmat[2][1] = 0;		rmat[2][2] = MyCos(Ang.y);	rmat[2][3] = 0;
    rmat[3][0] = 0;				rmat[3][1] = 0;		rmat[3][2] = 0;				rmat[3][3] = 1;
    matrix_multiply(gtl_matrix, rmat);

    // X
    rmat[0][0] = 1;		rmat[0][1] = 0;				rmat[0][2] = 0;				rmat[0][3] = 0;
    rmat[1][0] = 0;		rmat[1][1] = MyCos(Ang.x);	rmat[1][2] = MySin(Ang.x);	rmat[1][3] = 0;
    rmat[2][0] = 0;		rmat[2][1] = -MySin(Ang.x);	rmat[2][2] = MyCos(Ang.x);	rmat[2][3] = 0;
    rmat[3][0] = 0;		rmat[3][1] = 0;				rmat[3][2] = 0;				rmat[3][3] = 1;
    matrix_multiply(gtl_matrix, rmat);
}

inline void matrix_ident(float Matrix[4][4])
{
    Matrix[0][0] = 1; Matrix[0][1] = 0; Matrix[0][2] = 0; Matrix[0][3] = 0;
    Matrix[1][0] = 0; Matrix[1][1] = 1; Matrix[1][2] = 0; Matrix[1][3] = 0;
    Matrix[2][0] = 0; Matrix[2][1] = 0; Matrix[2][2] = 1; Matrix[2][3] = 0;
    Matrix[3][0] = 0; Matrix[3][1] = 0; Matrix[3][2] = 0; Matrix[3][3] = 1;
}


inline void matrix_translate(float gtl_matrix[4][4], Vec3& p)
{
    float tmat[4][4];

    tmat[0][0] = 1; tmat[0][1] = 0; tmat[0][2] = 0; tmat[0][3] = 0;
    tmat[1][0] = 0; tmat[1][1] = 1; tmat[1][2] = 0; tmat[1][3] = 0;
    tmat[2][0] = 0; tmat[2][1] = 0; tmat[2][2] = 1; tmat[2][3] = 0;
    tmat[3][0] = p.x; tmat[3][1] = p.y; tmat[3][2] = p.z; tmat[3][3] = 1;
    matrix_multiply(gtl_matrix, tmat);
}

inline void matrix_transform(float gtl_matrix[4][4], Vec3& p)
{
    float outX = p.x * gtl_matrix[0][0] + p.y * gtl_matrix[1][0] + p.z * gtl_matrix[2][0] + gtl_matrix[3][0];
    float outY = p.x * gtl_matrix[0][1] + p.y * gtl_matrix[1][1] + p.z * gtl_matrix[2][1] + gtl_matrix[3][1];
    float outZ = p.x * gtl_matrix[0][2] + p.y * gtl_matrix[1][2] + p.z * gtl_matrix[2][2] + gtl_matrix[3][2];
    p.x = outX; p.y = outY; p.z = outZ;
}




#endif /* ZSPACE_MATH_TYPES_H */