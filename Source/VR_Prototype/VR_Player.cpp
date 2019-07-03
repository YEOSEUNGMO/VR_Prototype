// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_Player.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VR_MotionController.h"
#include "VR_ItemHolder.h"
#include "VR_RifleHolder.h"
#include "VR_CrossBowHolder.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVR_Player::AVR_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	CameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainHandFixSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MainHandFixedSphere"));
	SubHandFixSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SubHandFixedSphere"));

	RootComponent = RootScene;
	CameraBase->SetupAttachment(RootComponent);
	VRCamera->SetupAttachment(CameraBase);
	MainHandFixSphere->SetupAttachment(VRCamera);
	MainHandFixSphere->OnComponentBeginOverlap.AddDynamic(this, &AVR_Player::MainHandFixSphereBeginOverlap);
	MainHandFixSphere->OnComponentEndOverlap.AddDynamic(this, &AVR_Player::MainHandFixSphereEndOverlap);
	SubHandFixSphere->SetupAttachment(VRCamera);
	SubHandFixSphere->OnComponentBeginOverlap.AddDynamic(this, &AVR_Player::SubHandFixSphereBeginOverlap);
	SubHandFixSphere->OnComponentEndOverlap.AddDynamic(this, &AVR_Player::SubHandFixSphereEndOverlap);

	DefaultPlayerHeight = 180.0f;
	bUseControllerRollToRotate = false;
	CrossBowHolder_HeightPercent = 0.0f;
	CrossBowHolder_MinHeight = -65.0f;
	MainHand = nullptr;
	SubHand = nullptr;
}

