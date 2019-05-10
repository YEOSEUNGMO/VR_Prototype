// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleAnimInstance.h"
#include"VR_Rifle.h"

void UVR_RifleAnimInstance::setRifle(AVR_Rifle* val)
{
	Rifle = val;
}

void UVR_RifleAnimInstance::setTriggerPulled(float val)
{
	TriggerPulled = val;
}