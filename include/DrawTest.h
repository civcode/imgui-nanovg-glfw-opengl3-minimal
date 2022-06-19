#ifndef DRAWTEST_H_
#define DRAWTEST_H_

#include <vector>
#include <stack>
#include <queue>
#include <cstdio>

#include <Eigen/Dense>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "nanovg.h"
#include "SimpleTimer.h"

#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

using std::vector;
//using std::stack;

class DrawTest {

public:
    DrawTest(GLFWwindow *window, NVGcontext *ctx);

    void draw();


private:

    typedef struct Vec2i_ {
        int x;
        int y;
    } Vec2i; 

    typedef struct Vec2f_ {
        float x;
        float y;
    } Vec2f;

    enum class ECellType {
        kFree,
        kOccupied,
        kVisited,
        kAdjacent
    };

    typedef struct Cell_ {
        bool has_changed;
        ECellType type;

    } Cell;

    typedef struct Grid_ {
        bool isGridOn;
        int cellSizePx;
        //Eigen::Vector2i origin;
        //Eigen::Vector2i size;
        int width;
        int height;
        Vec2f offsetPx;
        vector<vector<Cell>> cells;
        
        //Eigen::Vector2i offset;
    } Grid;

    int winWidth_;
    int winHeight_;
    int fbWidth_;
    int fbHeight_;

    float pxRatio_;
    float zoom_;

    GLFWwindow *window_;
    NVGcontext *vg_;
    NVGLUframebuffer *fb_;

    Grid grid_;
    std::stack<Vec2i> stack_;
    std::queue<Vec2i> queue_;

    SimpleTimer *timer_;

    void set_occupied();
    void DrawToFb();

};

#endif // DRAWTEST_H_
