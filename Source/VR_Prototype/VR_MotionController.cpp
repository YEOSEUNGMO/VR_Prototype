// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_MotionController.h"
#include "VR_Player.h"
#include "VR_Rifle.h"
#include "VR_RifleHolder.h"
#include "VR_CrossBow.h"
#include "VR_CrossBowHolder.h"
#include "VR_HandAnimInstance.h"
#include "VR_InteractionInterface.h"
#include "VR_ItemHolder.h"
#include "IN_CatchableItem.h"
#include "IN_ButtonUsableItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/SkeletalMesh.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

// Sets default values
AVR_MotionController::AVR_MotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// SomWorks :D // Animation Blueprint // cast "UClass" and Asset path is Different Why? -> Because UAnimBlueprint Class will crash a packaged game. so use postfix "_C", animation blueprint cast to UClass.
	static ConstructorHelpers::FObjectFinder<UClass> AnimBP_HandAnimation(TEXT("AnimBlueprint'/Game/Graphics/New/Objects/Hand/Hand_AnimBP.Hand_AnimBP_C'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Hand(TEXT("SkeletalMesh'/Game/Graphics/New/Objects/Hand/Hand.Hand'"));

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RotateDummy = CreateDefaultSubobject<USceneComponent>(TEXT("RotateDummy"));
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionContoller"));
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	DirectionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DirectionPoint"));
	GripSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GripSphere"));
	AttachingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AttachingPoint"));
	static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Base> HapticEffect(TEXT("HapticFeedbackEffect_Curve'/Game/Graphics/VirtualRealityBP/Blueprints/MotionControllerHaptics.MotionControllerHaptics'"));

	RootComponent = RootScene;

	/*MotionController->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HandMesh->SetRelativeRotation(FRotator(0.0f,0.0f, 0.0f));
	MotionController->SetCollisionProfileName(TEXT("BlockAll"));*/


	// SomWorks :D // Setup Components
	MotionController->SetupAttachment(RootComponent);
	MotionController->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MotionController->SetCollisionProfileName(TEXT("BlockAll"));

	RotateDummy->SetupAttachment(MotionController);
	RotateDummy->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	DirectionPoint->SetupAttachment(RotateDummy);
	DirectionPoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	HandMesh->SetupAttachment(MotionController);
	HandMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HandMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HandMesh->SetCollisionProfileName(TEXT("NoCollision"));
	HandMesh->CastShadow = false;
	if (SK_Hand.Succeeded())
	{
		HandMesh->SetSkeletalMesh(SK_Hand.Object);
		HandMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		if (AnimBP_HandAnimation.Succeeded())
		{
			HandMesh->SetAnimInstanceClass(AnimBP_HandAnimation.Object);
		}
	}

	AttachingPoint->SetupAttachment(HandMesh);
	AttachingPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	GripSphere->SetupAttachment(HandMesh);
	GripSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	GripSphere->SetSphereRadius(10.0f);
	GripSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	GripSphere->SetGenerateOverlapEvents(true);
	GripSphere->OnComponentBeginOverlap.AddDynamic(this, &AVR_MotionController::OnComponentBeginOverlap);
	GripSphere->OnComponentEndOverlap.AddDynamic(this, &AVR_MotionController::OnComponentEndOverlap);

	if (HapticEffect.Succeeded())
	{
		VRHapticEffect = HapticEffect.Object;
	}

	/*초기 변수 세팅값*/
	Hand = EControllerHand::Left;
	CurrentGripState = EGrip_Code::Open;
	CatchedComp = nullptr;
	EquipedRifle = false;
	RifleMainHand = false;
	TriggerState = false;
	TriggerValue = 0;
	TriggerPullLimit = 1.0f;
	TriggerReleaseLimit = 0.92f;
	bWantsToGrip = false;
	CrossBowDelayTime = 0.1f;
	DropWhenReleased = true;
}

// Called when the game starts or when spawned
void AVR_MotionController::BeginPlay()
{
	Super::BeginPlay();

	if (Hand == EControllerHand::Left)
	{
		HandMesh->SetWorldScale3D(FVector(1.0f, -1.0f, 1.0f));

		//const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
		//FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);

		//Rifle = GetWorld()->SpawnActorDeferred<AVR_Rifle>(AVR_Rifle::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);//라이플 스폰
		//if (Rifle)
		//{
		//	AVR_Player::SetOnRifle(true);
		//	EquipedRifle = true;
		//	Rifle->FinishSpawning(SpawnTransform);
		//	Rifle->AttachToComponent(HandMesh, AttachRules, TEXT("WeaponPoint"));
		//}
	}


}

// Called every frame
void AVR_MotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UpdateHandAnimation();
}

