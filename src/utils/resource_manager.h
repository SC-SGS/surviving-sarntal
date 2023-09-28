#pragma once

#include "flecs.h"
#include "raylib.h"
#include "string"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace graphics {

typedef size_t HANDLE;
constexpr HANDLE NULL_HANDLE = -1;

constexpr std::hash<std::string> hasher{};

template <typename T> class ResourceManager {
  public:
    /*
     * Loads the resource from disc into memory and sets it up for use.
     * @return HANDLE to the resource.
     * Use Get to retrieve the resource.
     */
    HANDLE load(const std::string &path) {
        // Create a unique hash for this resource
        auto hash = hasher(path);

        // check if resource was already loaded before
        auto it = res.find(hash);

        if (it != res.end()) {
            // found
            return hash;
        } else {
            // not found, create new

            res.insert({hash, LoadTexture(path.c_str())});
            return hash;
        }
    }

    /*
     * Inserts the resource into the manager
     * see load(const std::string &path) above
     */
    HANDLE load(const T &resource) {
        HANDLE handle = 0;
        // generate unused handle
        do {
            handle = rand();
        } while (res.find(handle) != res.end());

        res.insert({handle, resource});
        return handle;
    }

    /**
     * Retrieve the resource behind the handle.
     * Do not store the returned resource in your own variables.
     * Instead always use the handle with this get method anytime you access
     * this resource
     * @param resource assigned to this handle
     */
    T get(HANDLE handle) const {
        auto it = res.find(handle);
        if (it != res.end()) {
            return it->second;
        } else {
            throw std::runtime_error("invalid handle!");
        }
    }

    /**
     * Free the resource behind this handle.
     * After calling this the handle is invaild and is not pointing to a
     * resource anymore This means for any other entity the resource is unloaded
     * too! Only call this once this resource will not in use for the current
     * game state
     * @param handle
     */
    void free(HANDLE handle) {
        auto it = res.find(handle);
        if (it != res.end()) {
            onFree(it->second);
            res.erase(it);
        }
    }

    /**
     * frees all resources
     */
    ~ResourceManager() {
        for (auto &a : res) {
            onFree(a.second);
        }
        res.clear();
    }

  protected:
    friend struct Resources;
    void setOnFreeCallback(std::function<void(T)> callback) {
        onFree = callback;
    }

  private:
    std::unordered_map<HANDLE, T> res;
    std::function<void(T &)> onFree;
};

struct Resources {
    ResourceManager<Texture2D> textures;
    ResourceManager<Music> music;

    Resources() {

        textures.setOnFreeCallback(UnloadTexture);

        music.setOnFreeCallback(UnloadMusicStream);
    }
};

} // namespace graphics
