#include "helpers.h"
#include <math.h>
#include <stdlib.h>

#define CAP(colorValue) ((colorValue) > 255.0 ? 255.0 : (colorValue))
#define DIAG(x, y)      sqrt((x) * (x) + (y) *(y))

// Function prototypes
RGBTRIPLE blurPixel(int x, int y, int height, int width, RGBTRIPLE image[height][width]);
RGBTRIPLE sobelPixel(int x, int y, int height, int width, RGBTRIPLE image[height][width]);


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
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

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
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

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE *tmpImage = (RGBTRIPLE *) malloc(height * width * sizeof(RGBTRIPLE));
    RGBTRIPLE filteredPixel;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            filteredPixel = sobelPixel(x, y, height, width, image);
            tmpImage[width * y + x] = filteredPixel;
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

// Sobel pixel
RGBTRIPLE sobelPixel(int x, int y, int height, int width, RGBTRIPLE image[height][width])
{
    int GxKernel[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    int GyKernel[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

    RGBTRIPLE *grid = (RGBTRIPLE *) malloc(9 * sizeof(RGBTRIPLE));
    RGBTRIPLE pixel, sobelledPixel;
    int newX, newY;
    int counter = 0;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            newX = x + dx;
            newY = y + dy;
            if (newX < 0 || newY < 0 || newX == width || newY == height)
            {
                pixel = (RGBTRIPLE)
                {
                    0, 0, 0
                };
            }
            else
            {
                pixel = image[y + dy][x + dx];
            }
            grid[counter++] = pixel;
        }
    }

    int redGX = 0, redGY = 0;
    int greenGX = 0, greenGY = 0;
    int blueGX = 0, blueGY = 0;
    RGBTRIPLE nextPoint;
    for (int i = 0; i < 9; ++i)
    {
        nextPoint = grid[i];
        redGX += GxKernel[i] * nextPoint.rgbtRed;
        greenGX += GxKernel[i] * nextPoint.rgbtGreen;
        blueGX += GxKernel[i] * nextPoint.rgbtBlue;
        redGY += GyKernel[i] * nextPoint.rgbtRed;
        greenGY += GyKernel[i] * nextPoint.rgbtGreen;
        blueGY += GyKernel[i] * nextPoint.rgbtBlue;
    }
    free(grid);

    sobelledPixel.rgbtRed = (BYTE) round(CAP(DIAG(redGX, redGY)));
    sobelledPixel.rgbtGreen = (BYTE) round(CAP(DIAG(greenGX, greenGY)));
    sobelledPixel.rgbtBlue = (BYTE) round(CAP(DIAG(blueGX, blueGY)));

    return sobelledPixel;
}