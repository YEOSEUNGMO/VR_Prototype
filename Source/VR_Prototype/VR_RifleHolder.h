// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VR_ItemHolder.h"
#include "VR_RifleHolder.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API AVR_RifleHolder : public AVR_ItemHolder
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Sets default values for this actor's properties
	AVR_RifleHolder();
	virtual void makeItem();
};
