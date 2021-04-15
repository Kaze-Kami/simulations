#pragma once

/*
 * Created by Kami-Kaze on 4/15/2021.
 *
 * todo: move implementations to .cpp
 */

#include "core/input/input_controller.h"
#include "core/input/key_codes.h"

#include "core/renderer/shader_program/shader_program.h"

#include <string>

#include <glm/glm.hpp>

namespace Engine {

    class Camera {
    public:
        inline Camera(const std::string uniformName = "view") : uniform(Uniform(uniformName, glm::mat4(1.f))) {}

        inline void reset() {
            currentOffset = glm::vec3(0.f);
            currentScale = glm::vec3(1.f);
        }

        inline void translate(glm::vec2 delta) {
            translate(glm::vec3(delta.x, delta.y, 0.f));
        }
        inline void translate(glm::vec3 delta) {
            currentOffset += delta;
            update();
        }

        inline void scale(glm::vec2 delta, glm::vec2 at = glm::vec2(0.f)) {
            scale(glm::vec3(delta.x, delta.y, 1.f), glm::vec3(at.x, at.y, 0.f));
        }
        inline void scale(glm::vec3 delta, glm::vec3 at = glm::vec3(0.f)) {
            glm::vec4 at4 = glm::vec4(at.x, at.y, at.z, 1.f);
            glm::vec4 atScreen0 = viewInverse * at4;

            currentScale *= delta;
            // todo: calculate offset by hand instead of updating twice.
            update();

            glm::vec4 atScreen1 = viewInverse * at4;
            glm::vec4 offsetScreen = atScreen0 - atScreen1;
            glm::vec4 offset = view * offsetScreen;

            currentOffset -= glm::vec3(offset.x, offset.y, offset.z);
            update();
        }

        const Uniform<glm::mat4>& getUniform() {
            return uniform;
        }

        // convert a point in world space ([-1, ..., 1]) to screen space
        inline glm::vec4 apply(glm::vec4 pointInWorldSpace) {
            return view * pointInWorldSpace;
        }

        // convert a point in screen space to world space ([-1, ..., 1])
        inline glm::vec4 applyInverse(glm::vec4 pointInScreenSpace) {
            return viewInverse * pointInScreenSpace;
        }

    private:
        glm::vec3 currentOffset = glm::vec3(0.f);
        glm::vec3 currentScale = glm::vec3(1.f);

        glm::mat4 view = glm::mat4(1.f);
        glm::mat4 viewInverse = glm::mat4(1.f);

        Uniform<glm::mat4> uniform;

        void update() {
            view = glm::mat4(1.f);
            view = glm::translate(view, currentOffset);
            view = glm::scale(view, currentScale);
            uniform.data = view;

            viewInverse = glm::mat4(1.f);
            viewInverse = glm::scale(viewInverse, 1.f / currentScale);
            viewInverse = glm::translate(viewInverse, -currentOffset);
        }
    };


}