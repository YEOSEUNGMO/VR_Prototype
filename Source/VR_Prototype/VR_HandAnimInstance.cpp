// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_HandAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

UVR_HandAnimInstance::UVR_HandAnimInstance()
{
	Grip = 0.0;
	InterpSpeed = 7.0f;
	RifleGrabed = false;
	CurrentGripState = EGrip_Code::Open;
	CurrentRifleGripState = ERifleGripState::NoGrip;
}

void UVR_HandAnimInstance::setCurrentRifleGripState(ERifleGripState val)
{
	CurrentRifleGripState = val;
}

void UVR_HandAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	float TempGrip = UKismetMathLibrary::Conv_IntToFloat(UKismetMathLibrary::Conv_ByteToInt((uint8)CurrentGripState)) / 2;
	Grip = FMath::FInterpConstantTo(Grip, TempGrip, DeltaTime, InterpSpeed);	
}

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

void UVR_HandAnimInstance::setReturnToMain(bool val)
{
	ReturnToMain = val;
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
