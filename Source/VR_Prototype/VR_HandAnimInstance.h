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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		bool RifleGrabed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		bool RifleSubGrabed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		ERifleGripState CurrentRifleGripState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		EGrip_Code CurrentGripState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		bool useOneHandReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnimation", meta = (AllowPrivateAccess = "true"))
		bool useTwoHandReload;
public:

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

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void setuseOneHandReload(bool val);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void setuseTwoHandReload(bool val);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void setRifleGrabed(bool val);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void setRifleSubGrabed(bool val);
private:

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandAnim", meta = (AllowPrivateAccess = "true"))
	//	float GripValue;

protected:

public:

};
