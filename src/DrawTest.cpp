#include "DrawTest.h"


DrawTest::DrawTest(GLFWwindow *window, NVGcontext *ctx) :
    zoom_(1),
    dataStructureType_(EDataStructureType::kStack)
{
    window_ = window;
    vg_ = ctx;

    grid_.isGridOn = true;
    grid_.cellSizePx = 10;
    grid_.width = 35;
    grid_.height = 25;
    grid_.offsetPx = {30, 30};
    //grid_.cells[0].resize(100);
    //grid_.cells[1].resize(grid_.height);
    Cell c = {.has_changed = false, 
              .type = ECellType::kFree};
    grid_.cells = vector<vector<Cell>> (grid_.height, vector<Cell>(grid_.width, c));
    // for (int mrow=0; mrow<grid_.height; mrow++) {
    //     for (int ncol=0; ncol<grid_.width; ncol++) {
    //         //grid_.cells[m][n].type = ECellType::kFree;
    //         Cell c = {.type = ECellType::kFree};
    //         //grid_.cells[m  .push_back(c);
    //     }
    // }
    Vec2i p = {0, 0};
    stack_.push(p);
    queue_.push(p);
    grid_.cells[0][0].has_changed = true;
    timer_ = new SimpleTimer(std::chrono::milliseconds(100));

    this->set_occupied();

    glfwGetWindowSize(window_, &winWidth_, &winHeight_);
    glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
    pxRatio_ = static_cast<float>(fbWidth_) / static_cast<float>(winWidth_);
    printf("pxRatio = %f\n", pxRatio_);

    int fbw = grid_.width * grid_.cellSizePx * pxRatio_; 
    int fbh = grid_.height * grid_.cellSizePx * pxRatio_; 
    //fb_ = nvgluCreateFramebufferGL3(vg_, fbw, fbh, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
    fb_ = nvgluCreateFramebufferGL3(vg_, fbw, fbh, 0);
    //fb_ = nvgluCreateFramebufferGL3(vg_, fbw, fbw, 0);

    if (fb_ == nullptr) {\
        printf("Could not create FBO.\n");
    }

    this->DrawToFb();
}

void DrawTest::set_occupied() {
    vector<Vec2i> occ = { {3, 5}, {8, 0}, {4, 0}, {6, 6}, {6,7}, {6,8}, /*{6,9}, {6,10},*/
                          {7,7}, {8,7}, {9,7} };
    for (auto elem : occ) {
        int col = elem.y;
        int row = elem.x;
        grid_.cells[row][col].type = ECellType::kOccupied;
        grid_.cells[row][col].has_changed = true;
    }
}


/*
void DrawTest::DrawToFb() {

    int winWidth, winHeight;
    int fboWidth, fboHeight;
    int pw, ph, x, y;
    static float fx = 1;
    float s = 20.0f;
    float sr = (cosf(fx) + 1) * 0.5f;
    float r = s * 0.6f * (0.2f + 0.8f * sr);
    fx += 0.2;

    if (fb_ == nullptr)
        return;

    nvgImageSize(vg_, fb_->image, &fboWidth, &fboHeight);
    winWidth = (int)(fboWidth / pxRatio_);
    winHeight = (int)(fboHeight / pxRatio_);

    // Draw some stuff to an FBO as a test
    nvgluBindFramebufferGL3(fb_);
    glViewport(0, 0, fboWidth, fboHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(vg_, winWidth, winHeight, pxRatio_);

    pw = (int)ceilf(winWidth / s);
    ph = (int)ceilf(winHeight / s);

    nvgBeginPath(vg_);
    for (y = 0; y < ph; y++) {
        for (x = 0; x < pw; x++) {
            float cx = (x + 0.5f) * s;
            float cy = (y + 0.5f) * s;
            nvgCircle(vg_, cx, cy, r);
        }
    }
    nvgFillColor(vg_, nvgRGBA(220, 160, 0, 200));
    nvgFill(vg_);

    // ImGui::Begin("fb");
    // if (ImGui::Button("step")) {
    //     nvgCircle(vg_, 50, 50, 20);
    //     nvgFillColor(vg_, nvgRGBAf(1,0,0,1));
    //     nvgFill(vg_);
    // }
    // ImGui::End();

    nvgClosePath(vg_);

    nvgEndFrame(vg_);
    nvgluBindFramebufferGL3(nullptr);    
}
*/

