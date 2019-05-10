// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VR_Projectile.h"
#include "CoreMinimal.h"
#include "VR_Weapon.h"
#include "VR_CrossBow.generated.h"

/**
 * 
 */
UCLASS()
class VR_PROTOTYPE_API AVR_CrossBow : public AVR_Weapon
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* CrossBowMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotinoController", meta = (AllowPrivateAccess = "true"))
		bool OnFireReady;

protected:
	FTimerHandle OnFireTimerHandle;

public:
	// Sets default values for this actor's properties
	AVR_CrossBow();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AVR_Projectile> ProjectileClass;//TSubclassOf:������ ���ø� Ÿ���� ���� ���� ���ε�� Ŭ���� Ÿ���� �����ϵ����Ѵ�.

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* ProjSpawn;

	UPROPERTY(EditAnywhere)
		float OnFireDelayTime;

	void SetFireReady(bool);
	virtual void OnFire() override;
};
