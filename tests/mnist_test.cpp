#include <dataset/mnist.h>
#include <algorithm>
#ifdef __GNUC__
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <gtest/gtest.h>

#ifdef __GNUC__
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

std::string generateTmpFileName()
{
    std::string filename("0123456789ABCD");
    auto tmpdir = fs::temp_directory_path();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<char> distrib('a', 'z');
    do
    {
        std::generate(filename.begin(), filename.end(), [&distrib, &gen]() { return distrib(gen); });
    } while (fs::exists(tmpdir / filename));
    return tmpdir / filename;
}

class MnistTest : public ::testing::Test
{
public:
    MnistTest()
    {
        {
            std::ofstream mnistImages {mnistImagesFilename, std::ios::binary};
            std::int32_t mnistImagesMagicNumber = 0x00000803;
            mnistImages.write(reinterpret_cast<const char*>(&mnistImagesMagicNumber), sizeof mnistImagesMagicNumber);
            std::int32_t numImages = 2;
            mnistImages.write(reinterpret_cast<const char*>(&numImages), sizeof numImages);
            std::int32_t width = 2;
            mnistImages.write(reinterpret_cast<const char*>(&width), sizeof width);
            std::int32_t height = 2;
            mnistImages.write(reinterpret_cast<const char*>(&height), sizeof height);
            std::uint8_t dataChunk {};
            for (int i = 0; i < 2 * 28 * 28; i++)
            {
                mnistImages.write(reinterpret_cast<const char*>(&dataChunk), sizeof dataChunk);
                dataChunk++;
            }
        }
        {
            std::ofstream mnistLabels {mnistLabelsFilename, std::ios::binary};
            std::int32_t mnistLabelsMagicNumber = 0x00000801;
            mnistLabels.write(reinterpret_cast<const char*>(&mnistLabelsMagicNumber), sizeof mnistLabelsMagicNumber);
            std::int32_t numLabels = 2;
            mnistLabels.write(reinterpret_cast<const char*>(&numLabels), sizeof numLabels);
            for (std::uint8_t i = 0; i < 2; i++)
                mnistLabels.write(reinterpret_cast<const char*>(&i), sizeof i);
        }
        mnistImages_.open(mnistImagesFilename, std::ios::binary);
        mnistLabels_.open(mnistLabelsFilename, std::ios::binary);
    }

    void TearDown() override
    {
        fs::remove(mnistImagesFilename);
        fs::remove(mnistLabelsFilename);
    }

protected:
    std::string mnistImagesFilename { generateTmpFileName() };
    std::string mnistLabelsFilename { generateTmpFileName() };

    std::ifstream mnistImages_ {};
    std::ifstream mnistLabels_ {};
};


TEST_F(MnistTest, LoadImagesBasicTest) {
    yt::dataset::Mnist mnist {mnistImages_, mnistLabels_};
    auto imageBytes = mnist.loadImages(2);
    std::uint8_t dataChunk {};
    for (int i = 0; i < mnist.imageWidth() * mnist.imageHeight() * mnist.size(); i++)
        EXPECT_EQ(imageBytes[i], dataChunk++);
}


TEST_F(MnistTest, LoadLabelsBasicTest) {
    yt::dataset::Mnist mnist {mnistImages_, mnistLabels_};
    auto labels = mnist.loadLabels(2);
    unsigned char dataChunk {};
    for (int i = 0; i < mnist.size(); i++)
        EXPECT_EQ(labels[i], dataChunk++);
}

TEST_F(MnistTest, LoadMoreImagesThanAvailableTest) {
    yt::dataset::Mnist mnist {mnistImages_, mnistLabels_};
    EXPECT_THROW(mnist.loadImages(10), std::out_of_range);
}

TEST_F(MnistTest, LoadMoreLabelsThanAvailableTest) {
    yt::dataset::Mnist mnist {mnistImages_, mnistLabels_};
    EXPECT_THROW(mnist.loadLabels(10), std::out_of_range);
}
