

// bool naiveImgCmp(Image img1, Image img2)
// {
//     if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight())
//         return false;

//     if (img1.getWidth() * img1.getWidth() > INT_MAX)
//         return false;

//     int countSame = 0;
//     int imgSize = img1.getWidth() * img1.getHeight();

//     for (int h = 0; h < img1.getHeight(); h++)
//         for (int w = 0; w < img1.getWidth(); w++)
//             if (img1.getPixel(w, h) == img2.getPixel(w, h))
//                 countSame++;

//     return countSame >= imgSize * 0.95;
// }

// bool hashImgCmp(Image img1, Image img2)
// {
//     auto hash1 = phash(img1, 8, 4);
//     auto hash2 = phash(img2, 8, 4);

//     for (auto val : hash1)
//     {
//         if (val)
//             std::cout << "1";
//         else
//             std::cout << "0";
//     }
//     std::cout << '\n';

//     for (auto val : hash2)
//     {
//         if (val)
//             std::cout << "1";
//         else
//             std::cout << "0";
//     }
//     std::cout << '\n';

//     return hash1 == hash2;
// }