// Called when the game starts or when spawned
void AVR_Player::BeginPlay()
{
	Super::BeginPlay();

	//// Epic Comment :D // Setup Player Height for various Platforms (PS4, Vive, Oculus)
	FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	if (DeviceName == "SteamVR" || DeviceName == "OculusHMD")
	{
		// Epic Comment :D // Windows (Oculus / Vive)
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else
	{
		// Epic Comment :D // PS4
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		CameraBase->AddLocalOffset(FVector(0.0f, 0.0f, DefaultPlayerHeight));

		// Epic Comment :D // Force Enable. PS Move lacks thumbstick input, this option lets user adjust pawn orientation during teleport using controller Roll motion.
		bUseControllerRollToRotate = true;
	}

	// Epic Comment :D // Spawn and attach both motion controllers

	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	LeftController = GetWorld()->SpawnActorDeferred<AVR_MotionController>(AVR_MotionController::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (LeftController)
	{
		// SomWorks :D // ...setstuff here..then finish spawn..
		LeftController->Hand = EControllerHand::Left;
		LeftController->FinishSpawning(SpawnTransform); // UGameplayStatics::FinishSpawningActor(LeftController, SpawnTransform);
		LeftController->AttachToComponent(CameraBase, AttachRules);
		
	}

	RightController = GetWorld()->SpawnActorDeferred<AVR_MotionController>(AVR_MotionController::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (RightController)
	{
		RightController->Hand = EControllerHand::Right;
		RightController->FinishSpawning(SpawnTransform);
		RightController->AttachToComponent(CameraBase, AttachRules);
	}

	RifleHolder = GetWorld()->SpawnActorDeferred<AVR_RifleHolder>(AVR_RifleHolder::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (RifleHolder)
	{
		RifleHolder->AttachToComponent(VRCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
		RifleHolder->SetActorRelativeLocation(FVector(-50.0f, 0.0f, 33.0f));
		RifleHolder->FinishSpawning(SpawnTransform);
	}

	CrossBowHolder = GetWorld()->SpawnActorDeferred<AVR_CrossBowHolder>(AVR_CrossBowHolder::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (CrossBowHolder)
	{
		CrossBowHolder->AttachToComponent(CameraBase, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
		CrossBowHolder->SetActorRelativeLocation(FVector(10.0f, -80.0f, -10.0f));
		CrossBowHolder->SetActorRelativeRotation(FRotator(-90.0f, 90.0f, 180.0f));
		CrossBowHolder->SetActorRelativeScale3D(FVector(0.30f, 0.12f, 0.21f));
		CrossBowHolder->FinishSpawning(SpawnTransform);
	}

	TickEventHandle_SyncCrossBowHolder = TickEvent.AddUObject(this, &AVR_Player::SyncCrossBowHolder);
	TickEventHandle_WeaponFix = TickEvent.AddUObject(this, &AVR_Player::WeaponFix);
}

// Called every frame
void AVR_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickEvent.Broadcast(DeltaTime);
}

// Called to bind functionality to input
void AVR_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVR_Player::OnResetVR);
	PlayerInputComponent->BindAction("R_Thumb_Bottom", IE_Pressed, this, &AVR_Player::R_ThumbBottom_Pressed);
	PlayerInputComponent->BindAction("R_Thumb_Bottom", IE_Released, this, &AVR_Player::R_ThumbBottom_Released);
	PlayerInputComponent->BindAction("L_Thumb_Bottom", IE_Pressed, this, &AVR_Player::L_ThumbBottom_Pressed);
	PlayerInputComponent->BindAction("L_Thumb_Bottom", IE_Released, this, &AVR_Player::L_ThumbBottom_Released);
	
	PlayerInputComponent->BindAxis("L_TriggerButton", this, &AVR_Player::Trigger_Left);
	PlayerInputComponent->BindAxis("R_TriggerButton", this, &AVR_Player::Trigger_Right);
}

void AVR_Player::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVR_Player::ReleaseActor_Left()
{
	LeftController->GripRelease();
}

void AVR_Player::Trigger_Left(float val)
{
	LeftController->ReceiveTriggerPostion(val);
}


void AVR_Player::Trigger_Right(float val)
{
	RightController->ReceiveTriggerPostion(val);
}

void AVR_Player::ReleaseActor_Right()
{
	RightController->GripRelease();
}


UCameraComponent* AVR_Player::GetCamera()
{
	
	return VRCamera;
}

void AVR_Player::R_ThumbBottom_Pressed()
{
	float R_Thumbstick_X = InputComponent->GetAxisValue(TEXT("R_ThumbStick_X")); 
	float R_Thumbstick_Y = InputComponent->GetAxisValue(TEXT("R_ThumbStick_Y")); 

	FVector2D vector;
	vector.X = R_Thumbstick_X;
	vector.Y = R_Thumbstick_Y;

	float angle = GetAngleVector2D(vector);
	EThumbstick_Direction direction = ClassifyAngleUpAndDown(angle);
	
	switch (direction)
	{
	case EThumbstick_Direction::Up:
		break;

	case EThumbstick_Direction::Down:
		RightController->ThumbBottom(true);
		break;
	}
}

void AVR_Player::R_ThumbBottom_Released()
{
	float R_Thumbstick_X = InputComponent->GetAxisValue(TEXT("R_ThumbStick_X"));
	float R_Thumbstick_Y = InputComponent->GetAxisValue(TEXT("R_ThumbStick_Y"));

	FVector2D vector;
	vector.X = R_Thumbstick_X;
	vector.Y = R_Thumbstick_Y;

	float angle = GetAngleVector2D(vector);
	EThumbstick_Direction direction = ClassifyAngleUpAndDown(angle);

	switch (direction)
	{
	case EThumbstick_Direction::Up:
		break;

	case EThumbstick_Direction::Down:
		RightController->ThumbBottom(true);
		break;
	}
}
void AVR_Player::L_ThumbBottom_Pressed()
{
	float L_Thumbstick_X = InputComponent->GetAxisValue(TEXT("L_ThumbStick_X"));
	float L_Thumbstick_Y = InputComponent->GetAxisValue(TEXT("L_ThumbStick_Y"));

	FVector2D vector;
	vector.X = L_Thumbstick_X;
	vector.Y = L_Thumbstick_Y;

	float angle = GetAngleVector2D(vector);
	EThumbstick_Direction direction = ClassifyAngleUpAndDown(angle);

	switch (direction)
	{
	case EThumbstick_Direction::Up:
		break;

	case EThumbstick_Direction::Down:
		RightController->ThumbBottom(true);
		break;
	}
}
void AVR_Player::L_ThumbBottom_Released()
{
	float L_Thumbstick_X = InputComponent->GetAxisValue(TEXT("L_ThumbStick_X"));
	float L_Thumbstick_Y = InputComponent->GetAxisValue(TEXT("L_ThumbStick_Y"));

	FVector2D vector;
	vector.X = L_Thumbstick_X;
	vector.Y = L_Thumbstick_Y;

	float angle = GetAngleVector2D(vector);
	EThumbstick_Direction direction = ClassifyAngleUpAndDown(angle);

	switch (direction)
	{
	case EThumbstick_Direction::Up:
		break;

	case EThumbstick_Direction::Down:
		RightController->ThumbBottom(true);
		break;
	}
}

float AVR_Player::GetAngleVector2D(FVector2D vector)
{
	FVector2D normalize2D = vector.GetSafeNormal();

	if (asinf(normalize2D.Y)<0.0f)
	{
		return FMath::RadiansToDegrees(acosf(normalize2D.X));
	}
	else
	{
		return (360 - (FMath::RadiansToDegrees(acosf(normalize2D.X))));
	}
}

EThumbstick_Direction AVR_Player::ClassifyAngleUpAndDown(float angle)
{
	if (angle >= 0 && angle <= 180)
		return EThumbstick_Direction::Up;
	else
		return EThumbstick_Direction::Down;
}

void AVR_Player::SyncCrossBowHolder(float DeltaTime)
{
	FVector newVector;
	newVector.X = VRCamera->GetComponentLocation().X;
	newVector.Y = -20.0f + VRCamera->GetComponentLocation().Y;
	//newVector.Z = UKismetMathLibrary::FMax(CrossBowHolder_HeightPercent * (VRCamera->GetComponentLocation().Z - CameraBase->GetComponentLocation().Z), CrossBowHolder_MinHeight) + CameraBase->GetComponentLocation().Z;
	newVector.Z = CrossBowHolder_MinHeight + VRCamera->GetComponentLocation().Z;
	CrossBowHolder->SetActorLocation(newVector);

	/*if (VRCamera->GetComponentRotation().Roll<90.0f && VRCamera->GetComponentRotation().Roll>-90.0f && VRCamera->GetComponentRotation().Pitch > -25.0f)
	{
		FRotator newRotator;
		newRotator.Roll = CrossBowHolder->GetActorRotation().Roll;
		newRotator.Pitch = CrossBowHolder->GetActorRotation().Pitch;
		newRotator.Yaw = VRCamera->GetComponentRotation().Yaw;
		CrossBowHolder->SetActorRotation(newRotator);
	}*/
}

void AVR_Player::MainHandFixSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == RightController || OtherActor == LeftController)
	{
		MainHand = Cast<AVR_MotionController>(OtherActor);
		if (MainHand->GetCatchedComp() != nullptr)
		{
			MainHandFixed = true;
		}
		else
		{
			SubHandFixed = false;
		}
	}
}

void AVR_Player::MainHandFixSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == RightController || OtherActor == LeftController)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("MainOut!"), true, FVector2D(10.0f, 10.0f));
		MainHandFixed = false;
	}
}

