// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IN_CatchableItem.h"
#include "IN_ItemOwner.h"
#include "IN_ButtonUsableItem.h"
#include "VR_Projectile.h"
#include "CoreMinimal.h"
#include "VR_DataTable.h"
#include "VR_Rifle.generated.h"

//��Ƽĳ��Ʈ ��������Ʈ ����
DECLARE_MULTICAST_DELEGATE_OneParam(TickEventDelegate,float);
DECLARE_MULTICAST_DELEGATE(MagazineAttacheDelegate);

UCLASS()
class VR_PROTOTYPE_API AVR_Rifle : public AActor, public IIN_CatchableItem,public IIN_ButtonUsableItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* RifleMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* ReloadParent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* MainHandBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* MainHandLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* SubHandBox;
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
	//	class UBoxComponent* MagazineBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SubHandLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* ReLoadTracker;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ReLoadTarget;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* TargetMark;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TargetMarkImg;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class UVR_RifleAnimInstance* RifleAnim;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* MainHand;//firstHand
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* SubHand;//SecondHand
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		FVector Tracker_oldLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		FRotator Rotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		FRotator HandRotDiff;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		AActor* HoldingOwner;
	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class AVR_RifleMagazine* AttachedMagazine;*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		bool IsReadyToShot;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		bool IsRecharging;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		bool BottomButtonPressed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		ERifleGripState GripState;
public:
	// Sets default values for this actor's properties
	AVR_Rifle();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AVR_Projectile> ProjectileClass;//TSubclassOf:������ ���ø� Ÿ���� ���� ���� ���ε�� Ŭ���� Ÿ���� �����ϵ����Ѵ�.

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* ProjSpawn;

	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void ReloadTrackerTracing(float time);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void setMainHand(AVR_MotionController* val);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void LateUpdate(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void SetTriggerPosition(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void ClassifyState(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void TargetMarkMatching(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void TriggerPulled();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void TryShot();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void Shot();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void TriggerReleased();
	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void SetGripState(ERifleGripState state);
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void MainGrip_Enter();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void BothGrip_Enter();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void MainGrip_Tick();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		void BothGrip_Tick();
	UFUNCTION(BlueprintCallable, Category = "Rifle")
		FTransform InvertTransform(FTransform transform);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TickEventDelegate TickEvent;
	FDelegateHandle TickEventHandle;
	MagazineAttacheDelegate MagazineAttacheEvenet;
	FDelegateHandle MagazineAttacheEvenetHandle; 
	virtual AActor* Dropped_Implementation(AActor* OldOwner);
	virtual USceneComponent* Catched_Implementation(class USceneComponent* ItemComponent, AActor* Owner, class USceneComponent* OwnerComponent, FName SocketName, bool HoldedWithVisible);
	virtual USceneComponent* GetBaseCatchingComp_Implementation();
	virtual AActor* GetHoldingOwner_Implementation();
	virtual bool SetHoldingOwner_Implementation(AActor* Owner);
	virtual bool IsDroppedWhenRelease_Implementation(USceneComponent* targetComp);
	virtual bool IsCatchableComp_Implementation(USceneComponent* SelectedComponent);

	//virtual void BottomButton_Implementation(bool T_Dwon_F_Up);
	//virtual bool ItemIn_Implementation(AActor* Actor, class USceneComponent* Component);
	//virtual bool ItemOut_Implementation(AActor* Actor);
};
