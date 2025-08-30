#pragma once

#include <optional>
#include <string>
#include <vector>

struct LoadKeymapInput {
    std::string path;
};

struct LoadKeymapOutput {
    std::vector<std::string> layerNames;
};

class KeymapRepositoryPort {
   public:
    virtual ~KeymapRepositoryPort() = default;
    virtual std::optional<LoadKeymapOutput> loadFromPath(const std::string& path) = 0;
};

class LoadKeymap {
   public:
    explicit LoadKeymap(KeymapRepositoryPort& repo) : m_repo(repo) {}
    std::optional<LoadKeymapOutput> execute(const LoadKeymapInput& input) {
        return m_repo.loadFromPath(input.path);
    }

   private:
    KeymapRepositoryPort& m_repo;
};
