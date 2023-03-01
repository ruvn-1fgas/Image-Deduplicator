#include "imageLib/image.cpp"

bool naiveImgCmp(Image img1, Image img2)
{
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight())
        return false;

    if (img1.getWidth() * img1.getWidth() > INT_MAX)
        return false;

    int countSame = 0;
    int imgSize = img1.getWidth() * img1.getHeight();

    for (int h = 0; h < img1.getHeight(); h++)
        for (int w = 0; w < img1.getWidth(); w++)
            if (img1.getPixel(w, h) == img2.getPixel(w, h))
                countSame++;

    return countSame >= imgSize * 0.95;
}