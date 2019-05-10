// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IN_ButtonUsableItem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIN_ButtonUsableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VR_PROTOTYPE_API IIN_ButtonUsableItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ButtonUsableItem")
		void BottomButton(bool T_Dwon_F_Up);
};
