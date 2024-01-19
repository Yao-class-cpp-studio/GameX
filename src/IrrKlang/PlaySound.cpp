#include "PlaySound.h"

namespace PlaySound {
ISoundEngine *engine;

void InitSoundEngine(){
  engine = createIrrKlangDevice();
  if(!engine){
    std::cerr << "Error creating sound engine."<< std::endl;
    return;
  }
}

void Play(const std::string& path, bool looped){
  engine->play2D(path.c_str(), looped);
}

void Play(char* path, bool looped){
  engine->play2D(path, looped);
}

void ReleaseSoundEngine(){
    engine->drop();
}
}  // namespace PlaySound