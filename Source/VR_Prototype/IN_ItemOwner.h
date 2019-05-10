// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IN_ItemOwner.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIN_ItemOwner : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VR_PROTOTYPE_API IIN_ItemOwner
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_ItemOwener")
		bool ItemIn(AActor* Actor, class USceneComponent* Component);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_ItemOwener")
		bool ItemOut(AActor* Actor);
};
