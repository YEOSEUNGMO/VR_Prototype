// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VR_DataTable.h"
#include "GameFramework/Pawn.h"
#include "VR_Player.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(TickEventDelegate, float);

UCLASS()
class VR_PROTOTYPE_API AVR_Player : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* CameraBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_ItemHolder* RifleHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_CrossBowHolder* CrossBowHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* MainHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* SubHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* MainHandFixSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		bool MainHandFixed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* SubHandFixSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* Overloadwidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		bool SubHandFixed;
public:
	// Sets default values for this pawn's properties
	AVR_Player();

	// Resets HMD orientation and position in VR
	void OnResetVR();
	UFUNCTION(BlueprintCallable, Category = "Player")
	class UCameraComponent* GetCamera();

	UFUNCTION()
		void MainHandFixSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void MainHandFixSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void SubHandFixSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void SubHandFixSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void WeaponFix(float DeltaTime);
	void Trigger_Left(float val);
	void ReleaseActor_Left();

	void Trigger_Right(float val);
	void ReleaseActor_Right();

	void R_ThumbBottom_Pressed();
	void R_ThumbBottom_Released();
	void L_ThumbBottom_Pressed();
	void L_ThumbBottom_Released();

	float GetAngleVector2D(FVector2D vector);
	EThumbstick_Direction ClassifyAngleUpAndDown(float angle);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		class AVR_MotionController* RightController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		float DefaultPlayerHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		bool bUseControllerRollToRotate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		float CrossBowHolder_HeightPercent;	//CBHG=CrossBowHolder

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		float CrossBowHolder_MinHeight;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TickEventDelegate TickEvent;
	FDelegateHandle TickEventHandle_SyncCrossBowHolder;
	FDelegateHandle TickEventHandle_WeaponFix;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SyncCrossBowHolder(float DeltaTime);

};