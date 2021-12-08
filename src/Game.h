#pragma once

/* 8+ Each Value */
enum class GameStatus : unsigned short 
{
	Menu = 0x08,
	Playing = 0x010,
	Paused = 0x018,
	Lost = 0x020,
	Error = 0x028,
};

/* 2+ Each Value */
enum class Controls : unsigned short
{
	WASD = 0x02,
	ArrowKeys = 0x04,
};

/* Constants */
#define BALL_SHOW_CONSOLE_WINDOW 0
#define BALL_CONTROL 0x02
constexpr const char* windowName = "ball";
constexpr std::pair<unsigned, unsigned> windowMode = { 700, 500 }; // width & height
constexpr unsigned framerateLimit = 60;
constexpr const char* menuMsg = "Welcome to ball.\nhttps://github.com/dehoisted/Ball\n\nGoal: Get 5 balls every 10 seconds\nPress space to start . . .";
constexpr const char* pauseMsg = "\nBall paused.\nPress space to continue . . .";
constexpr const char* lostMsg = "\nYou lost.\nPress space to restart . . .";
constexpr const char* fontFile = "resources/fonts/PlayfairDisplay-BlackItalic.ttf";
constexpr const char* ballLogoFile = "resources/textures/ball_logo.png";
constexpr const char* const ballTextureFile = "resources/textures/ball.png";
constexpr const char* collisionFile = "resources/audios/collision.wav";
constexpr const char* musicFile = "resources/audios/music.wav";
/* Nonconstants & Thread Safe */
inline volatile bool music = false;
inline volatile bool musicRestart = false;