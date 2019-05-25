// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_HandAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

UVR_HandAnimInstance::UVR_HandAnimInstance()
{
	GripValue = 0.0;
	InterpSpeed = 7.0f;
	RifleGripped = false;
	CurrentGripState = EGrip_Code::Open;
	CurrentRifleGripState = ERifleGripState::NoGrip;
}

//void UVR_HandAnimInstance::setCurrentRifleGripState(ERifleGripState val)
//{
//	CurrentRifleGripState = val;
//}

//void UVR_HandAnimInstance::NativeUpdateAnimation(float DeltaTime)
//{
//	Super::NativeUpdateAnimation(DeltaTime);
//	float TempGrip = UKismetMathLibrary::Conv_IntToFloat(UKismetMathLibrary::Conv_ByteToInt((uint8)CurrentGripState)) / 2;
//	GripValue = FMath::FInterpConstantTo(GripValue, TempGrip, DeltaTime, InterpSpeed);
//}

void UVR_HandAnimInstance::SetGripValue(float val)
{
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Black, FString::SanitizeFloat(val), true, FVector2D(10.0f, 10.0f));
	GripValue = val;
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

void UVR_HandAnimInstance::setRifleGripped(bool val)
{
	RifleGripped = val;
}

void UVR_HandAnimInstance::setRifleSubGripped(bool val)
{
	RifleSubGripped = val;
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
