#pragma once

#include <list>
#include <queue>
#include <string>
#include <unordered_map>

#include "raylib.h"

namespace Omni
{
class Assets
{
  public:
    /* ==================== LOADING ==================== */

    void loadImage(const std::string &fileName)
    {
        load(fileName, IMAGE);
    }

    void loadTexture(const std::string &fileName)
    {
        load(fileName, TEXTURE);
    }

    void loadFont(const std::string &fileName)
    {
        load(fileName, FONT);
    }

    void loadModel(const std::string &fileName)
    {
        load(fileName, MODEL);
    }

    void loadWave(const std::string &fileName)
    {
        load(fileName, WAVE);
    }

    void loadSound(const std::string &fileName)
    {
        load(fileName, SOUND);
    }

    void loadMusic(const std::string &fileName)
    {
        load(fileName, MUSIC);
    }

    /* ==================== INITIALIZING AND UPDATING ==================== */

    // Get the first queued asset and initialize it.
    // Returns true if all queued assets have been initialized.
    bool update()
    {
        // Initial size check to ensure the assets aren't already done loading
        size_t size = loadQueue.size();
        if (size == 0)
            return true;

        // Retrieve the first queued asset and get its type
        std::string &asset = loadQueue.front();
        const Type &t = typeMap.at(asset);

        // Get C String representation of asset file
        const char *a = asset.c_str();

        // Load each asset into their appropriate map
        if (t == IMAGE)
            imageMap.insert({ asset, LoadImage(a) });
        else if (t == TEXTURE)
            textureMap.insert({ asset, LoadTexture(a) });
        else if (t == FONT)
            fontMap.insert({ asset, LoadFont(a) });
        else if (t == MODEL)
            modelMap.insert({ asset, LoadModel(a) });
        else if (t == WAVE)
            waveMap.insert({ asset, LoadWave(a) });
        else if (t == SOUND)
            soundMap.insert({ asset, LoadSound(a) });
        else if (t == MUSIC)
            musicMap.insert({ asset, LoadMusicStream(a) });

        // Remove the loaded asset from the queue
        loadQueue.pop();

        // Check if loading is complete
        // If there was one asset in the queue, it was loaded so empty queue
        return size == 1;
    }

    // Initialize queued assets for the given time (in seconds).
    // NOTE: The timing is NOT accurate, some assets take longer to load than others!
    // Returns true if all queued assets have been initialized.
    bool update(double time)
    {
        double start = GetTime();
        while (GetTime() - start < time) {
            if (update())
                return true;
        }
        return false;
    }

    // Initialize ALL queued assets for loading
    void finishLoading()
    {
        while (!update())
            ;
    }

    /* ==================== GET LOADED AND INITIALIZED ASSETS ==================== */

    Image &getImage(const std::string &fileName)
    {
        return imageMap.at(fileName);
    }

    Texture2D &getTexture(const std::string &fileName)
    {
        return textureMap.at(fileName);
    }

    Font &getFont(const std::string &fileName)
    {
        return fontMap.at(fileName);
    }

    Model &getModel(const std::string &fileName)
    {
        return modelMap.at(fileName);
    }

    Wave &getWave(const std::string &fileName)
    {
        return waveMap.at(fileName);
    }

    Sound &getSound(const std::string &fileName)
    {
        return soundMap.at(fileName);
    }

    Music &getMusic(const std::string &fileName)
    {
        return musicMap.at(fileName);
    }

    /* ==================== UNLOAD ASSETS ==================== */

    // Unloads and removes the specified asset, does not remove the asset from the load queue
    void unload(const std::string &fileName)
    {
        // Ensure the load() method was called on the asset
        auto typeIterator = typeMap.find(fileName);
        if (typeIterator == typeMap.end()) {
            return;
        }

        // Retrieve the asset's type
        const Type &t = typeIterator->second;

        // Unload each asset from their appropriate map
        // 1. Find the code block with the correct asset type via the switch-case.
        // 2. Ensure the asset is within that type's map (not in queue).
        // 3. Unload the asset.
        // 4. Remove the asset from the type's map.
        switch (t) {
        case IMAGE:
        {
            auto asset = imageMap.find(fileName);
            if (asset != imageMap.end()) {
                UnloadImage(asset->second);
                imageMap.erase(asset);
            }
            break;
        }
        case TEXTURE:
        {
            auto asset = textureMap.find(fileName);
            if (asset != textureMap.end()) {
                UnloadTexture(asset->second);
                textureMap.erase(asset);
            }
            break;
        }
        case FONT:
        {
            auto asset = fontMap.find(fileName);
            if (asset != fontMap.end()) {
                UnloadFont(asset->second);
                fontMap.erase(asset);
            }
            break;
        }
        case MODEL:
        {
            auto asset = modelMap.find(fileName);
            if (asset != modelMap.end()) {
                UnloadModel(asset->second);
                modelMap.erase(asset);
            }
            break;
        }
        case WAVE:
        {
            auto asset = waveMap.find(fileName);
            if (asset != waveMap.end()) {
                UnloadWave(asset->second);
                waveMap.erase(asset);
            }
            break;
        }
        case SOUND:
        {
            auto asset = soundMap.find(fileName);
            if (asset != soundMap.end()) {
                UnloadSound(asset->second);
                soundMap.erase(asset);
            }
            break;
        }
        case MUSIC:
        {
            auto asset = musicMap.find(fileName);
            if (asset != musicMap.end()) {
                UnloadMusicStream(asset->second);
                musicMap.erase(asset);
            }
            break;
        }
        }

        // Remove the asset from the type map
        typeMap.erase(typeIterator);
    }

    // Unloads and removes all assets, does not remove assets from the load queue
    void unloadAll()
    {
        while (!typeMap.empty())
            unload(typeMap.begin()->first);
    }

    // Unload all assets
    ~Assets()
    {
        unloadAll();
    }

  private:
    /* ==================== ASSET TYPES ==================== */

    enum Type : unsigned char
    {
        // TODO: Load shaders, materials, model animations
        IMAGE,
        TEXTURE,
        FONT,
        MODEL,
        WAVE,
        SOUND,
        MUSIC
    };

    /* ==================== ASSET MAPS ==================== */

    std::unordered_map<std::string, Image> imageMap;
    std::unordered_map<std::string, Texture2D> textureMap;
    std::unordered_map<std::string, Font> fontMap;
    std::unordered_map<std::string, Model> modelMap;
    std::unordered_map<std::string, Wave> waveMap;
    std::unordered_map<std::string, Sound> soundMap;
    std::unordered_map<std::string, Music> musicMap;

    /* ==================== LOADING ==================== */

    // Asset file names queued to load, linked-list for frequent asset insertions and removals
    std::queue<std::string, std::list<std::string>> loadQueue;

    // Keeps track of each asset's type
    std::unordered_map<std::string, Type> typeMap;

    // Queues the asset file for loading and stores its type
    void load(const std::string &fileName, Type assetType)
    {
        loadQueue.push(fileName);
        typeMap.insert({ fileName, assetType });
    }
};
} // namespace Omni