UPrimitiveComponent* AVR_MotionController::GetComponentNearHand() const
{
	UPrimitiveComponent* NearestOverlappingComponent = nullptr;
	float NearestOverlap = 10000.0f;

	// Epic Comment :D // Filtered to StaticMeshes found in the level.
	TArray<UPrimitiveComponent*> OverlapComponents;
	GripSphere->GetOverlappingComponents(OverlapComponents);
	FVector GripSphereLocation = GripSphere->GetComponentLocation();

	for (UPrimitiveComponent* Components : OverlapComponents)
	{
		if(Components->GetOwner()->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass()))
		{
			float MyLength = (Components->GetComponentLocation() - GripSphereLocation).Size();

			if (IIN_CatchableItem::Execute_IsCatchableComp(Components->GetOwner(), Components) && MyLength < NearestOverlap)
			{
				NearestOverlappingComponent = Components;
				NearestOverlap = MyLength;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("Iter_Error"), true, FVector2D(10.0f, 10.0f));
		}

	}

	return NearestOverlappingComponent;
}

AActor*  AVR_MotionController::GetActorNearHand() const
{
	AActor* NearestOverlappingActor = nullptr;
	float NearestOverlap = 10000.0f;

	// Epic Comment :D // Filtered to StaticMeshes found in the level.
	TArray<AActor*> OverlapActors;
	GripSphere->GetOverlappingActors(OverlapActors);
	FVector GripSphereLocation = GripSphere->GetComponentLocation();

	for (AActor* Actors : OverlapActors)
	{
		 //Epic Comment :D // Filter to Actors that implement our interface for pickup/dropping
		bool bHasInterface = Actors->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());

		 //Epic Comment :D // We want to only Grip simulated meshes.
		if (bHasInterface)
		{
			float MyLength = (Actors->GetActorLocation() - GripSphereLocation).Size();
			// float MyLengthSquared = (Actors->GetActorLocation() - GripSphereLocation).SizeSquared();

			if (MyLength < NearestOverlap)
			{
				NearestOverlappingActor = Actors;
				NearestOverlap = MyLength;
			}
		}
	}
	
	return NearestOverlappingActor;
}

void  AVR_MotionController::TriggerPull()
{
	UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	AActor* Temp=nullptr;
	
	TriggerPullActions.Broadcast();
	USceneComponent* NearComponent = GetComponentNearHand();
	AActor* NearActor = nullptr;

	/*디버그로 확인*/
	//if (NearComponent)
		//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, NearComponent->GetName(), true, FVector2D(10.0f, 10.0f));

	if (NearComponent->IsValidLowLevel())
	{
		NearActor = NearComponent->GetOwner();
		if (CatchedComp == nullptr)//손에 아무 무기를 장착하고 있지 않은 상태.
		{
			if (ItemIn_Implementation(NearActor, NearComponent) && DropWhenReleased)
			{
				TriggerReleaseActions.AddUObject(this, &AVR_MotionController::ItemDropByTrigger);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("ITEM IN ERROR"), true, FVector2D(10.0f, 10.0f));
			}
		}
		else//무기를 장착 하고 있는 상태
		{
			bool bHasItemOwnerInterface = NearActor->GetClass()->ImplementsInterface(UIN_ItemOwner::StaticClass());
			if (bHasItemOwnerInterface)
			{
				IIN_ItemOwner::Execute_ItemIn(NearActor, CatchedComp->GetOwner(),CatchedComp);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("ITEM IN ERROR"), true, FVector2D(10.0f, 10.0f));
			}
		}
	}
	//UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	//HandAnimation->SetGripState(GripValue);
}

void AVR_MotionController::ItemDropByTrigger()
{
	if (CatchedComp != nullptr)
	{
		ItemOut(CatchedComp->GetOwner());
		//TriggerReleaseActions에 언바인딩
		TriggerReleaseActions.RemoveAll(this);	
	}
}

void AVR_MotionController::TriggerRelease()
{
	TriggerReleaseActions.Broadcast();
	
}

void  AVR_MotionController::GripPush()
{
	UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	USceneComponent* NearComponent;

	if (CatchedComp == nullptr)
	{
		NearComponent = GetComponentNearHand();

		/*디버그로 확인*/
		if (NearComponent)
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, NearComponent->GetName(), true, FVector2D(10.0f, 10.0f));

		if (NearComponent != nullptr)
		{
			AVR_ItemHolder* ItemHolder =Cast<AVR_ItemHolder>(NearComponent->GetOwner());
			ItemHolder->ItemIn_Implementation(CatchedComp->GetOwner(), CatchedComp);
		}
	}
	else
	{
		NearComponent = GetComponentNearHand();
		if (NearComponent != nullptr&&NearComponent->GetOwner()!=nullptr)
		{
			if (ItemIn_Implementation(NearComponent->GetOwner(), NearComponent) && DropWhenReleased)
			{
				//ItemDrop_Grip
			}
			
		}
	}
}

