/*
 * Created by Kami-Kaze on 4/28/2021.
 */
#include "texture.h"

namespace Engine {


    Texture::Texture(
            TextureType type, unsigned int textureSlot,
            int width, int height, int minFilter, int magFilter, int wrapS, int wrapT,
            TextureFormatInternal formatInternal, TextureInputFormat inputFormat, TextureInputType inputType, void* data)
                : width(width), height(height), formatInternal(formatInternal), type(type), textureSlot(textureSlot) {
        // gen texture
        GL_CALL(glGenTextures(1, &id));
        bind();
        GL_CALL(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter));
        GL_CALL(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter));

        // todo: maybe set border color
        // GL_CALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor));
        GL_CALL(glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS));
        GL_CALL(glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT));

        bufferData(inputFormat, inputType, data);
    }

    Texture::Texture(
            TextureType type, unsigned int textureSlot,
            int width, int height, int minFilter, int magFilter, int wrapS, int wrapT,
            TextureFormatInternal formatInternal)
                : Texture(type, textureSlot, width, height, minFilter, magFilter, wrapS, wrapT,
                  formatInternal, TextureInputFormat::RGBA, TextureInputType::FLOAT, nullptr) {}

    void Texture::bufferData(TextureInputFormat inputFormat, TextureInputType inputType, void* data) {
        //! todo: when adding other [TextureType]s we need to switch here!
        // initialize texture
        switch (type) {
            case TEXTURE_2D:
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, formatInternal, width, height, 0, inputFormat, inputType, data));
                break;
            default:
                LOG_CORE_WARN("Unkonwn texture type {}!", type);
        }
    }

    void Texture::bind() {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureSlot));
        GL_CALL(glBindTexture(type, id));
    }

    void Texture::bindImage(int slot, TextureAccess access) {
        CORE_ASSERT(isBound(), "Texture needs to be bound to bind image!");
        GL_CALL(glBindImageTexture(slot, id, 0, false, 0, access, formatInternal));
    }

    void Texture::bindUnit(int slot) {
        CORE_ASSERT(isBound(), "Texture needs to be bound to bind unit!");
        GL_CALL(glBindTextureUnit(slot, id));
    }

    bool Texture::isBound() {
        // todo: no clue if this is right (just winged it)
        int activeSlot;
        int binding;
        GL_CALL(glGetIntegerv(GL_ACTIVE_TEXTURE, &activeSlot));
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);

        return activeSlot == GL_TEXTURE0 + textureSlot && binding == id;
    }
}