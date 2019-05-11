// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VR_DataTable.h"
#include "VR_HandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API UVR_HandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UVR_HandAnimInstance();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation")
		bool RifleGraped;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation")
		bool RifleSubGraped;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation")
		ERifleGripState CurrentRifleGripState;
	//	EWeaponState_Code CurrentWeaponState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation")
		EGrip_Code CurrentGripState;

	//UFUNCTION(BlueprintCallable, Category = "VRTemplate|Teleportation")
	//	void SetGripState(float val);
	//UFUNCTION(BlueprintCallable, Category = "HandAnimation")
		//void SetWeaponState(EWeaponState_Code val);
	UFUNCTION(BlueprintCallable, Category = "HandAnimation")
		void SetGripState(EGrip_Code GripState);

	//UFUNCTION(BlueprintCallable, Category = "HandAnimation")
	//	void SetRifleGraped(bool val);
	//UFUNCTION(BlueprintCallable, Category = "HandAnimation")
	//	void SetRifleSubGraped(bool val);
private:

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnim", meta = (AllowPrivateAccess = "true"))
	//	float GripValue;

protected:

public:

};