void AVR_MotionController::GripRelease()
{
	FTimerManager& CrossBowTimer = GetWorld()->GetTimerManager();
	//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("RELEASE!!"), true, FVector2D(10.0f, 10.0f));
	bWantsToGrip = false;

	if (EquipedCrossBow)
	{
		CrossBowTimer.ClearTimer(CrossBowTimerHandle);
	}
	if (CatchedComp && CatchedComp->IsValidLowLevel() && !CatchedComp->IsPendingKill())
	{
		CatchedComp = nullptr;
	}
}

void AVR_MotionController::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	// SomWorks :D // Deprecated Hand Variable in 4.19
	// MotionController->Hand = Hand; // 4.18 Code
	MotionController->SetTrackingSource(Hand); // 4.19 Code
}

void AVR_MotionController::RumbleController(float Intensity)
{
	// SomWorks :D // GetPlayerController
	APlayerController* MyPlayerController = GetWorld()->GetFirstPlayerController(); // = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		MyPlayerController->PlayHapticEffect(VRHapticEffect, Hand, Intensity);
	}
	else
	{
		// SomWorks :D // GamePad ForceFeedback, Deprecated in Epic Template
		MyPlayerController->PlayDynamicForceFeedback(Intensity, 0.1f, true, true, true, true, EDynamicForceFeedbackAction::Start);
	}
}

void AVR_MotionController::StartRumbleController(float Intensity,bool loop)
{
	// SomWorks :D // GetPlayerController
	APlayerController* MyPlayerController = GetWorld()->GetFirstPlayerController(); // = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		MyPlayerController->PlayHapticEffect(VRHapticEffect, Hand, Intensity,loop);
	}
	else
	{
		// SomWorks :D // GamePad ForceFeedback, Deprecated in Epic Template
		MyPlayerController->PlayDynamicForceFeedback(Intensity, 0.1f, true, true, true, true, EDynamicForceFeedbackAction::Start);
	}
}

void AVR_MotionController::StopRumbleController()
{
	// SomWorks :D // GetPlayerController
	APlayerController* MyPlayerController = GetWorld()->GetFirstPlayerController(); // = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		MyPlayerController->StopHapticEffect(Hand);
	}
}
// Epic Comment :D // Rumble Controller when overlapping valid StaticMesh
void AVR_MotionController::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherComp != nullptr) && (OtherComp != GripSphere))
	{

		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, OtherComp->GetName(), true, FVector2D(10.0f, 10.0f));

		// SomWorks :D // Cast the OverlapComponet to UStaticMeshComponent
		UStaticMeshComponent* const MyOverlapComponent = Cast<UStaticMeshComponent>(OtherComp);


		if (MyOverlapComponent && MyOverlapComponent->IsSimulatingPhysics())
		{
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, MyOverlapComponent->GetName(), true, FVector2D(10.0f, 10.0f));
			RumbleController(0.8f);
		}
	}

	/*if (CatchedComp->IsValidLowLevel())
	{
		if (CatchedComp->GetOwner()->IsValidLowLevel())
		{
			if (HandOverlappedComps.AddUnique(OtherComp) == 0)
			{
				RumbleController(0.1f);
			}
		}
		else
		{
			if (OtherActor != nullptr)
			{
				if (HandOverlappedComps.AddUnique(OtherComp) == 0)
				{
					RumbleController(0.1f);
				}
			}
		}
	}*/
}

void AVR_MotionController::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandOverlappedComps.Remove(OtherComp);
	if (HandOverlappedComps.Num() == 0)
	{
		StopRumbleController();
	}
}

//void AVR_MotionController::OnComponentHitOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	StartRumbleController(UKismetMathLibrary::MapRangeClamped(NormalImpulse.Size(), 0, 1500, 0, 0), false);
//}

void AVR_MotionController::UpdateHandAnimation()
{
	UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	HandAnimation->SetGripValue(TriggerValue);
	//// Epic Comment :D // Update Animation of Hand
	//if (CatchedComp || bWantsToGrip)
	//{
	//	//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Grip!!"), true, FVector2D(10.0f, 10.0f));
	//	CurrentGripState = EGrip_Code::Grip;
	//}
	//else
	//{
	//	AActor* NearActor = GetActorNearHand();

	//	if (NearActor)
	//	{
	//		CurrentGripState = EGrip_Code::CanGrip;
	//	}
	//	else
	//	{
	//		if (bWantsToGrip)
	//		{
	//			CurrentGripState = EGrip_Code::Grip;
	//			//GripValue = 1;
	//		}
	//		else
	//		{
	//			CurrentGripState = EGrip_Code::Open;
	//			//GripValue = 0;
	//		}
	//	}
	//}

	//// Epic Comment :D // Update the animation state of the hand mesh.
	//UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	//HandAnimation->SetGripState(CurrentGripState);
	//if (CurrentGripState == EGrip_Code::Grip)
	//{
	//	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow,TEXT("Grip!") , true, FVector2D(10.0f, 10.0f));
	//}
	////HandAnimation->SetGripState(GripValue);

	//// Epic Comment :D // Only let hand collide with environment while gripping
	//// SomWorks :D // Not Use Switch
	//if (CurrentGripState == EGrip_Code::Open || CurrentGripState == EGrip_Code::CanGrip)
	//{
	//	HandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	//else if (CurrentGripState == EGrip_Code::Grip)
	//{
	//	HandMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//}
}

