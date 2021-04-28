#pragma once

/*
 * Created by Kami-Kaze on 4/28/2021.
 *
 * todo: sloppy implementation on the fly.
 *  > clean up code
 */

#include <glad/glad.h>

#include <core/logging/engine_log.h>
#include <macros/gl_call.h>
#include <macros/assert.h>

namespace Engine {

    enum TextureType {
        TEXTURE_2D = GL_TEXTURE_2D,
    };

    enum TextureFormatInternal {
        RGBA32F = GL_RGBA32F,
    };

    enum TextureInputFormat {
        RGBA = GL_RGBA
    };

    enum TextureInputType {
        FLOAT = GL_FLOAT,
    };

    enum TextureAccess {
        READ_WRITE = GL_READ_WRITE
    };

    class Texture {
    public:
        Texture(
                TextureType type, unsigned int textureSlot,
                int width, int height,
                int minFilter, int magFilter,
                int wrapS, int wrapT,
                TextureFormatInternal formatInternal,
                TextureInputFormat inputFormat,
                TextureInputType inputType,
                void* data);

        Texture(
                TextureType type, unsigned int textureSlot,
                int width, int height,
                int minFilter, int magFilter,
                int wrapS, int wrapT,
                TextureFormatInternal formatInternal);

        void bufferData(TextureInputFormat inputFormat, TextureInputType inputType, void* data);

        void bind();

        void bindImage(int slot, TextureAccess access);

        void bindUnit(int slot);

        bool isBound();

    private:
        TextureType type;
        int width, height;
        TextureFormatInternal formatInternal;
        unsigned int id, textureSlot;
    };

}



