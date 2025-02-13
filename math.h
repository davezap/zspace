#ifndef ZSPACE_MATH_TYPES_H
#define ZSPACE_MATH_TYPES_H  1

#include "engine_types.h"

#define fround 
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


inline void MatMult(float mat1[4][4], float mat2[4][4])
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


inline void RotatePntExB(float Matrix[4][4], float fAngX, float fAngY, float fAngZ)
{
    float rmat[4][4];
    
    int AngX = static_cast<int>(fround(fAngX));
    int AngY = static_cast<int>(fround(fAngY));
    int AngZ = static_cast<int>(fround(fAngZ));

    if (AngX > 359) AngX = AngX - 360;
    if (AngX < 0) AngX = AngX + 360;
    if (AngY > 359) AngY = AngY - 360;
    if (AngY < 0) AngY = AngY + 360;
    if (AngZ > 359) AngZ = AngZ - 360;
    if (AngZ < 0) AngZ = AngZ + 360;
    
    float cos = MyCos(fAngX);
    if (AngX == 359)
    {

    }

    
/*
        rmat[0][0] = MyCos(fAngY)*MyCos(fAngZ); rmat[0][1] = MySin(fAngZ)					;rmat[0][2] = -MySin(fAngY);					rmat[0][3] = 0;
        rmat[1][0] = -MySin(fAngZ);               rmat[1][1] = MyCos(fAngZ)*MyCos(fAngX)	;rmat[1][2] = MySin(fAngX);					rmat[1][3] = 0;
        rmat[2][0] = MySin(fAngY);                rmat[2][1] = -MySin(fAngX)				;rmat[2][2] = MyCos(fAngY)*MyCos(fAngX);	rmat[2][3] = 0;
        rmat[3][0] = 0;								rmat[3][1] = 0								;rmat[3][2] = 0;								rmat[3][3] = 1;
        MatMult(Matrix, rmat);
        return;
*/
// Y
    rmat[0][0] = MyCos(fAngY); rmat[0][1] = 0; rmat[0][2] = -MySin(fAngY); rmat[0][3] = 0;
    rmat[1][0] = 0; rmat[1][1] = 1; rmat[1][2] = 0; rmat[1][3] = 0;
    rmat[2][0] = MySin(fAngY); rmat[2][1] = 0; rmat[2][2] = MyCos(fAngY); rmat[2][3] = 0;
    rmat[3][0] = 0; rmat[3][1] = 0; rmat[3][2] = 0; rmat[3][3] = 1;
    MatMult(Matrix, rmat);

    // X
    rmat[0][0] = 1;               rmat[0][1] = 0; rmat[0][2] = 0;               rmat[0][3] = 0;
    rmat[1][0] = 0;               rmat[1][1] = MyCos(fAngX); rmat[1][2] = MySin(fAngX); rmat[1][3] = 0;
    rmat[2][0] = 0;               rmat[2][1] = -MySin(fAngX); rmat[2][2] = MyCos(fAngX); rmat[2][3] = 0;
    rmat[3][0] = 0;               rmat[3][1] = 0; rmat[3][2] = 0;               rmat[3][3] = 1;
    MatMult(Matrix, rmat);

    // Z
    rmat[0][0] = MyCos(fAngZ); rmat[0][1] = MySin(fAngZ); rmat[0][2] = 0; rmat[0][3] = 0;
    rmat[1][0] = -MySin(fAngZ); rmat[1][1] = MyCos(fAngZ); rmat[1][2] = 0; rmat[1][3] = 0;
    rmat[2][0] = 0; rmat[2][1] = 0; rmat[2][2] = 1; rmat[2][3] = 0;
    rmat[3][0] = 0; rmat[3][1] = 0; rmat[3][2] = 0; rmat[3][3] = 1;
    MatMult(Matrix, rmat);



}


inline void Matrix_Ident(float Matrix[4][4])
{
    Matrix[0][0] = 1; Matrix[0][1] = 0; Matrix[0][2] = 0; Matrix[0][3] = 0;
    Matrix[1][0] = 0; Matrix[1][1] = 1; Matrix[1][2] = 0; Matrix[1][3] = 0;
    Matrix[2][0] = 0; Matrix[2][1] = 0; Matrix[2][2] = 1; Matrix[2][3] = 0;
    Matrix[3][0] = 0; Matrix[3][1] = 0; Matrix[3][2] = 0; Matrix[3][3] = 1;
}


