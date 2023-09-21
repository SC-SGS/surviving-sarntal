#pragma once

#include "flecs.h"
#include "raylib.h"
#include "string"
#include <unordered_map>

namespace graphics {
typedef int HANDLE;
constexpr HANDLE NULL_HANDLE = -1;

// TODO, do not use yet
template <typename T> class ResourceManager {

  public:
    HANDLE Load(const std::string &path) {
        bool found = false;
        HANDLE hash = std::hash(path);
        auto it = res.find(hash);

        if (it != res.end()) {
            // found
            return hash;
        } else {
            // not found, create new
            return Load(LoadTexture(path.c_str()));
        }
    }

    HANDLE Load(Texture2D texture) {

        HANDLE handle = rand();
        // todo check for collisions

        res.insert({handle, texture});
        return handle;
    }

    T Get(HANDLE handle) const {
        auto it = res.find(handle);
        if (it != res.end()) {
            return it->second;
        } else {
            return T(); // TODO handle invalid handle, return placeholder / null
                        // resource
        }
    }

    void Free(HANDLE handle) {
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
};

static Resources res;

} // namespace graphics
