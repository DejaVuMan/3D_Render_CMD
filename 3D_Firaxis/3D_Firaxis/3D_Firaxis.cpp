// 3D_Firaxis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "olcConsoleGameEngine.h" // Uses some code from OneLineCoder for generating of console window and triangles
                                  // Math is worked on from scratch in this code, as well as 90% of rendering workload solutions
#include <fstream>
#include <strstream>
#include <algorithm>
using namespace std;

struct vec3d
{
    float x, y, z;
}; // remember to add semicolon at end of all braces

struct triangle // group together 3 vect 3ds
{
    vec3d p[3];

    wchar_t sym;
    short col;
};

struct mesh
{
    vector<triangle> tris;

    bool LoadOBJ(string sFilename) // disable materials and uv data when exporting your OBJ file
    {
        ifstream f(sFilename);
        if (!f.is_open())
            return false;

        //local vertex cache
        vector<vec3d> verts;

        while (!f.eof())
        {
            char line[128]; // assume lines do not exceed 128 characters
            f.getline(line, 128);

            strstream s;
            s << line;

            char junk; // holds 'v' or 'f' etc

            if (line[0] == 'v')
            {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v); // add into vec3d for vertex point data
            }

            if (line[0] == 'f')
            {
                int f[3]; // 3 integer values
                s >> junk >> f[0] >> f[1] >> f[2]; // ex, f 0 33 99 would be result in [0, 33, 99]
                tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]}); 
                // point 0, 1, and 2 of triangle, indexing with face id val
                // we subtract one from each bc the file starts counting from 1
            }


        }

        return true;
    }
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
    vec3d vCamera; // Position of camera - very crude implementation, no direction info - placeholder
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

    CHAR_INFO GetColour(float lum) // inspired and optimized from OneLineCoder's Webcam in Console display implementation
                                   // basically enables actual greyscale color spaces for usage 
    {
        short bg_col, fg_col;
        wchar_t sym;
        int pixel_bw = (int)(13.0f * lum);
        switch (pixel_bw)
        {
        case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

        case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
        case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
        case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

        case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
        case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
        case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

        case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
        case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
        case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
        case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
        default:
            bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
        }

        CHAR_INFO c;
        c.Attributes = bg_col | fg_col;
        c.Char.UnicodeChar = sym;
        return c;
    }

public:
    bool OnUserCreate() override
    {
        //meshCube.tris = {
        //    // assuming max length is 1 - 0,0,0 connects to 0,0,1 to make straight line etc
        //    // triangles are connected together in clockwise fashion

        //// SOUTH
        //{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        //{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        //// EAST                                                      
        //{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        //{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        //// NORTH                                                     
        //{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        //// WEST                                                      
        //{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        //{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        //// TOP                                                       
        //{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        //{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        //// BOTTOM                                                    
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        //};

        meshCube.LoadOBJ("H.obj");

        // Projection Matrix
        float fNear = 0.1f;
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

        vector<triangle> vecTriToRaster;


        // Draw Triangles
        for (auto tri : meshCube.tris)
        {
            // we must condense the 3d into 2d for our 2d space screen
            // PROJECTION - this way we normalize the center as 0,0, the top as -1, bottom as +1, left as -1, right as +1
            // ultimately we must make assumptions, such as aspect ratio (a=h/w)
            // [x,y,z] -> [(h/w)x, y, z] : anything above -1 and +1 wont be drawn
            // really, it should be (h/w)fx and fy, where the f is 1/(tan(theta/2)) - z should also be normalized to optimize algorithm

            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX = {0,0,0};

            // Rotate in Z-Axis
            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(tri.p[i], triRotatedZ.p[i], matRotZ); // rotating illusion on Z axis, 0 to 1 to 2
            }

            // Rotate in X-Axis
            for (int i = 0; i < 3; i++) {
                MultiplyMatrixVector(triRotatedZ.p[i], triRotatedZX.p[i], matRotX); // rotating illusion on X axis
            }

            // Screen Offset
            triTranslated = triRotatedZX;
            for (int i = 0; i < 3; i++) {
                triTranslated.p[i].z = triRotatedZX.p[i].z + 8.0f;
            }

            vec3d normal, line, line2;

            line.x = triTranslated.p[1].x - triTranslated.p[0].x;
            line.y = triTranslated.p[1].y - triTranslated.p[0].y;
            line.z = triTranslated.p[1].z- triTranslated.p[0].z;

            line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            normal.x = line.y * line2.z - line.z * line2.y;
            normal.y = line.z * line2.x - line.x * line2.z;
            normal.z = line.x * line2.y - line.y * line2.x;

            float normed = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= normed; normal.y /= normed; normal.z /= normed;


            if(normal.x * (triTranslated.p[0].x - vCamera.x) +
               normal.y * (triTranslated.p[0].y - vCamera.y) +
               normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f){

                // Shading info, only when visible to improve overall performance
                // Illumination
                vec3d lightinfo = { 0.0f, 0.0f, -1.0f }; // single direction light, not real world though
                // triangle is more lit the more normal is aligned with Z direction - basic assumption
                float normed = sqrtf(lightinfo.x * lightinfo.x + lightinfo.y * lightinfo.y + lightinfo.z * lightinfo.z);
                lightinfo.x /= normed; lightinfo.y /= normed; lightinfo.z /= normed;

                float dp = normal.x * lightinfo.x + normal.y * lightinfo.y + normal.z * lightinfo.z;

                CHAR_INFO c = GetColour(dp); // Console specific since we have a limited color palette instead of working in a true RGB space
                triTranslated.col = c.Attributes;
                triTranslated.sym = c.Char.UnicodeChar;

                // Projection from 3d to 2d
                for (int i = 0; i < 3; i++) {
                    MultiplyMatrixVector(triTranslated.p[i], triProjected.p[i], matProj); // cant use triangle directly
                }
                // ensures color info remains after translation - we could do this in conversion, but encapsulation makes it easier to follow.
                // also makes it easier if we should make our engine more complex in the future.
                triProjected.col = triTranslated.col; 
                triProjected.sym = triTranslated.col;

                //Scale into view
                triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

                for (int i = 0; i < 3; i++) {
                    triProjected.p[i].x *= 0.5f * (float)ScreenWidth();
                    triProjected.p[i].y *= 0.5f * (float)ScreenHeight();
                };

                // Storing triangles to sort first before rasterizing
                vecTriToRaster.push_back(triProjected);

                ////We can use DrawTriangle for wire mesh cube and debugging
                ///*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                //    triProjected.p[1].x, triProjected.p[1].y,
                //    triProjected.p[2].x, triProjected.p[2].y,
                //    PIXEL_SOLID, BG_BLACK);*/
            }

        }

        // sort from back to front
        sort(vecTriToRaster.begin(), vecTriToRaster.end(), [](triangle& t1, triangle& t2)
        {
            // we want to get mid point z values of both triangles, approximation so occasional glitch might occur
                float z = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return z > z2;
        });

        for (auto& triProjected : vecTriToRaster)
        {
            FillTriangle(triProjected.p[0].x, triProjected.p[0].y, // rasterize and fill in
                triProjected.p[1].x, triProjected.p[1].y,
                triProjected.p[2].x, triProjected.p[2].y,
                triProjected.sym, triProjected.col);
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
    *((unsigned int*)0) = 0xDEAD;
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