void AVR_Player::SubHandFixSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == RightController || OtherActor == LeftController)
	{
		SubHand = Cast<AVR_MotionController>(OtherActor);
		if (SubHand->GetCatchedComp() != nullptr)
		{
			SubHandFixed = true;
		}
		else
		{
			SubHandFixed = false;
		}
	}
}

void AVR_Player::SubHandFixSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == RightController || OtherActor == LeftController)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("SubOut!"), true, FVector2D(10.0f, 10.0f));
		SubHandFixed = false;
	}
}

void AVR_Player::WeaponFix(float DeltaTime)
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	if (MainHand != nullptr && SubHand != nullptr)
	{
		if (MainHandFixed && SubHandFixed && MainHand->GetCatchedComp() != nullptr && SubHand->GetCatchedComp() != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("@@@@@@@"), true, FVector2D(10.0f, 10.0f));
			MainHand->GetHandMesh()->AttachToComponent(MainHandFixSphere, AttachRules);
			//SubHand->GetHandMesh()->AttachToComponent(SubHandFixSphere, AttachRules);
			MainHand->GetHandMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		}

		else
		{
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("@@@@@@@"), true, FVector2D(10.0f, 10.0f));
			MainHand->GetHandMesh()->DetachFromComponent(DetachRules);
			MainHand->GetHandMesh()->AttachToComponent(MainHand->GetRotateDummy(), AttachRules);
			//SubHand->GetHandMesh()->DetachFromComponent(DetachRules);
			//SubHand->GetHandMesh()->AttachToComponent(SubHand->GetRotateDummy(), AttachRules);
			MainHand = nullptr;
			SubHand = nullptr;
		}
	}
}