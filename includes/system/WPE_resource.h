#pragma once
#ifndef WPE_RESOURCE_H
#define WPE_RESOURCE_H
#include <system/WPE_base.h>
#include <system/WPE_path.h>

WPE_NAMESPACE_BEGIN
namespace Resource {
    
/*************************** */
// Image
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

    using ImagePtr = std::shared_ptr<const Image>;

    class ResourceCache {
    public:
        static ResourceCache& GetInstance();

        ImagePtr LoadImage(const std::string& key, const WPE::Path::Path& filename);
        ImagePtr LoadImage(const std::string& key);
        
    private:
        std::unordered_map<std::string, Image> images;
    };
}
WPE_NAMESPACE_END
#endif