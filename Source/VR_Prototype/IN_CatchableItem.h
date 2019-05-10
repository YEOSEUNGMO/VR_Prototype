// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IN_CatchableItem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIN_CatchableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VR_PROTOTYPE_API IIN_CatchableItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		USceneComponent* Catched(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		AActor* Dropped(AActor* OldOwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		bool IsCatchableComp(class USceneComponent* SelectedComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		AActor* GetHoldingOwner();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		bool SetHoldingOwner(AActor* Owner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		USceneComponent* GetBaseCatchingComp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IN_CatchableItem")
		bool IsDroppedWhenRelease(class USceneComponent* targetComp);

};
