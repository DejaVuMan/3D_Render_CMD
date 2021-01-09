// 3D_Firaxis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "olcConsoleGameEngine.h" // Uses some code from OneLineCoder for generating of console window and triangles
                                  // Math is worked on from scratch in this code, as well as 90% of rendering workload solutions
using namespace std;

struct vec3d
{
    float x, y, z;
}; // remember to add semicolon at end of all braces

struct triangle // group together 3 vect 3ds
{
    vec3d p[3];
};

struct mesh
{
    vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = { 0 }; // initialize to 0, row followed by column
};


class olcEngine3D : public olcConsoleGameEngine
{
public:
    olcEngine3D()
    {
        m_sAppName = L"3D Demo"; // Unicode is enabled by default as a quick reminder
    }

private:
    mesh meshCube;
    mat4x4 matProj;

    float fTheta;

    void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
    {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; // mysterious 4th element implied 1

        // 4d to 3d, division by w unless equal to 0
        if (w != 0.0f)
        {
            o.x /= w; o.y /= w; o.z /= w;
        }
    }

public:
    bool OnUserCreate() override
    {
        meshCube.tris = {
            // assuming max length is 1 - 0,0,0 connects to 0,0,1 to make straight line etc
            // triangles are connected together in clockwise fashion

        // SOUTH
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        // EAST                                                      
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        // NORTH                                                     
        { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        // WEST                                                      
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        // TOP                                                       
        { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        // BOTTOM                                                    
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        };

        // Projection Matrix
        float fNear = 0.08f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectR = (float)ScreenHeight() / (float)ScreenWidth();
        float fFovR = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f); // converted from degree to radian

        matProj.m[0][0] = fAspectR * fFovR;
        matProj.m[1][1] = fFovR;
        matProj.m[2][2] = fFar / (fFar - fNear);
        matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        mat4x4 matRotZ, matRotX;

        fTheta += 1.0f * fElapsedTime;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        // Draw Triangles
        for (auto tri : meshCube.tris)
        {
            // we must condense the 3d into 2d for our 2d space screen
            // PROJECTION - this way we normalize the center as 0,0, the top as -1, bottom as +1, left as -1, right as +1
            // ultimately we must make assumptions, such as aspect ratio (a=h/w)
            // [x,y,z] -> [(h/w)x, y, z] : anything above -1 and +1 wont be drawn
            // really, it should be (h/w)fx and fy, where the f is 1/(tan(theta/2)) - z should also be normalized to optimize algorithm
            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate in Z-Axis
            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(tri.p[i], triRotatedZ.p[i], matRotZ); // rotating illusion on Z axis, 0 to 1 to 2
            }

            // Rotate in X-Axis
            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(triRotatedZ.p[i], triRotatedZX.p[i], matRotX); // rotating illusion on X axis
            }

            triTranslated = triRotatedZX;
            for (int i = 0; i < 3; i++) {
                triTranslated.p[i].z = triRotatedZX.p[i].z + 3.0f;
            }

            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(triTranslated.p[i], triProjected.p[i], matProj); // cant use triangle directly
            }

            //Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            for (int i = 0; i < 3; i++) {
                triProjected.p[i].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[i].y *= 0.5f * (float)ScreenHeight();
            }

            DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                triProjected.p[1].x, triProjected.p[1].y,
                triProjected.p[2].x, triProjected.p[2].y,
                PIXEL_SOLID, BG_WHITE); // if we left it just as this we'd only get small cube in upper left, we must scale it now

        }

        return true; // tells engine everything is fine and to continue running
    }

};

int main()
{
    olcEngine3D demo;
    if (demo.ConstructConsole(256, 256, 2, 2)) // 256x256 res, @ 2x2 pixels for characters - on my Windows build, anything higher than 2x2 results in error
        demo.Start(); // Roughly uses 2MB of memory and 1% CPU Utilization on i7 7700HQ with Single Channel DDR4-2400 16GB
    else
        return 3; // 3 error code for failure to create window
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
