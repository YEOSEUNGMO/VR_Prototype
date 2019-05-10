// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IN_ItemOwner.h"
#include "IN_CatchableItem.h"
#include "VR_Weapon.generated.h"

UCLASS()
class VR_PROTOTYPE_API AVR_Weapon : public AActor, public IIN_ItemOwner, public IIN_CatchableItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVR_Weapon();

	virtual void OnFire();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
