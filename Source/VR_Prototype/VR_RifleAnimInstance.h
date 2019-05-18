// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VR_RifleAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API UVR_RifleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		class AVR_Rifle* Rifle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		float HammerPulled;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		float TriggerPulled;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		bool HammerWillBePushed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		bool useOneHandRecharging;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		bool useTwoHandRecharging;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RifleAnim", meta = (AllowPrivateAccess = "true"))
		bool ReturnTonormal;
public:
	UVR_RifleAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "RifleAnim")
		void setRifle(class AVR_Rifle* val);
	UFUNCTION(BlueprintCallable, Category = "RifleAnim")
		void setTriggerPulled(float val);
	
};
