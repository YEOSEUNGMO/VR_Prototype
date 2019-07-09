// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IN_CatchableItem.h"
#include "IN_ItemOwner.h"
#include "VR_Projectile.h"
#include "CoreMinimal.h"
#include "VR_DataTable.h"
#include "VR_CrossBow.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(TickEventDelegate, float);
UCLASS()
class VR_PROTOTYPE_API AVR_CrossBow : public AActor, public IIN_CatchableItem, public IIN_ButtonUsableItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* CrossBowMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* MainHand;//firstHand
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* SubHand;//SecondHand
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		AActor* HoldingOwner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		bool IsReadyToShot;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		bool SubHandIsInRange;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		float RechargingTime_Now;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		float RechargingTime_Need;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		FRotator HandRotDiff;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		EWeaponGripState GripState;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AVR_CrossBow();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow")
		class UBoxComponent* MainHandBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow")
		class USceneComponent* MainHandLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrossBow")
		class UBoxComponent* SubHandBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow")
		class USceneComponent* SubHandLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AVR_Projectile> ProjectileClass;//TSubclassOf:제공된 템플릿 타입의 계층 내에 바인드된 클래스 타입을 정의하도록한다.

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* ProjSpawn;

	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void AutoRecharge(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void Recharged();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void ClassifyState(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void SetGripState(EWeaponGripState state);
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void MainGrip_Enter();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void BothGrip_Enter();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void MainGrip_Tick();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void BothGrip_Tick();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		FTransform InvertTransform(FTransform transform);
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void TriggerPulled();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void TryShot(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void Shot();
	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void TriggerReleased();

	UFUNCTION(BlueprintCallable, Category = "CrossBow")
		void DrawAimLine(float DeltaTime);

	TickEventDelegate TickEvent;
	FDelegateHandle TickEventHandle_ClassifyState;
	FDelegateHandle TickEventHandle_AutoRecharge;
	FDelegateHandle TickEventHandle_DrawAimLine;
	FDelegateHandle TickEventHandle_TryShot;

	virtual AActor* Dropped_Implementation(AActor* OldOwner);
	virtual USceneComponent* Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	virtual USceneComponent* GetBaseCatchingComp_Implementation();
	virtual AActor* GetHoldingOwner_Implementation();
	virtual bool SetHoldingOwner_Implementation(AActor* Owner);
	virtual bool IsDroppedWhenRelease_Implementation(USceneComponent* targetComp);
	virtual bool IsCatchableComp_Implementation(USceneComponent* SelectedComponent);

};
