void playMusic(const char* context, int mustLoop){
    if (getparam_music_enabled())
        (*cenv)->CallStaticVoidMethod(cenv, nclass, OnStartMusic, (*cenv)->NewStringUTF(cenv, context), mustLoop);    
}

void stopMusic(){
    if (getparam_music_enabled())
        (*cenv)->CallStaticVoidMethod(cenv, nclass, OnStopMusic);    
}

void playSound(const char* context, int mustLoop){
    if (getparam_sound_enabled())
        (*cenv)->CallStaticVoidMethod(cenv, nclass, OnStartSound, (*cenv)->NewStringUTF(cenv, context), mustLoop);    
}

void adjustSoundGain(const char* context, int volume){
    if (getparam_sound_enabled())
        (*cenv)->CallStaticVoidMethod(cenv, nclass, OnVolumeSound, (*cenv)->NewStringUTF(cenv, context), volume);   
}

void haltSound(const char* context){
    if (getparam_sound_enabled())
        (*cenv)->CallStaticVoidMethod(cenv, nclass, OnStopSound, (*cenv)->NewStringUTF(cenv, context));   
}

