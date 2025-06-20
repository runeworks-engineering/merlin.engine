#include "pch.h"
#include "merlin/utils/textureLoader.h"
#include "merlin/utils/util.h"

namespace Merlin {
	using namespace Utils;
    ImageData TextureLoader::loadImageData(const std::string& filepath, bool flipped) {
        FileType ft = getFileType(filepath);
        ImageData data;
        data.bytes = nullptr;
        stbi_set_flip_vertically_on_load(flipped);
        switch (ft) {
        case FileType::HDR:
            parseHDR(filepath, data);
            break;
        case FileType::PNG:
        case FileType::JPG:
            parsePNG_JPG(filepath, data);
            break;
        default:
            break;
        }

        if (data.bytes == nullptr) {
            Console::error("TextureLoader") << "could not load texture " << filepath << Console::endl;
            Console::error("TextureLoader") << " stb error : ";
            if (stbi_failure_reason()) Console::write(stbi_failure_reason());
            Console::write(Console::endl);

            //throw std::runtime_error("cannot load image");
            stbi_image_free(data.bytes);
            return data;
        }

        return data;
    }


    shared<Texture2D> TextureLoader::loadTexture(const std::string& filepath, TextureType t) {
        return Texture2D::create(filepath, t);
    }

    void Merlin::TextureLoader::parseHDR(const std::string& filepath, ImageData& data) {
        data.bytes = stbi_loadf(filepath.c_str(), &data.width, &data.height, &data.channels, 0);
        data.bits = 32; //32bits per channel
        data.dataType = GL_FLOAT; // Specify that the data type is float

    }

    void Merlin::TextureLoader::parsePNG_JPG(const std::string& filepath, ImageData& data) {
        data.bytes = stbi_load(filepath.c_str(), &data.width, &data.height, &data.channels, 0);
        data.dataType = GL_UNSIGNED_BYTE; // Data is stored as unsigned bytes
        data.bits = 8;
    }

}