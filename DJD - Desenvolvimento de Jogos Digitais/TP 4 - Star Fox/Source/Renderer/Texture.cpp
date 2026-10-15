#include "Texture.h"

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
}

bool Texture::Load(const std::string &filePath)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image %s: %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    // Convert surface to a known 32-bit RGBA format to be safe for GL upload
    SDL_Surface* uploadSurface = surface;
    if (surface->format->BytesPerPixel != 4 || surface->format->format != SDL_PIXELFORMAT_RGBA32) {
        SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        if (converted) {
            uploadSurface = converted;
            // free original surface if conversion created a new one
            SDL_FreeSurface(surface);
        } else {
            SDL_FreeSurface(surface);
            return false;
        }
    }

    mWidth = uploadSurface->w;
    mHeight = uploadSurface->h;

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    // Set row length based on pitch and 4 bytes per pixel
    glPixelStorei(GL_UNPACK_ROW_LENGTH, uploadSurface->pitch / 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, uploadSurface->pixels);
    GLenum err = glGetError();

    // Delete the SDL surface we used for upload (if we converted, uploadSurface != surface)
    SDL_FreeSurface(uploadSurface);

    // Generate mipmaps for texture
    glGenerateMipmap(GL_TEXTURE_2D);
    err = glGetError();

    // Enable linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
    mWidth = surface->w;
    mHeight = surface->h;

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch / 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 surface->pixels);

    // Use linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(int index) const
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
