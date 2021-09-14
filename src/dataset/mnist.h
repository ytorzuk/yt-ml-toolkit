#pragma once

#include <iostream>
#include <vector>

namespace yt_ml_toolkit {
namespace dataset {

class Mnist
{
public:
    Mnist(std::istream& images, std::istream& labels);
    std::vector<unsigned char> loadImages(std::size_t numImages);
    std::vector<unsigned char> loadLabels(std::size_t numImages);
    int imageWidth() const;
    int imageHeight() const;
    int size() const;

private:
    static constexpr int kImagesMagicNumber = 0x00000803;
    static constexpr int kLabelsMagicNumber = 0x00000801;

    std::istream &images_;
    std::istream &labels_;
    std::int32_t numItems_ {};
    std::int32_t width_ {};
    std::int32_t height_ {};
};

} // dataset
} // yt_ml_toolkit
