#include "FileKeymapRepository.hpp"

#include <filesystem>

std::optional<LoadKeymapOutput> FileKeymapRepository::loadFromPath(const std::string& path) {
    using std::filesystem::exists;
    if (!exists(path)) {
        return std::nullopt;
    }

    return LoadKeymapOutput{.layerNames = {"Base", "Lower", "Raise", "Adjust"}};
}
