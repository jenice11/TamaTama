#include <iostream>
#include "textureManager.h"

sf::Texture& TextureManager::getTexture(const std::string& path) {
	auto findTexture = textures.find(path);
	if (findTexture == textures.end()) {
		sf::Texture t;
		if (!t.loadFromFile(path)) {
			std::cerr << "Failed to load texture: " << path << std::endl;
			throw std::runtime_error("Failed to load texture: " + path);
		}
		textures[path] = t;
		return textures[path];
	}
	else {
		return findTexture->second;
	}
}
