#pragma once

/*
 * Created by Kami-Kaze on 4/24/2021.
 */

#include <core/application/application.h>

using namespace Engine;

class NBodiesApplication : public Application {

public:
    void init() override;
    void shutdown() override;

protected:
    void setup(ApplicationProps& props) override;
    void update(float dt) override;
    void render(Context* context) override;
    void renderImGui() override;

};