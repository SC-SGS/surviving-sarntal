#pragma once

#include "flecs.h"
#include "raylib.h"
#include "string"
#include <stdexcept>
#include <unordered_map>

namespace graphics {
typedef int HANDLE;
constexpr HANDLE NULL_HANDLE = -1;

// TODO, do not use yet
template <typename T> class ResourceManager {

  public:
    HANDLE load(const std::string &path) {
        // Create a unique hash for this resource
        HANDLE hash = std::hash(path);

        // check if resource was already loaded before
        auto it = res.find(hash);

        if (it != res.end()) {
            // found
            return hash;
        } else {
            // not found, create new
            return load(LoadTexture(path.c_str()));
        }
    }

    HANDLE load(Texture2D texture) {
        HANDLE handle;
        // generate unused handle
        do {
            handle = rand(); // NOLINT(*-msc50-cpp)
        } while (res.find(handle) != res.end());

        res.insert({handle, texture});
        return handle;
    }

    HANDLE load(Music music) {
        HANDLE handle;
        // generate unused handle
        do {
            handle = rand(); // NOLINT(*-msc50-cpp)
        } while (res.find(handle) != res.end());

        res.insert({handle, music});
        return handle;
    }

    T get(HANDLE handle) const {
        auto it = res.find(handle);
        if (it != res.end()) {
            return it->second;
        } else {
            throw std::runtime_error("invalid handle!");
        }
    }

    void free(HANDLE handle) {
        auto it = res.find(handle);
        if (it != res.end()) {
            res.erase(it);
        }
    }

  private:
    std::unordered_map<HANDLE, T> res;
};

struct Resources {
    ResourceManager<Texture2D> textures;
    ResourceManager<Music> music;
};

} // namespace graphics
