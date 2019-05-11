// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_HandAnimInstance.h"
#include "Engine/Engine.h"

UVR_HandAnimInstance::UVR_HandAnimInstance()
{
	//GripValue = 0;
	RifleGraped = false;
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
