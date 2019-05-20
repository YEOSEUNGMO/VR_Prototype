// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VR_DataTable.h"
#include "IN_ItemOwner.h"
#include "VR_MotionController.generated.h"

//멀티캐스트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(TriggerDelegate);

UCLASS()
class VR_PROTOTYPE_API AVR_MotionController : public AActor,public IIN_ItemOwner
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RotateDummy;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DirectionPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* GrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* AttachingPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		TArray<USceneComponent*> HandOverlappedComps;

public:
	// Sets default values for this actor's properties
	AVR_MotionController();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void TriggerRelease();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void ItemDropByTrigger();

	virtual void OnConstruction(const FTransform & Transform) override;

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void StartRumbleController(float Intensity, bool loop);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void StopRumbleController();

	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void OnComponentHitOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void ReceiveTriggerPostion(float val);
	UFUNCTION(BlueprintCallable, Category = "MotionController")
		AActor* GetActorNearHand() const;

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		UPrimitiveComponent* GetComponentNearHand() const;

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void TriggerPull();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void GripPush();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void GripRelease();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void ThumbBottom(bool T_Down_F_Up);

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		void UpdateHandAnimation();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		float GetTriggerValue();

	UPROPERTY(EditAnywhere)
		float CrossBowDelayTime;

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		class USceneComponent* GetRotateDummy();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		class USceneComponent* GetAttachingPoint();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		class USceneComponent* GetDirectionPoint();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		class USkeletalMeshComponent* GetHandMesh();

	UFUNCTION(BlueprintCallable, Category = "MotionController")
		class USphereComponent* GetGrabSphere();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VRTemplate|Variables", meta = (AllowPrivateAccess = "true"))
		EGrip_Code CurrentGripState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		class UHapticFeedbackEffect_Base* VRHapticEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool bWantsToGrip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool RifleMainHand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		USceneComponent* CatchedComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rifle", meta = (AllowPrivateAccess = "true"))
		class AVR_Rifle* Rifle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool EquipedRifle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
		class AVR_CrossBow* CrossBow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool EquipedCrossBow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool DropWhenReleased;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool IsSubOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		bool TriggerState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		float TriggerValue;//TriggerPressed

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		float TriggerPullLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MotionController", meta = (AllowPrivateAccess = "true"))
		float TriggerReleaseLimit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrossBow", meta = (AllowPrivateAccess = "true"))
	FTimerHandle CrossBowTimerHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ItemIn_Implementation(AActor* Actor, class USceneComponent* Component);
	virtual bool ItemOut_Implementation(AActor* Actor);
	TriggerDelegate TriggerReleaseActions;
	FDelegateHandle TriggerReleaseActionsHandle;//델리게이트 핸들
	TriggerDelegate TriggerPullActions;
	FDelegateHandle TriggerPullActionsHandle;//델리게이트 핸들
	EControllerHand Hand;//컨트롤러의 오른쪽,왼쪽 설정.
};
