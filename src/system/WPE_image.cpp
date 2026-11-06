#include <system/WPE_resource.h>
#include <image/stb_image.h>

bool WPE::Resource::LoadImage(const std::string& image, WPE::Resource::Image& image_data) {
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
