// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBase.h"
#include "VR_RifleHolder.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API AVR_RifleHolder : public ATriggerBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "RifleHolder")
		class UBoxComponent* RifleHolderCollision;
public:
	// Sets default values for this actor's properties
	AVR_RifleHolder();

};