inline void RotateAxis(float Matrix[4][4], float fAng, float u, float v, float w)
{
    float rmat[4][4];
    int Ang = static_cast<int>(fround(fAng));

    if (Ang > 359) Ang = Ang - 360;
    if (Ang < 0) Ang = Ang + 360;

    rmat[0][0] = MyCos(fAng) + (u * u) * (1 - MyCos(fAng)); rmat[0][1] = -w * MySin(fAng) + u * v * (1 - MyCos(fAng));	rmat[0][2] = v * MySin(fAng) + u * w * (1 - MyCos(fAng));					rmat[0][3] = 0;
    rmat[1][0] = w * MySin(fAng) + u * v * (1 - MyCos(fAng)); rmat[1][1] = MyCos(fAng) + (v * v) * (1 - MyCos(fAng));		rmat[1][2] = -u * MySin(fAng) + v * w * (1 - MyCos(fAng));					rmat[1][3] = 0;
    rmat[2][0] = -v * MySin(fAng) + u * w * (1 - MyCos(fAng)); rmat[2][1] = u * MySin(fAng) + v * w * (1 - MyCos(fAng));		rmat[2][2] = MyCos(fAng) + (w * w) * (1 - MyCos(fAng));	rmat[2][3] = 0;
    rmat[3][0] = 0;								rmat[3][1] = 0; rmat[3][2] = 0;								rmat[3][3] = 1;
    MatMult(Matrix, rmat);
}

inline void RotatePntExBe(float Matrix[4][4], float fAngX, float fAngY, float fAngZ)
{
    float rmat[4][4];

    int AngX = static_cast<int>(fround(fAngX));
    int AngY = static_cast<int>(fround(fAngY));
    int AngZ = static_cast<int>(fround(fAngZ));
    if (AngX > 359) AngX = AngX - 360;
    if (AngX < 0) AngX = AngX + 360;
    if (AngY > 359) AngY = AngY - 360;
    if (AngY < 0) AngY = AngY + 360;
    if (AngZ > 359) AngZ = AngZ - 360;
    if (AngZ < 0) AngZ = AngZ + 360;


    rmat[0][0] = MyCos(fAngY) * MyCos(fAngZ); rmat[0][1] = MySin(fAngZ); rmat[0][2] = -MySin(fAngY);               rmat[0][3] = 0;
    rmat[1][0] = -MySin(fAngZ);               rmat[1][1] = MyCos(fAngZ) * MyCos(fAngX); rmat[1][2] = MySin(fAngX); rmat[1][3] = 0;
    rmat[2][0] = MySin(fAngY);                rmat[2][1] = -MySin(fAngX); rmat[2][2] = MyCos(fAngY) * MyCos(fAngX); rmat[2][3] = 0;
    rmat[3][0] = 0;								rmat[3][1] = 0; rmat[3][2] = 0;               rmat[3][3] = 1;

    MatMult(Matrix, rmat);

    /*
    // X
    rmat[0][0] = 1;               rmat[0][1] = 0			;rmat[0][2] = 0;               rmat[0][3] = 0;
    rmat[1][0] = 0;               rmat[1][1] = MyCos(fAngX)	;rmat[1][2] = MySin(fAngX); rmat[1][3] = 0;
    rmat[2][0] = 0;               rmat[2][1] = -MySin(fAngX)	;rmat[2][2] = MyCos(fAngX); rmat[2][3] = 0;
    rmat[3][0] = 0;               rmat[3][1] = 0			;rmat[3][2] = 0;               rmat[3][3] = 1;
    MatMult(Matrix, rmat);
    // Y
    rmat[0][0] = MyCos(fAngY); rmat[0][1] = 0; rmat[0][2] = -MySin(fAngY)	; rmat[0][3] = 0;
    rmat[1][0] = 0				; rmat[1][1] = 1; rmat[1][2] = 0				; rmat[1][3] = 0;
    rmat[2][0] = MySin(fAngY); rmat[2][1] = 0; rmat[2][2] = MyCos(fAngY)	; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0; rmat[3][2] = 0				; rmat[3][3] = 1;
    MatMult(Matrix, rmat);

        // Z
    rmat[0][0] = MyCos(fAngZ)	; rmat[0][1] = MySin(fAngZ)	; rmat[0][2] = 0; rmat[0][3] = 0;
    rmat[1][0] = -MySin(fAngZ)	; rmat[1][1] = MyCos(fAngZ)	; rmat[1][2] = 0; rmat[1][3] = 0;
    rmat[2][0] = 0				; rmat[2][1] = 0			; rmat[2][2] = 1; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0			; rmat[3][2] = 0; rmat[3][3] = 1;
    MatMult(Matrix, rmat);
*/
}

