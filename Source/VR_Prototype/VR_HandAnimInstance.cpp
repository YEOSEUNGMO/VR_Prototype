// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_HandAnimInstance.h"
#include "Engine/Engine.h"

UVR_HandAnimInstance::UVR_HandAnimInstance()
{
	//GripValue = 0;
	RifleGrabed = false;
	CurrentGripState = EGrip_Code::Open;
	CurrentRifleGripState = ERifleGripState::NoGrip;
	//CurrentWeaponState = EWeaponState_Code::Idle;
}
//
//void UVR_HandAnimInstance::SetGripState(float val)
//{
//	GripValue = val;
//}

void UVR_HandAnimInstance::SetGripState(EGrip_Code GripState)
{
	CurrentGripState = GripState;
}

void UVR_HandAnimInstance::setuseOneHandReload(bool val)
{
	useOneHandReload = val;
}

void UVR_HandAnimInstance::setuseTwoHandReload(bool val)
{
	useTwoHandReload = val;
}

void UVR_HandAnimInstance::setRifleGrabed(bool val)
{
	RifleGrabed = val;
}

void UVR_HandAnimInstance::setRifleSubGrabed(bool val)
{
	RifleSubGrabed = val;
}

//void UVR_HandAnimInstance::SetWeaponState(EWeaponState_Code val)
//{
//	CurrentWeaponState = val;
//}

//void UVR_HandAnimInstance::SetRifleGraped(bool val)
//{
//	RifleGraped = val;
//}
//void UVR_HandAnimInstance::SetRifleSubGraped(bool val)
//{
//	RifleSubGraped = val;
//}
