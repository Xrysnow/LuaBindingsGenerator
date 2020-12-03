#pragma once

#include "base/CCRef.h"
#include <cstdint>
#include <string>

namespace lstg{
	class Buffer;
};

NS_CC_BEGIN

class Image
 : public Ref
{
public:
    /**
    @brief Load image from stream buffer.
    @param data  stream buffer which holds the image data.
    @param dataLen  data length expressed in (number of) bytes.
    @return true if loaded correctly.
    */
    bool initWithImageData(lstg::Buffer* b);

    /** @warning only support RGBA8888 */
    bool initWithRawData(lstg::Buffer* b, int width, int height, int bitsPerComponent, bool preMulti = false);

    bool initWithSVGFile(const std::string& path, float scale = 1, const std::string& units = "px", float dpi = 96);
};

class Texture2D : public Ref
{
public:
    /**
     * @warning only support RGBA8888
     */
    Image* newImage(size_t x, size_t y, size_t width, size_t height, bool flipImage = false);
    Image* newImage(bool flipImage = false);
};

class Sprite : public Ref
{
public:
    /**
     * @warning only support RGBA8888
     */
    Image* newImage(bool flipImage = false);
};

NS_CC_END