bool AVR_MotionController::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	if (Actor!=nullptr)
	{
		/*Sequence 0*/
		bool bHasCatchableItemInterface = Actor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
		if (Component != nullptr)
		{
			if (bHasCatchableItemInterface && Component==IIN_CatchableItem::Execute_GetBaseCatchingComp(Actor))
			{
				AActor* HoldingOwner = IIN_CatchableItem::Execute_GetHoldingOwner(Actor);
				if (HoldingOwner->IsValidLowLevel())
				{
					bool bHasItemOwnerInterface = HoldingOwner->GetClass()->ImplementsInterface(UIN_ItemOwner::StaticClass());
					if (bHasItemOwnerInterface)
					{
						IIN_ItemOwner::Execute_ItemOut(HoldingOwner,Actor);
					}
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Black, TEXT("interf error"), true, FVector2D(10.0f, 10.0f));
		}
		/*Sequence 1*/
		if (bHasCatchableItemInterface)
		{
			USceneComponent* RealCatchedComp;
			RealCatchedComp = IIN_CatchableItem::Execute_Catched(Actor, Component, this, AttachingPoint, "", true);
			if (RealCatchedComp->IsValidLowLevel())
			{
				bHasCatchableItemInterface = RealCatchedComp->GetOwner()->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
				if (bHasCatchableItemInterface)
				{
					DropWhenReleased = IIN_CatchableItem::Execute_IsDroppedWhenRelease(RealCatchedComp->GetOwner(), RealCatchedComp);
					CatchedComp = RealCatchedComp;
					StartRumbleController(0.7, false);
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Black, TEXT("catched error"), true, FVector2D(10.0f, 10.0f));
			}
		}
	}

	return false;
	
}

bool AVR_MotionController::ItemOut_Implementation(AActor* Actor)
{
	if (CatchedComp->GetOwner() == Actor&& CatchedComp !=nullptr)
	{

		bool bHasCatchableItemInterface = CatchedComp->GetOwner()->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());
		if (bHasCatchableItemInterface)
		{
			IIN_CatchableItem::Execute_Dropped(CatchedComp->GetOwner(), this);
			CatchedComp = nullptr;
			return true;
		}
		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

void AVR_MotionController::ReceiveTriggerPostion(float val)
{
	if (TriggerState == true)
	{
		if (TriggerValue < TriggerReleaseLimit)
		{
			//CurrentGripState = EGrip_Code::Open;
			TriggerRelease();
			TriggerState = false;
		}
	}
	else
	{
		if (TriggerValue >= TriggerPullLimit)
		{
			//CurrentGripState = EGrip_Code::Grip;
			TriggerPull();
			TriggerState = true;
		}
	}

	TriggerValue = val;
}

float AVR_MotionController::GetTriggerValue()
{
	return TriggerValue;
}

USceneComponent* AVR_MotionController::GetRotateDummy()
{
	return RotateDummy;
}

USceneComponent* AVR_MotionController::GetDirectionPoint()
{
	return DirectionPoint;
}

USceneComponent* AVR_MotionController::GetAttachingPoint()
{
	return AttachingPoint;
}

USkeletalMeshComponent* AVR_MotionController::GetHandMesh()
{
	return HandMesh;
}

USphereComponent* AVR_MotionController::GetGripSphere()
{
	return GripSphere;
}


void AVR_MotionController::ThumbBottom(bool T_Down_F_Up)
{
	if (CatchedComp != nullptr)
	{
		/*IIN_ButtonUsableItem* ButtonUsableItem = Cast<IIN_ButtonUsableItem>(CatchedComp->GetOwner());
		ButtonUsableItem->BottomButton(T_Down_F_Up);	*/
		bool bHas_ButtonUsableItem_Interface = CatchedComp->GetOwner()->GetClass()->ImplementsInterface(UIN_ButtonUsableItem::StaticClass());
		if (bHas_ButtonUsableItem_Interface)
			IIN_ButtonUsableItem::Execute_BottomButton(CatchedComp->GetOwner(), T_Down_F_Up);
	}
}