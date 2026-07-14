#pragma once

#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Atlas.h"
#include "Audio.h"
#include "Render.h"

namespace Omni
{
class Assets
{
  public:
    /* ==================== LOADING ==================== */

    void LoadSurface(const std::string &fileName)
    {
        Load(fileName, SURFACE);
    }

    void LoadTexture(const std::string &fileName)
    {
        Load(fileName, TEXTURE);
    }

    void LoadAtlas(const std::string &fileName)
    {
        Load(fileName, ATLAS);
    }

    void LoadFont(const std::string &fileName)
    {
        Load(fileName, FONT);
    }

    void LoadSound(const std::string &fileName)
    {
        Load(fileName, SOUND);
    }

    void LoadMusic(const std::string &fileName)
    {
        Load(fileName, MUSIC);
    }

    /* ==================== INITIALIZING AND UPDATING ==================== */

    /**
     * Get the first queued asset and initialize it.
     * Returns true if all queued assets have been initialized.
     */
    [[nodiscard]] bool Update()
    {
        // Initial size check to ensure the assets aren't already done loading
        size_t size{ loadQueue.size() };
        if (size == 0)
            return true;

        // Retrieve the first queued asset and get its type
        std::string asset{ loadQueue.front() };
        Type t{ typeMap.at(asset) };

        // Get C String representation of asset file
        const char *a{ asset.c_str() };

        // Load each asset into their appropriate map
        bool throwException{ false };
        if (t == SURFACE) {
            SDL_Surface *surface{ IMG_Load(a) };
            if (surface)
                surfaceMap.emplace(asset, surface);
            throwException = !surface;
        } else if (t == TEXTURE) {
            SDL_Texture *texture{ IMG_LoadTexture(Renderer(), a) };
            if (texture)
                textureMap.emplace(asset, texture);
            throwException = !texture;
        } else if (t == ATLAS) {
            try {
                atlasMap.emplace(asset, Atlas(asset));
            } catch (std::runtime_error e) {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
                throwException = true;
            }
        } else if (t == FONT) {
            TTF_Font *font{ TTF_OpenFont(a, 16.0f) };
            if (font)
                fontMap.emplace(asset, font);
            throwException = !font;
        } else if (t == SOUND) {
            auto sound{ std::make_unique<ma_sound>() };
            ma_result result{ ma_sound_init_from_file(SoundEngine(), a, 0, nullptr, nullptr, sound.get()) };
            if (result == MA_SUCCESS)
                soundMap.emplace(asset, std::move(sound));
            throwException = result != MA_SUCCESS;
        } else if (t == MUSIC) {
            auto music{ std::make_unique<ma_sound>() };
            ma_result result{ ma_sound_init_from_file(SoundEngine(), a, MA_SOUND_FLAG_STREAM, nullptr, nullptr, music.get()) };
            if (result == MA_SUCCESS)
                musicMap.emplace(asset, std::move(music));
            throwException = result != MA_SUCCESS;
        }

        // Remove the loaded asset from the queue
        loadQueue.pop();

        if (throwException)
            throw std::runtime_error("Failed to load asset: " + asset);

        // Check if loading is complete
        // If there was one asset in the queue, it was loaded so empty queue
        return size == 1;
    }

    /**
     * Initialize queued assets for the given time (in milliseconds).
     * NOTE: The timing is NOT accurate, some assets take longer to load than others!
     * Returns true if all queued assets have been initialized.
     */
    [[nodiscard]] bool Update(Uint64 time)
    {
        Uint64 start{ SDL_GetTicks() };
        while (SDL_GetTicks() - start < time) {
            if (Update())
                return true;
        }
        return false;
    }

    /** Initialize ALL queued assets for loading. */
    void FinishLoading()
    {
        while (!Update())
            ;
    }

    /* ==================== GET LOADED AND INITIALIZED ASSETS ==================== */

    /** Returns the same (shared) surface, best to clone if editing!  */
    [[nodiscard]] SDL_Surface *GetSurface(const std::string &fileName) const
    {
        return surfaceMap.at(fileName);
    }

    [[nodiscard]] SDL_Texture *GetTexture(const std::string &fileName) const
    {
        return textureMap.at(fileName);
    }

    [[nodiscard]] const Atlas &GetAtlas(const std::string &fileName) const
    {
        return atlasMap.at(fileName);
    }

    /** Returns the specified font with a default size of 16.0f. Clone or resize if necessary! */
    [[nodiscard]] TTF_Font *GetFont(const std::string &fileName) const
    {
        return fontMap.at(fileName);
    }

