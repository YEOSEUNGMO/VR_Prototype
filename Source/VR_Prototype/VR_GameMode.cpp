// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_GameMode.h"
#include "VR_Player.h"

AVR_GameMode::AVR_GameMode()
{
	DefaultPawnClass = AVR_Player::StaticClass();
}


