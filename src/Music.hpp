#pragma once
#include <SFML/Audio.hpp>

void playMusic(const std::string& filename)
{
	sf::Music musicInst;
	if (!musicInst.openFromFile(filename)) {
		std::cout << "Failed to open music file: " << filename << '\n';
		return;
	}

	std::cout << "Playing music, file name \"" << filename << "\":" << std::endl;
	std::cout << " " << musicInst.getDuration().asSeconds() << " seconds" << std::endl;
	std::cout << " " << musicInst.getSampleRate() << " samples / sec" << std::endl;
	std::cout << " " << musicInst.getChannelCount() << " channels" << std::endl;

	musicInst.setLoop(true);
	musicInst.play();
	while (musicInst.getStatus() == sf::Music::Playing)
	{
		if (!music)
			musicInst.stop();

		if (musicRestart) {
			musicInst.stop();
			musicInst.play();
			musicRestart = false;
		}

		sf::sleep(sf::milliseconds(100));
	}
}