    /** Returns the same (shared) loaded sound instance, clone to play! */
    [[nodiscard]] ma_sound *GetSound(const std::string &fileName) const
    {
        return soundMap.at(fileName).get();
    }

    /** Returns the same (shared) loaded streamed sound instance, clone to play! */
    [[nodiscard]] ma_sound *GetMusic(const std::string &fileName) const
    {
        return musicMap.at(fileName).get();
    }

    /* ==================== UNLOAD ASSETS ==================== */

    /** Destroys and removes the specified asset, does not remove the asset from the load queue. */
    void Unload(const std::string &fileName)
    {
        // Ensure the load() method was called on the asset
        auto typeIterator{ typeMap.find(fileName) };
        if (typeIterator == typeMap.end()) {
            return;
        }

        // Retrieve the asset's type
        Type t{ typeIterator->second };

        // Unload each asset from their appropriate map
        // 1. Find the code block with the correct asset type via the switch-case.
        // 2. Ensure the asset is within that type's map (not in queue).
        // 3. Unload the asset.
        // 4. Remove the asset from the type's map.
        switch (t) {
        case SURFACE:
        {
            auto asset{ surfaceMap.find(fileName) };
            if (asset != surfaceMap.end()) {
                SDL_DestroySurface(asset->second);
                surfaceMap.erase(asset);
            }
            break;
        }
        case TEXTURE:
        {
            auto asset{ textureMap.find(fileName) };
            if (asset != textureMap.end()) {
                SDL_DestroyTexture(asset->second);
                textureMap.erase(asset);
            }
            break;
        }
        case ATLAS:
        {
            auto asset{ atlasMap.find(fileName) };
            if (asset != atlasMap.end()) {
                atlasMap.erase(asset);
            }
            break;
        }
        case FONT:
        {
            auto asset{ fontMap.find(fileName) };
            if (asset != fontMap.end()) {
                TTF_CloseFont(asset->second);
                fontMap.erase(asset);
            }
            break;
        }
        case SOUND:
        {
            auto asset{ soundMap.find(fileName) };
            if (asset != soundMap.end()) {
                ma_sound_uninit(asset->second.get());
                soundMap.erase(asset);
            }
            break;
        }
        case MUSIC:
        {
            auto asset{ musicMap.find(fileName) };
            if (asset != musicMap.end()) {
                ma_sound_uninit(asset->second.get());
                musicMap.erase(asset);
            }
            break;
        }
        }

        // Remove the asset from the type map
        typeMap.erase(typeIterator);
    }

    /** Unloads and removes all assets (including from the load queue). */
    void UnloadAll()
    {
        while (!loadQueue.empty()) {
            std::string asset{ loadQueue.front() };
            typeMap.erase(asset);
            loadQueue.pop();
        }

        while (!typeMap.empty()) {
            // Copy file name to avoid dangling reference when passed into unload()
            std::string copyFileName{ typeMap.begin()->first };
            Unload(copyFileName);
        }
    }

    Assets() = default;

    Assets(const Assets &) = delete;

    Assets(Assets &&) noexcept = delete;

    Assets &operator=(const Assets &) = delete;

    Assets &operator=(Assets &&) noexcept = delete;

    /** Unload all assets. */
    ~Assets()
    {
        UnloadAll();
    }

  private:
    /* ==================== ASSET TYPES ==================== */

    enum Type : unsigned char
    {
        SURFACE,
        TEXTURE,
        ATLAS,
        FONT,
        SOUND,
        MUSIC
    };

    /* ==================== ASSET MAPS ==================== */

    std::unordered_map<std::string, SDL_Surface *> surfaceMap;
    std::unordered_map<std::string, SDL_Texture *> textureMap;
    std::unordered_map<std::string, Atlas> atlasMap;
    std::unordered_map<std::string, TTF_Font *> fontMap;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> soundMap;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> musicMap;

    /* ==================== LOADING ==================== */

    /** Asset file names queued to load. */
    std::queue<std::string> loadQueue;

    /** Keeps track of each asset's type. */
    std::unordered_map<std::string, Type> typeMap;

    /** Queues the asset file for loading and stores its type. */
    void Load(const std::string &fileName, Type assetType)
    {
        // Ensure this asset hasn't called load() before
        if (typeMap.find(fileName) == typeMap.end()) {
            loadQueue.push(fileName);
            typeMap.insert({ fileName, assetType });
        }
    }
};
} // namespace Omni