inline void TranslateEx(float Matrix[4][4], float Xt, float Yt, float Zt)
{
    float tmat[4][4];

    tmat[0][0] = 1; tmat[0][1] = 0; tmat[0][2] = 0; tmat[0][3] = 0;
    tmat[1][0] = 0; tmat[1][1] = 1; tmat[1][2] = 0; tmat[1][3] = 0;
    tmat[2][0] = 0; tmat[2][1] = 0; tmat[2][2] = 1; tmat[2][3] = 0;
    tmat[3][0] = Xt; tmat[3][1] = Yt; tmat[3][2] = Zt; tmat[3][3] = 1;
    MatMult(Matrix, tmat);
}

inline void Translate(float Matrix[4][4], float& Xt, float& Yt, float& Zt)
{
    float tmat[4][4];

    tmat[0][0] = 1; tmat[0][1] = 0; tmat[0][2] = 0; tmat[0][3] = 0;
    tmat[1][0] = 0; tmat[1][1] = 1; tmat[1][2] = 0; tmat[1][3] = 0;
    tmat[2][0] = 0; tmat[2][1] = 0; tmat[2][2] = 1; tmat[2][3] = 0;
    tmat[3][0] = Xt; tmat[3][1] = Yt; tmat[3][2] = Zt; tmat[3][3] = 1;
    MatMult(Matrix, tmat);
}



inline void Transform(float Matrix[4][4], float& pX, float& pY, float& pZ)
{
    float outX = pX * Matrix[0][0] + pY * Matrix[1][0] + pZ * Matrix[2][0] + Matrix[3][0];
    float outY = pX * Matrix[0][1] + pY * Matrix[1][1] + pZ * Matrix[2][1] + Matrix[3][1];
    float outZ = pX * Matrix[0][2] + pY * Matrix[1][2] + pZ * Matrix[2][2] + Matrix[3][2];
    pX = outX; pY = outY; pZ = outZ;
}





inline float CosAngle(float& AX, float& AY, float& AZ, float& bx, float& by, float& bz)
{
    return float((AX * bx + AY * by + AZ * bz) / (sqrt(AX * AX + AY * AY + AZ * AZ) * sqrt(bx * bx + by * by + bz * bz)));
}


inline void CalcNorm(polygon_type& Poly)
{

    // Calculate normal
    float dAx = (Poly.vertex[1]->wx - Poly.vertex[0]->wx);
    float dAy = (Poly.vertex[1]->wy - Poly.vertex[0]->wy);
    float dAz = (Poly.vertex[1]->wz - Poly.vertex[0]->wz);

    float dBx = (Poly.vertex[2]->wx - Poly.vertex[0]->wx);
    float dBy = (Poly.vertex[2]->wy - Poly.vertex[0]->wy);
    float dBz = (Poly.vertex[2]->wz - Poly.vertex[0]->wz);

    Poly.nX = (dAy * dBz - dAz * dBy);
    Poly.nY = (dAz * dBx - dAx * dBz);
    Poly.nZ = (dAx * dBy - dAy * dBx);

    //( A.y * B.z - A.z * B.y , A.z * B.x - A.x * B.z , A.x * B.y - A.y * B.x)

    //Or we can describe the same equation in terms of vector components:
    //A · B = A.x * B.x + A.y * B.y + A.z * B.z
    //Using a Dot Product, we can obtain the angle between two vectors A and B as follows:
    //Cos q = (A · B) / (|A| · |B|)

    //Poly.D = CosAngle(Poly.vertex[0]->wx+Poly.nX ,  Poly.vertex[0]->wy+Poly.nY, Poly.vertex[0]->wz+Poly.nZ,  Poly.vertex[0]->wx,  Poly.vertex[0]->wy, Poly.vertex[0]->wz);;
    Poly.D = CosAngle(Poly.nX, Poly.nY, Poly.nZ, Poly.vertex[0]->wx, Poly.vertex[0]->wy, Poly.vertex[0]->wz);;
    //Poly.D = 1;
}




#endif /* ZSPACE_MATH_TYPES_H */