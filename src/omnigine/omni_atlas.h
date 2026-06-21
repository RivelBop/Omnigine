#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <climits>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Omni
{
class Atlas
{
  public:
    Atlas()
    {
    }

    Atlas(const std::string &fileName, SDL_Renderer *renderer) : atlasFile(fileName)
    {
        // Open the atlas file
        std::ifstream f(fileName);

        // Check if the atlas file has successfully opened
        if (!f.is_open())
            throw std::runtime_error("Cannot open atlas file: " + fileName);
        std::string readData;

        // Get the atlas image
        std::getline(f, readData);
        size_t directory{ fileName.find_last_of('/') };
        if (directory != std::string::npos)
            readData.insert(0, fileName.substr(0, directory + 1));
        SDL_Surface *atlasImg{ IMG_Load(readData.c_str()) };
        if (!atlasImg) {
            f.close();
            throw std::runtime_error("Cannot create surface from atlas image.");
        }

        // Skip following data (size, repeat)
        f.ignore(LONG_MAX, '\n');
        if (!f.ignore(LONG_MAX, '\n')) {
            SDL_DestroySurface(atlasImg);
            f.close();
            throw std::runtime_error("Unable to parse atlas file, please ensure the atlas is formatted correctly.");
        }

        // Create textures for atlas regions
        Uint8 state{ 0 };
        std::string regionName;
        Uint32 index{ 0 };
        while (std::getline(f, readData)) {
            // Name
            if (state == 0) {
                state++;

                bool exitLoop{ true };
                for (char c : readData) {
                    if (!std::isspace(static_cast<unsigned char>(c))) {
                        exitLoop = false;
                        break;
                    }
                }

                if (exitLoop)
                    break;
                else {
                    regionName = readData;
                    continue;
                }
            }

            // Index
            if (state == 1) {
                state++;
                index = 0;

                if (readData.find("index:", 0) == 0) {
                    index = std::stoi(readData.substr(6));
                    continue;
                }
            }

            // Bounds
            if (state == 2) {
                if (readData.find("bounds:", 0) != 0)
                    break;

                // The index after "bounds:" is 7, but since x can be at least 1 char long, we can skip checking one character
                size_t xy{ readData.find(',', 8) };
                if (xy == std::string::npos)
                    break;

                // The index after the comma between x and y (comma between y and w), since y can be at least 1 char, skip checking it
                size_t yw{ readData.find(',', xy + 2) };
                if (yw == std::string::npos)
                    break;

                // The index after the comma between x and y (comma between y and w), since y can be at least 1 char, skip checking it
                size_t wh{ readData.find(',', yw + 2) };
                if (wh == std::string::npos)
                    break;

                // Extract texture region bounds from atlas
                SDL_Rect srcrect{
                    std::stoi(readData.substr(7, xy - 7)),           // x
                    std::stoi(readData.substr(xy + 1, yw - xy - 1)), // y
                    std::stoi(readData.substr(yw + 1, wh - yw - 1)), // w
                    std::stoi(readData.substr(wh + 1))               // h
                };

                // Retrieve (or automatically create) the vector of regions
                std::vector<SDL_Texture *> &indexedRegions = regionMap[regionName];

                // Ensure the located vector of indexed regions is the appropriate size
                if (indexedRegions.size() < index + 1)
                    indexedRegions.resize(index + 1);

                // Use the bounds to create a temporary surface to use to create the texture region
                SDL_Surface *tempSurface{ SDL_CreateSurface(srcrect.w, srcrect.h, atlasImg->format) };
                if (!tempSurface || !SDL_BlitSurface(atlasImg, &srcrect, tempSurface, nullptr)) {
                    if (tempSurface)
                        SDL_DestroySurface(tempSurface);
                    SDL_DestroySurface(atlasImg);
                    f.close();

                    free();
                    regionMap.clear();
                    throw std::runtime_error("Unable to create temporary surface for texture region generation.");
                }

                // Create and store a texture region from the surface
                SDL_Texture *textureRegion{ SDL_CreateTextureFromSurface(renderer, tempSurface) };
                if (!textureRegion) {
                    SDL_DestroySurface(tempSurface);
                    SDL_DestroySurface(atlasImg);
                    f.close();

                    free();
                    regionMap.clear();
                    throw std::runtime_error("Unable to create texture region from temporary surface.");
                }
                indexedRegions[index] = textureRegion;
                SDL_DestroySurface(tempSurface);

                state = 0; // Set the state back to starting state
            }
        }

        // Close atlas file
        SDL_DestroySurface(atlasImg);
        f.close();

        // Bounds must always be the last portion of the atlas (which should have set the state back to 0)
        if (state != 0) {
            free();
            regionMap.clear();
            throw std::runtime_error("Unable to parse atlas file, please ensure the atlas is formatted correctly.");
        }
    }

    Atlas(const Atlas &) = delete;

    Atlas(Atlas &&atlas) noexcept : atlasFile(std::move(atlas.atlasFile)), regionMap(std::move(atlas.regionMap))
    {
    }

    ~Atlas()
    {
        free();
    }

    Atlas &operator=(const Atlas &) = delete;

    Atlas &operator=(Atlas &&atlas) noexcept
    {
        if (this != &atlas) {
            // Dispose of this regionMap
            free();
            regionMap.clear();

            // Move the data from the other atlas to this
            atlasFile = std::move(atlas.atlasFile);
            regionMap = std::move(atlas.regionMap);
        }
        return *this;
    }

    /** Returns all texture regions (in indexed order) with the name provided. */
    std::vector<SDL_Texture *> &get(const std::string &regionName)
    {
        return regionMap.at(regionName);
    }

  private:
    std::string atlasFile;
    std::unordered_map<std::string, std::vector<SDL_Texture *>> regionMap;

    void free()
    {
        for (auto &r : regionMap) {
            for (SDL_Texture *t : r.second)
                SDL_DestroyTexture(t);
        }
    }
};
} // namespace Omni