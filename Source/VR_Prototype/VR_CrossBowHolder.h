// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBase.h"
#include "VR_CrossBowHolder.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API AVR_CrossBowHolder : public ATriggerBase
{
	GENERATED_BODY()
		
	UPROPERTY(VisibleDefaultsOnly, Category = "CrossBowHolder")
		class UBoxComponent* CrossBowHolderCollision;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBowHolder", meta = (AllowPrivateAccess = "ture"))
		class USkeletalMeshComponent* Mesh;

	// Sets default values for this actor's properties
	AVR_CrossBowHolder();

};
