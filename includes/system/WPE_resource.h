#pragma once
#ifndef WPE_RESOURCE_H
#define WPE_RESOURCE_H
#include <system/WPE_base.h>
#include <system/WPE_path.h>

WPE_NAMESPACE_BEGIN
namespace Resource {
    enum ImageChannel {
        IMAGE_CHANNEL_GREY,
        IMAGE_CHANNEL_GREY_ALPHA,
        IMAGE_CHANNEL_RGB,
        IMAGE_CHANNEL_RGBA
    };

    struct Image {
        int width;
        int height;
        ImageChannel channel;
        unsigned char* data;
    };

    bool LoadImage(const std::string& image, Image& image_data); 
}
WPE_NAMESPACE_END
#endif