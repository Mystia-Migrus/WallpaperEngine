#include <system/WPE_resource.h>
#include <image/stb_image.h>

using namespace WPE_NAMESPACE::Resource;
bool _LoadImage(const std::string& image, WPE::Resource::Image& image_data) {
    int channel;

    image_data.data = stbi_load(image.c_str(), &image_data.width, &image_data.height, &channel, 0);
    if(image_data.data == nullptr) 
        return false;

    switch(channel) {
    case 1: 
        image_data.channel = IMAGE_CHANNEL_GREY;
        break;
    case 2: 
        image_data.channel = IMAGE_CHANNEL_GREY_ALPHA; 
        break;
    case 3:
        image_data.channel = IMAGE_CHANNEL_RGB;
        break;
    case 4:
        image_data.channel = IMAGE_CHANNEL_RGBA;
        break;
    }
    
    return true;
}

ResourceCache& ResourceCache::GetInstance() {
    static ResourceCache cacher;
    return cacher;
}

ImagePtr ResourceCache::LoadImage(const std::string& key, const WPE::Path::Path& filename) {
    Image image;
    if(_LoadImage(filename.Native(), image)) {
        return nullptr;
    }

    auto& result = this->images.insert(std::pair<std::string, Image>(key, image));
    if(result.second){
        return std::make_shared<Image>(result.first->second);
    }

    return nullptr;
}

ImagePtr ResourceCache::LoadImage(const std::string& key) {
    auto& result = this->images.find(key);
    if(result == this->images.end())
        return nullptr;
    return std::make_shared<Image>(result->second);
}