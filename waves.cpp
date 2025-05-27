#pragma once

#include <cmath>
#include <raylib.h>

class Waves final {
#define WAVES_THREADS 1
#define WAVES_FREQ 45
#define bufSize 18204
#define linesAmt 8
#define speed 1
    bool completeFlags[WAVES_THREADS]{};
    int scrX, scrY;
    float ref = 0;
    float alphaMul = 1.0f;

    Vector3 buffer[WAVES_THREADS][bufSize][2]{};

    void lineWorker() {
        const auto instance = this;
        populateBuffer(instance->buffer[0], &instance->completeFlags[0], instance->ref, instance);
    }

    void drawLine(Vector3 v[2]) const {
        constexpr unsigned char r = 0x15, g = 0x17, b = 0x1A;
        const Vector2 vb = {v[0].x, v[0].y};
        const Vector2 ve = {v[1].x, v[1].y};
        const float cl = v[0].z*2;
        Color col = {(unsigned char) (r+(cl*2)), (unsigned char) (g+(cl*2)), (unsigned char) (b+(cl*2)), (unsigned char) (40+(cl*4))};
        float vv = ((float) col.a + 100);
        if (ve.x > (float)scrX/3*2) {
            vv -= (ve.x - (float)scrX/3*2+1) / 25;
        }
        vv *= alphaMul;
        if (vv < 0) {
            vv = 0;
        }
        col.a = (unsigned char) vv;
        DrawLineEx(vb, ve, 15, col);
    }

    static void populateBuffer(Vector3 vector2[18204][2], bool *done, const double ref, const Waves *inst) {
        const int sizeX = inst->scrX + 100;
        const int yMod = inst->scrY / 2;
        float lastPointX [linesAmt];
        float lastPointY [linesAmt];
        int c = 0;
        for (auto cl = (float) 0; cl < linesAmt; cl++) { // NOLINT(*-flp30-c)
            constexpr int skip = 20; // NOLINT(*-flp30-c)
            for (int i = 0; i < sizeX; i += skip) {
                const auto x = (float) i;
                auto ym = (sizeX - x + (ref - cl*30)) / skip / 6; // NOLINT(*-narrowing-conversions)
                ym *= 100;
                ym = (float) (int) ym / 100;
                ym = std::fmod(ym, 2*3.14f);
                auto y = (sin(ym) * 150) / ((x+175)/inst->scrX*1.75f) / 7.5; // NOLINT(*-narrowing-conversions)
                if (i != 0) {
                    constexpr int sizeY = 25;
                    const auto thisSize = sizeY - (cl * 4);
                    for (auto l = -thisSize; l < thisSize; l++) { // NOLINT(*-flp30-c)
                        vector2[c][0] = Vector3{.x = float(int(lastPointX[int(cl)] - 10)), .y = float(int(lastPointY[int(cl)] + yMod + l*2)), .z = float(cl)}; // NOLINT(*-narrowing-conversions)
                        vector2[c][1] = Vector3{.x = float(x - 10), .y =  float(y + yMod + l*2), .z =  1};
                        vector2[c+1][1].z = 0;
                        c++;
                    }
                }
                lastPointY[int(cl)] = (float) int(y);
                lastPointX[int(cl)] = (float) int(x);
            }
        }
        *done = true;
    }

    void drawBuffer(Vector3 b[bufSize][2]) const {
        for (int i = 0; i < bufSize; i++) {
            if (b[i][1].z != 1) {
                return;
            }
            drawLine(b[i]);
        }
    }

public:
    Waves(int scrX, int scrY) {
        this->scrX = scrX;
        this->scrY = scrY;
    }

    void resize(int x, int y) {
        scrX = x;
        scrY = y;
    }

    void draw() {
        lineWorker();
        for (auto & i : buffer) drawBuffer(i);
        ref+=speed;
    }

    void setAlphaPercent(float p = 1.0f) {
        if (p > 1) p = 1;
        else if (p < 0) p = 0;
        alphaMul = p;
    }

    [[nodiscard]] float getAlphaPercent() const {
        return alphaMul;
    }
};
