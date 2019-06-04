// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VR_ItemHolder.h"
#include "VR_CrossBowHolder.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API AVR_CrossBowHolder : public AVR_ItemHolder
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AVR_CrossBowHolder();
	virtual void makeItem();

};
