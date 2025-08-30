#pragma once

#include "application/usecases/LoadKeymap.hpp"

class FileKeymapRepository : public KeymapRepositoryPort {
   public:
    std::optional<LoadKeymapOutput> loadFromPath(const std::string& path) override;
};
