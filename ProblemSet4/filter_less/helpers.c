#include "helpers.h"
#include <math.h>
#include <stdlib.h>

#define CAP(colorValue)   ((colorValue) > 255.0 ? 255.0 : (colorValue))

// Function prototypes
RGBTRIPLE blurPixel(int x, int y, int height, int width, RGBTRIPLE image[height][width]);


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    //TODO
    const double oneThird = 1.0 / 3;
    RGBTRIPLE pixel;
    BYTE averageLevel;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            pixel = image[y][x];
            averageLevel = (BYTE) round(oneThird * (pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed));
            pixel.rgbtBlue = averageLevel;
            pixel.rgbtGreen = averageLevel;
            pixel.rgbtRed = averageLevel;
            image[y][x] = pixel;
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    //TODO
    BYTE originalRed, originalGreen, originalBlue;
    BYTE sepiaRed, sepiaGreen, sepiaBlue;
    float fSepiaBlue, fSepiaGreen, fSepiaRed;
    RGBTRIPLE pixel;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            pixel = image[y][x];
            originalBlue = pixel.rgbtBlue;
            originalGreen = pixel.rgbtGreen;
            originalRed = pixel.rgbtRed;

            fSepiaRed = CAP(.393 * originalRed + .769 * originalGreen + .189 * originalBlue);
            fSepiaGreen = CAP(.349 * originalRed + .686 * originalGreen + .168 * originalBlue);
            fSepiaBlue = CAP(.272 * originalRed + .534 * originalGreen + .131 * originalBlue);

            sepiaRed = (BYTE) round(fSepiaRed);
            sepiaGreen = (BYTE) round(fSepiaGreen);
            sepiaBlue = (BYTE) round(fSepiaBlue);

            pixel.rgbtRed = sepiaRed;
            pixel.rgbtGreen = sepiaGreen;
            pixel.rgbtBlue = sepiaBlue;

            image[y][x] = pixel;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    //TODO
    RGBTRIPLE tmpPixel;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0, semiWidth = width / 2; x < semiWidth; ++x)
        {
            tmpPixel = image[y][width - 1 - x];
            image[y][width - 1 - x] = image[y][x];
            image[y][x] = tmpPixel;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    //TODO
    RGBTRIPLE *tmpImage = (RGBTRIPLE *) malloc(height * width * sizeof(RGBTRIPLE));
    // RGBTRIPLE pixel, blurredPixel;
    RGBTRIPLE blurredPixel;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // pixel = image[y][x];
            blurredPixel = blurPixel(x, y, height, width, image);
            tmpImage[width * y + x] = blurredPixel;
        }
    }

    int counter = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            image[y][x] = tmpImage[counter++];
        }
    }
    free(tmpImage);
    return;
}

// Blur pixel
RGBTRIPLE blurPixel(int x, int y, int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE pixel, blurredPixel;
    int red = 0, green = 0, blue = 0;
    int newX, newY;
    int counter = 0;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            newX = x + dx;
            newY = y + dy;
            if (0 <= newX && newX < width && 0 <= newY && newY < height)
            {
                pixel = image[y + dy][x + dx];
                blue += pixel.rgbtBlue;
                green += pixel.rgbtGreen;
                red += pixel.rgbtRed;
                counter += 1;
            }
        }
    }

    blurredPixel.rgbtRed = (BYTE) round(1.0 * red / counter);
    blurredPixel.rgbtGreen = (BYTE) round(1.0 * green / counter);
    blurredPixel.rgbtBlue = (BYTE) round(1.0 * blue / counter);

    return blurredPixel;
}