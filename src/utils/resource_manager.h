#pragma once

#include "flecs.h"
#include "raylib.h"
#include "string"
#include <unordered_map>

namespace graphics {
typedef int HANDLE;
constexpr HANDLE NULL_HANDLE = -1;

template <typename T> class ResourceManager {

  public:
    HANDLE Load(std::string_view path) {
        bool found = false;
        if (found) {
            HANDLE handle = hash(path);

            Texture2D texture = LoadTexture(path);
            res.insert(handle, texture);
            return handle;
        } else {
            return NULL_HANDLE;
        }
    }

    T Get(HANDLE handle) {
        auto it = res.find(handle);
        if (it != res.end()) {
            return *it;
        } else {
            return NULL_HANDLE;
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

class Resources {
    ResourceManager<Texture2D> textures;
};

} // namespace graphics
