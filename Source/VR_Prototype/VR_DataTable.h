// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VR_DataTable.generated.h"

UENUM(BlueprintType)
enum class ERifleGripState: uint8
{
	NoGrip=0,
	MainGrip,
	SubGrip,
	BothGrip
};

UENUM(BlueprintType)
enum class EThumbstick_Direction : uint8
{
	Up=0,
	Down,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EGrip_Code : uint8
{
	Open = 0,
	CanGrip,
	Grip
};

UENUM(BlueprintType)
enum class ERifleFire_Code : uint8
{
	None,
	Fire,
	Reloade
};