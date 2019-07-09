// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VR_RifleOverLoadWidget.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API UVR_RifleOverLoadWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
		float OverloadCount;
public:
	virtual void NativeConstruct() override;
	TArray<class UImage> OverloadImgs[10];
};
