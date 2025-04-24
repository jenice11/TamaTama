#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

struct TextureManager {
private:
	std::unordered_map<std::string, sf::Texture> textures;

public:
	sf::Texture& getTexture(const std::string& path);
};
