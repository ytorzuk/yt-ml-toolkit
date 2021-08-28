#include "mnist.h"
#include <stdexcept>

namespace yt_ml_toolkit
{

Mnist::Mnist(std::istream &images, std::istream &labels) :
    images_ {images},
    labels_ {labels}
{
    std::int32_t imgsMagicNumber {};
    images_.read(reinterpret_cast<char*>(&imgsMagicNumber), sizeof imgsMagicNumber);
    if (imgsMagicNumber != kImagesMagicNumber)
        throw std::runtime_error("MNIST Read Error: Invalid magic number while reading images");
    images_.read(reinterpret_cast<char*>(&numItems_), sizeof numItems_);
    images_.read(reinterpret_cast<char*>(&width_), sizeof width_);
    images_.read(reinterpret_cast<char*>(&height_), sizeof height_);
    std::int32_t lblsMagicNumber {};
    std::int32_t numLabelsInDataset {};
    labels_.read(reinterpret_cast<char*>(&lblsMagicNumber), sizeof lblsMagicNumber);
    if (lblsMagicNumber != kLabelsMagicNumber)
        throw std::runtime_error("MNIST Read Error: Invalid magic number while reading labels");
    labels_.read(reinterpret_cast<char*>(&numLabelsInDataset), sizeof numLabelsInDataset);
    if (numLabelsInDataset != numItems_)
        throw std::out_of_range("MNIST Read Error: Number of images doesn't match number of labels");
}

std::vector<unsigned char> Mnist::loadImages(std::size_t numImages)
{
    if (numImages > numItems_)
        throw std::out_of_range("MNIST Read Error: Requested number of images exceeds number of images in the dataset");
    std::size_t numReadElements = numImages * width_ * height_;
    std::vector<unsigned char> result(numReadElements);
    images_.read(reinterpret_cast<char*>(result.data()), numReadElements * sizeof(unsigned char));
    if (!images_.good())
        throw std::runtime_error("I/O error accured during loading images from dataset");
    return result;
}

std::vector<unsigned char> Mnist::loadLabels(std::size_t numLabels)
{
    if (numLabels > numItems_)
        throw std::out_of_range("MNIST Read Error: Requested number of images exceeds number of labels in the dataset");
    std::vector<unsigned char> result(numLabels);
    labels_.read(reinterpret_cast<char*>(result.data()), numLabels * sizeof(unsigned char));
    if (!labels_.good())
        throw std::runtime_error("I/O error accured during loading labels from dataset");
    return result;
}

int Mnist::imageWidth() const
{
    return width_;
}

int Mnist::imageHeight() const
{
    return height_;
}

int Mnist::size() const
{
    return numItems_;
}

} // yt_ml_toolkit
