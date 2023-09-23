#pragma once

#include "flecs.h"
#include "raylib.h"
#include "string"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace graphics {
typedef int HANDLE;
constexpr HANDLE NULL_HANDLE = -1;

template <typename T> class ResourceManager {
  public:
      //delete copy and assignment constructor
    //ResourceManager(const ResourceManager &) = delete;

    ResourceManager() { std::cout << "-------yo" << std::endl;
    }

    HANDLE Load(const std::string &path) {
        // Create a unique hash for this resource
        std::hash<std::string> hasher;
        HANDLE hash = hasher(path);

        // check if resource was already loaded before
        auto it = res.find(hash);

        if (it != res.end()) {
            // found
            return hash;
        } else {
            // not found, create new
            return Load(LoadTexture(path.c_str()));
        }
    }

    HANDLE Load(const T &resource) {
        HANDLE handle = 0;
        // generate unused handle
        do {
            handle = rand();
        } while (res.find(handle) != res.end());

        res.insert({handle, resource});
        return handle;
    }

    T Get(HANDLE handle) const {
        auto it = res.find(handle);
        if (it != res.end()) {
            return it->second;
        } else {
            throw std::runtime_error("invalid handle!");
        }
    }

    void Free(HANDLE handle) {
        auto it = res.find(handle);
        if (it != res.end()) {
            onFree(it->second);
            res.erase(it);
        }
    }

    void setOnFreeCallback(std::function<void(T)> callback) {
        onFree = callback;
    }

    ~ResourceManager() {
        for (auto &a : res) {
            onFree(a.second);
        }
        res.clear();
    }

  private:
    std::unordered_map<HANDLE, T> res;
    std::function<void(T &)> onFree;
};

struct Resources {
    ResourceManager<Texture2D> textures;
    ResourceManager<Music> music;

    Resources() {

        textures.setOnFreeCallback(
            [](Texture2D &texture) { UnloadTexture(texture); });

        music.setOnFreeCallback([](Music &music) { UnloadMusicStream(music); });
    }
};

} // namespace graphics