void DrawTest::DrawToFb() {

    int winWidth, winHeight;
    int fboWidth, fboHeight;
    int pw, ph, x, y;

    if (fb_ == nullptr)
        return;

    nvgImageSize(vg_, fb_->image, &fboWidth, &fboHeight);
    printf("fbo_size [w,h] = [%d, %d]\n", fboWidth, fboHeight);
    winWidth = (int)(fboWidth / pxRatio_);
    winHeight = (int)(fboHeight / pxRatio_);

    // Draw some stuff to an FBO as a test
    nvgluBindFramebufferGL3(fb_);
    glViewport(0, 0, fboWidth, fboHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(vg_, winWidth, winHeight, pxRatio_);

    nvgBeginPath(vg_);
    for (int i=0; i<grid_.height+1; i++) {
        float xFrom = 0;
        float xTo = grid_.width*grid_.cellSizePx;
        float y = i*grid_.cellSizePx;
        nvgMoveTo(vg_, xFrom, y);
        nvgLineTo(vg_, xTo, y);
    }
    //vertical lines
    //for (int i=0; i<grid_.width/grid_.cellSizePx+1; i++) {
    for (int i=0; i<grid_.width+1; i++) {
        float x = i*grid_.cellSizePx;
        float yFrom = 0;
        float yTo = grid_.height*grid_.cellSizePx;
        nvgMoveTo(vg_, x, yFrom);
        nvgLineTo(vg_, x, yTo);
    }
    nvgStrokeColor(vg_, nvgRGBAf(0.2, 0.2, 0.2, 1));
    nvgStrokeWidth(vg_, 1.0);
    nvgStroke(vg_);
    nvgClosePath(vg_);

    // ImGui::Begin("fb");
    // if (ImGui::Button("step")) {
    //     nvgCircle(vg_, 50, 50, 20);
    //     nvgFillColor(vg_, nvgRGBAf(1,0,0,1));
    //     nvgFill(vg_);
    // }
    // ImGui::End();

    nvgEndFrame(vg_);
    nvgluBindFramebufferGL3(nullptr);    
}
// void DrawTest::DataStructure_pop(Vec2i p) {
//     switch (dataStructureType_) {
//         case EDataStructureType::kStack: 
//             stack_.pop();
//             break;
//         case EDataStructureType::kQueue:
//             queue_.pop();
//             break;
//     }
    
// }
// DrawTest::Vec2i DrawTest::DataStructure_get(Vec2i p) {
//     Vec2i p = {0,0};
//     switch (dataStructureType_) {
//         case EDataStructureType::kStack: 
//             p = stack_.top();
//             break;
//         case EDataStructureType::kQueue:
//             p = queue_.front();
//             break;
//     }
//     return p;
// }
// void DrawTest::DataStructure_push(Vec2i p) {
//     switch (dataStructureType_) {
//         case EDataStructureType::kStack: 
//             stack_.push(p);
//             break;
//         case EDataStructureType::kQueue:
//             queue_.push(p);
//             break;
//     }
// }
/*
void DrawTest::draw() {

    static bool run = false;

    // if (run) {
    //     run = false;
    // } else {
    //     return;
    // }
    ImGui::Begin("main");
    //ImGui::SetWindowPos("main", ImVec2(10, 0));
    if (ImGui::Button("step") || (run && timer_->is_expired())) {
        //printf("test\n");
        if (stack_.size() > 0) {
            Vec2i p = stack_.top();
            //printf("top = [%d, %d]\n", p.x, p.y);
            stack_.pop();
            int x, y;
            //ECellType type;
            ECellType type = grid_.cells[p.y][p.x].type;
            //if (grid_.cells[p.y][p.x].type != ECellType::kVisited &&
            //    grid_.cells[p.y][p.x].type != ECellType::kOccupied) {
            if (type != ECellType::kVisited &&
                type != ECellType::kOccupied) {
                grid_.cells[p.y][p.x].type = ECellType::kVisited;
                x = p.x - 1;
                y = p.y;
                if (x >= 0) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        stack_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x;
                y = p.y - 1;
                if (y >= 0) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        stack_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x + 1;
                y = p.y;
                if (x < grid_.width) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        stack_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x;
                y = p.y + 1;
                if (y < grid_.height) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        stack_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }

                }

            }
        } else {
            printf("stack is empty\n");
            run = false;
        }
    }  
    static int interval = 0;
    //ImGui::Button("adjacent");
    //if (ImGui::Button("run")) {
    if (ImGui::Button(!run ? "run" : "stop")) {
        run = !run;
        timer_->set_interval(std::chrono::milliseconds(interval));
    }
    if (ImGui::Button("reset")) {
        Vec2i p = {0, 0};
        //std::stack<Vec2i> s;
        stack_ = std::stack<Vec2i>();
        stack_.push(p);
        Cell c = {.has_changed = false, .type = ECellType::kFree};
        grid_.cells = vector<vector<Cell>> (grid_.height, vector<Cell>(grid_.width, c));
        grid_.cells[0][0].has_changed = true;
        this->set_occupied();
    }
    if (ImGui::SliderInt("interval in ms", &interval, 0, 200)) {
        timer_->set_interval(std::chrono::milliseconds(interval));
    }
    //ImGui::Checkbox("Show step buttons", true);
    bool input_step = true;
    float f32_one = 1;
    float f32_half = 0.5;
    float f32_step = 0.5;
    if (ImGui::InputScalar("zoom", ImGuiDataType_Float, &zoom_, input_step ? &f32_step : NULL)) {
        if (zoom_ < f32_step)
            zoom_ = f32_step;

    }
    ImGui::End();
    
    glfwGetWindowSize(window_, &winWidth_, &winHeight_);
    glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
    pxRatio_ = static_cast<float>(fbWidth_) / static_cast<float>(winWidth_);

    // Grid grid = {.isGridOn = true,
    //              .cellSize = 10,
    //              //.origin = {50, 50},
    //              .width = 150,
    //              .height = 100,
    //              .offset = {50, 50}
    //             };

    {
        nvgBeginFrame(vg_, winWidth_, winHeight_, pxRatio_);
        nvgSave(vg_);
        nvgTranslate(vg_, grid_.offsetPx.x, grid_.offsetPx.y);
        nvgScale(vg_, zoom_, zoom_);
        //horizontal lines
        //for (int i=0; i<grid_.height/grid_.cellSizePx+1; i++) {
        
        nvgBeginPath(vg_);
        //nvgRect(vg_, grid_.offsetPx.x, grid_.offsetPx.y, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
        nvgRect(vg_, 0, 0, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
        nvgFillColor(vg_, nvgRGBf(1,1,1));
        nvgFill(vg_);
        nvgClosePath(vg_);

        for (int mrow=0; mrow<grid_.height; mrow++) {
            for (int ncol=0; ncol<grid_.width; ncol++) {
                bool has_changed = grid_.cells[mrow][ncol].has_changed;
                ECellType type = grid_.cells[mrow][ncol].type;
                float x = ncol * grid_.cellSizePx;
                float y = mrow * grid_.cellSizePx;
                float w = grid_.cellSizePx;
                float h = w;
                if (type != ECellType::kFree && has_changed) {
                    //grid_.cells[mrow][ncol].has_changed = false;
                    nvgBeginPath(vg_);
                    nvgRect(vg_, x, y, w, h);
                    switch (type) {
                        case ECellType::kFree: 
                            nvgFillColor(vg_, nvgRGBf(1,1,1));
                            nvgFill(vg_);
                            break;
                        case ECellType::kOccupied: 
                            nvgFillColor(vg_, nvgRGBf(0.1, 0.1, 0.1));
                            nvgFill(vg_);
                            break;
                        case ECellType::kVisited: 
                            nvgFillColor(vg_, nvgRGBf(0,1,0));
                            nvgFill(vg_);
                            break;
                        case ECellType::kAdjacent: 
                            nvgFillColor(vg_, nvgRGBAf(0.5,0,0,0.5));
                            //nvgFillColor(vg_, nvgRGBA(255, 204, 255, 255));
                            //nvgFillColor(vg_, nvgRGBA(204, 102, 255, 255));
                            nvgFill(vg_);
                            break;

                    }
                    nvgClosePath(vg_);
                }
            }
        }
        //printf("\n");
        //nvgFillColor(vg_, nvgRGBf(0.8, 0.8, 0.8));
        //nvgFill(vg_);
        //nvgRestore(vg_);
        //nvgTranslate(vg_, grid_.offsetPx.x, grid_.offsetPx.y);
        //nvgScale(vg_, 2.0, 2.0);

        nvgBeginPath(vg_);
        for (int i=0; i<grid_.height+1; i++) {
            float xFrom = 0;
            float xTo = grid_.width*grid_.cellSizePx;
            float y = i*grid_.cellSizePx;
            nvgMoveTo(vg_, xFrom, y);
            nvgLineTo(vg_, xTo, y);
        }
        //vertical lines
        //for (int i=0; i<grid_.width/grid_.cellSizePx+1; i++) {
        for (int i=0; i<grid_.width+1; i++) {
            float x = i*grid_.cellSizePx;
            float yFrom = 0;
            float yTo = grid_.height*grid_.cellSizePx;
            nvgMoveTo(vg_, x, yFrom);
            nvgLineTo(vg_, x, yTo);
        }
        // for (i=0; i<grid_0.w/grid_0.d+1; i++) {
        //     nvgMoveTo(vg, grid_0.x+i*grid_0.d, grid_0.y);
        //     nvgLineTo(vg, grid_0.x+i*grid_0.d, grid_0.y+grid_0.h);
        // }
        nvgStrokeColor(vg_, nvgRGBAf(0.2, 0.2, 0.2, 1));
        nvgStrokeWidth(vg_, 1.0);
        nvgStroke(vg_);
        nvgClosePath(vg_);
        nvgRestore(vg_);

        nvgEndFrame(vg_);

    }

}
*/

void DrawTest::draw() {



    static bool run = false;

    // if (run) {
    //     run = false;
    // } else {
    //     return;
    // }
    ImGui::Begin("main");
    static int e = 0;
    ImGui::RadioButton("DFS", &e, 0); ImGui::SameLine();
    ImGui::RadioButton("BFS", &e, 1); //ImGui::SameLine();
    if (e == 0) algorithmType_ = EAlgorithmType::kDFS;
    if (e == 1) algorithmType_ = EAlgorithmType::kBFS;

    //ImGui::SetWindowPos("main", ImVec2(10, 0));
    if (ImGui::Button("step") || (run && timer_->is_expired())) {
        //printf("test\n");
        if (queue_.size() > 0) {
            Vec2i p = queue_.front();
            //printf("top = [%d, %d]\n", p.x, p.y);
            queue_.pop();
            int x, y;
            //ECellType type;
            ECellType type = grid_.cells[p.y][p.x].type;
            //if (grid_.cells[p.y][p.x].type != ECellType::kVisited &&
            //    grid_.cells[p.y][p.x].type != ECellType::kOccupied) {
            if (type != ECellType::kVisited &&
                type != ECellType::kOccupied) {
                grid_.cells[p.y][p.x].type = ECellType::kVisited;
                x = p.x - 1;
                y = p.y;
                if (x >= 0) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        queue_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x;
                y = p.y - 1;
                if (y >= 0) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        queue_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x + 1;
                y = p.y;
                if (x < grid_.width) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        queue_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }
                }
                x = p.x;
                y = p.y + 1;
                if (y < grid_.height) {
                    type = grid_.cells[y][x].type;
                    if (type != ECellType::kAdjacent && type != ECellType::kVisited && type != ECellType::kOccupied) {
                        grid_.cells[y][x].type = ECellType::kAdjacent;
                        grid_.cells[y][x].has_changed = true;
                        Vec2i p = {.x = x, .y = y};
                        queue_.push(p);
                        //printf("push [%d, %d]\n", y, x);
                    }

                }

            }
        } else {
            printf("stack is empty\n");
            run = false;
        }
    }  
    static int interval = 0;
    //ImGui::Button("adjacent");
    //if (ImGui::Button("run")) {
    if (ImGui::Button(!run ? "run" : "stop")) {
        run = !run;
        timer_->set_interval(std::chrono::milliseconds(interval));
    }
    if (ImGui::Button("reset")) {
        Vec2i p = {0, 0};
        //std::stack<Vec2i> s;
        stack_ = std::stack<Vec2i>();
        stack_.push(p);
        queue_ = std::queue<Vec2i>();
        queue_.push(p);
        Cell c = {.has_changed = false, .type = ECellType::kFree};
        grid_.cells = vector<vector<Cell>> (grid_.height, vector<Cell>(grid_.width, c));
        grid_.cells[0][0].has_changed = true;
        this->set_occupied();
    }
    if (ImGui::SliderInt("interval in ms", &interval, 0, 200)) {
        timer_->set_interval(std::chrono::milliseconds(interval));
    }
    //ImGui::Checkbox("Show step buttons", true);
    bool input_step = true;
    float f32_one = 1;
    float f32_half = 0.5;
    float f32_step = 0.5;
    if (ImGui::InputScalar("zoom", ImGuiDataType_Float, &zoom_, input_step ? &f32_step : NULL)) {
        if (zoom_ < f32_step)
            zoom_ = f32_step;

    }
    ImGui::End();
    
    glfwGetWindowSize(window_, &winWidth_, &winHeight_);
    glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
    pxRatio_ = static_cast<float>(fbWidth_) / static_cast<float>(winWidth_);

    // Grid grid = {.isGridOn = true,
    //              .cellSize = 10,
    //              //.origin = {50, 50},
    //              .width = 150,
    //              .height = 100,
    //              .offset = {50, 50}
    //             };

    {
        nvgBeginFrame(vg_, winWidth_, winHeight_, pxRatio_);
        nvgSave(vg_);
        nvgTranslate(vg_, grid_.offsetPx.x, grid_.offsetPx.y);
        nvgScale(vg_, zoom_, zoom_);
        //horizontal lines
        //for (int i=0; i<grid_.height/grid_.cellSizePx+1; i++) {
        
        nvgBeginPath(vg_);
        //nvgRect(vg_, grid_.offsetPx.x, grid_.offsetPx.y, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
        nvgRect(vg_, 0, 0, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
        nvgFillColor(vg_, nvgRGBf(1,1,1));
        nvgFill(vg_);
        nvgClosePath(vg_);

        for (int mrow=0; mrow<grid_.height; mrow++) {
            for (int ncol=0; ncol<grid_.width; ncol++) {
                bool has_changed = grid_.cells[mrow][ncol].has_changed;
                ECellType type = grid_.cells[mrow][ncol].type;
                float x = ncol * grid_.cellSizePx;
                float y = mrow * grid_.cellSizePx;
                float w = grid_.cellSizePx;
                float h = w;
                if (type != ECellType::kFree && has_changed) {
                    //grid_.cells[mrow][ncol].has_changed = false;
                    nvgBeginPath(vg_);
                    nvgRect(vg_, x, y, w, h);
                    switch (type) {
                        case ECellType::kFree: 
                            nvgFillColor(vg_, nvgRGBf(1,1,1));
                            nvgFill(vg_);
                            break;
                        case ECellType::kOccupied: 
                            nvgFillColor(vg_, nvgRGBf(0.1, 0.1, 0.1));
                            nvgFill(vg_);
                            break;
                        case ECellType::kVisited: 
                            nvgFillColor(vg_, nvgRGBf(0,1,0));
                            nvgFill(vg_);
                            break;
                        case ECellType::kAdjacent: 
                            nvgFillColor(vg_, nvgRGBAf(0.5,0,0,0.5));
                            //nvgFillColor(vg_, nvgRGBA(255, 204, 255, 255));
                            //nvgFillColor(vg_, nvgRGBA(204, 102, 255, 255));
                            nvgFill(vg_);
                            break;

                    }
                    nvgClosePath(vg_);
                }
            }
        }
        //printf("\n");
        //nvgFillColor(vg_, nvgRGBf(0.8, 0.8, 0.8));
        //nvgFill(vg_);
        //nvgRestore(vg_);
        //nvgTranslate(vg_, grid_.offsetPx.x, grid_.offsetPx.y);
        //nvgScale(vg_, 2.0, 2.0);

        if (true) {
            // use fb
            float iw = grid_.width*grid_.cellSizePx;
            float ih = grid_.height*grid_.cellSizePx;
            NVGpaint img = nvgImagePattern(vg_, 0, 0, iw, ih, 0, fb_->image, 0.2f);

            nvgBeginPath(vg_);
            //nvgRoundedRect(vg_, 300, 30, 100, 100, 5);
            nvgRect(vg_, 0, 0, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
            nvgFillPaint(vg_, img);
            nvgFill(vg_);
            nvgClosePath(vg_);
        } else {
            // draw new
            nvgBeginPath(vg_);
            for (int i=0; i<grid_.height+1; i++) {
                float xFrom = 0;
                float xTo = grid_.width*grid_.cellSizePx;
                float y = i*grid_.cellSizePx;
                nvgMoveTo(vg_, xFrom, y);
                nvgLineTo(vg_, xTo, y);
            }
            //vertical lines
            //for (int i=0; i<grid_.width/grid_.cellSizePx+1; i++) {
            for (int i=0; i<grid_.width+1; i++) {
                float x = i*grid_.cellSizePx;
                float yFrom = 0;
                float yTo = grid_.height*grid_.cellSizePx;
                nvgMoveTo(vg_, x, yFrom);
                nvgLineTo(vg_, x, yTo);
            }
            // for (i=0; i<grid_0.w/grid_0.d+1; i++) {
            //     nvgMoveTo(vg, grid_0.x+i*grid_0.d, grid_0.y);
            //     nvgLineTo(vg, grid_0.x+i*grid_0.d, grid_0.y+grid_0.h);
            // }
            nvgStrokeColor(vg_, nvgRGBAf(0.2, 0.2, 0.2, 1));
            nvgStrokeWidth(vg_, 1.0);
            nvgStroke(vg_);
            nvgClosePath(vg_);
        }        

        nvgRestore(vg_);

        nvgEndFrame(vg_);

    }

    // ImGui::Begin("fb");
    // if (ImGui::Button("step")) {
    //     //nvgCircle(vg_, 50, 50, 20);
    //     //nvgFillColor(vg_, nvgRGBAf(1,0,0,1));
    //     //nvgFill(vg_);

    // this->DrawToFb();
    // }
    // ImGui::End();

    // NVGpaint img = nvgImagePattern(vg_, 0, 0, 100, 100, 0, fb_->image, 1.0f);

    // nvgBeginPath(vg_);
    // //nvgRoundedRect(vg_, 300, 30, 100, 100, 5);
    // nvgRect(vg_, 0, 0, grid_.width*grid_.cellSizePx, grid_.height*grid_.cellSizePx);
    // nvgFillPaint(vg_, img);
    // nvgFill(vg_);
    // nvgClosePath(vg_);

}