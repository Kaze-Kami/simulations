#pragma once

/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include <core/application/application.h>

class SandboxApplication : public Engine::Application {
public:
    void onContextAttach() override;

    void onContextDetach() override;

protected:
    void setup(Engine::ApplicationProps& props) override;

    void update(float dt) override;

    void render(Engine::Context* context) override;
};



