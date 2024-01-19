#pragma once
#include "include/irrKlang.h"
#include <string>
#include <iostream>

using namespace irrklang;
namespace PlaySound {
void InitSoundEngine();

void Play(const std::string& path, bool looped = false);

void Play(char* path, bool looped = false);

void ReleaseSoundEngine();

}  // namespace PlaySound

