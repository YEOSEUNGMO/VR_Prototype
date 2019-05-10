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
	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
	AttachingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AttachingPoint"));
	static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Base> HapticEffect(TEXT("HapticFeedbackEffect_Curve'/Game/Graphics/VirtualRealityBP/Blueprints/MotionControllerHaptics.MotionControllerHaptics'"));

	RootComponent = RootScene;

	MotionController->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HandMesh->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
	MotionController->SetCollisionProfileName(TEXT("BlockAll"));

	// SomWorks :D // Setup Components
	MotionController->SetupAttachment(RotateDummy);
	MotionController->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MotionController->SetCollisionProfileName(TEXT("BlockAll"));

	RotateDummy->SetupAttachment(MotionController);
	RotateDummy->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
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

	GrabSphere->SetupAttachment(HandMesh);
	GrabSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	GrabSphere->SetSphereRadius(10.0f);
	GrabSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	GrabSphere->SetGenerateOverlapEvents(true);
	GrabSphere->OnComponentBeginOverlap.AddDynamic(this, &AVR_MotionController::OnComponentBeginOverlap);
	GrabSphere->OnComponentEndOverlap.AddDynamic(this, &AVR_MotionController::OnComponentEndOverlap);

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
		HandMesh->SetWorldScale3D(FVector(1.0f, 1.0f, -1.0f));

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
	GrabSphere->GetOverlappingComponents(OverlapComponents);
	FVector GrabSphereLocation = GrabSphere->GetComponentLocation();

	for (UPrimitiveComponent* Components : OverlapComponents)
	{
		IIN_CatchableItem* NearActor = Cast<IIN_CatchableItem>(Components->GetOwner());
		if (NearActor)
		{
			NearActor->IsCatchableComp(Components);
			float MyLength = (Components->GetComponentLocation() - GrabSphereLocation).Size();
			// float MyLengthSquared = (Actors->GetActorLocation() - GrabSphereLocation).SizeSquared();

			if (MyLength < NearestOverlap)
			{
				NearestOverlappingComponent = Components;
				NearestOverlap = MyLength;
			}
		}
		//Epic Comment :D // Filter to Actors that implement our interface for pickup/dropping
		//bool bHasInterface = NearActor->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());

	}

	return NearestOverlappingComponent;
}

AActor*  AVR_MotionController::GetActorNearHand() const
{
	AActor* NearestOverlappingActor = nullptr;
	float NearestOverlap = 10000.0f;

	// Epic Comment :D // Filtered to StaticMeshes found in the level.
	TArray<AActor*> OverlapActors;
	GrabSphere->GetOverlappingActors(OverlapActors);
	FVector GrabSphereLocation = GrabSphere->GetComponentLocation();

	for (AActor* Actors : OverlapActors)
	{
		 //Epic Comment :D // Filter to Actors that implement our interface for pickup/dropping
		bool bHasInterface = Actors->GetClass()->ImplementsInterface(UIN_CatchableItem::StaticClass());

		 //Epic Comment :D // We want to only grab simulated meshes.
		if (bHasInterface)
		{
			float MyLength = (Actors->GetActorLocation() - GrabSphereLocation).Size();
			// float MyLengthSquared = (Actors->GetActorLocation() - GrabSphereLocation).SizeSquared();

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
	if (CatchedComp== nullptr)
	{
		USceneComponent* NearComponent = GetComponentNearHand();
		AActor* NearActor = nullptr;

		/*디버그로 확인*/
		if (NearComponent)
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, NearComponent->GetName(), true, FVector2D(10.0f, 10.0f));

		if (NearComponent!=nullptr)
		{
			NearActor = NearComponent->GetOwner();
			if (ItemIn_Implementation(NearActor, NearComponent)&&DropWhenReleased)
			{
				//TriggerReleaseActions에 바인딩
				TriggerReleaseActions.AddUObject(this, &AVR_MotionController::ItemDropByTrigger);
			}


			//구버전
			//if (CatchedComp->GetClass()->IsChildOf(AVR_RifleHolder::StaticClass()))		//RifleHolder에 손을 댓는지 확인.
			//{
			//	if (!playerInfo->GetOnRifle()&&!EquipedCrossBow)//다른손에 라이플이 장착 됬는지 확인,현재 손에 크로스보우가 장착되어있는지 확인.  //AVR_Player::GetOnRifle()
			//	{
			//		if (!EquipedRifle)//내손에 라이플이 장착된 상태가 아니라면.
			//		{
			//			Rifle = GetWorld()->SpawnActorDeferred<AVR_Rifle>(AVR_Rifle::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);//라이플 스폰
			//			if (Rifle)
			//			{
			//				playerInfo->SetOnRifle(true);				//AVR_Player::SetOnRifle(true);
			//				EquipedRifle = true;
			//				RifleMainHand = true;
			//				HandAnimation->SetWeaponState(EWeaponState_Code::RifleMain);
			//				Rifle->FinishSpawning(SpawnTransform);
			//				Rifle->AttachToComponent(HandMesh, AttachRules);
			//				Rifle->setMainHand(this);
			//			}
			//		}
			//	}
			//	else if(EquipedRifle)//내 손에 무기가 장착 되있는지 확인.
			//	{
			//		playerInfo->SetOnRifle(false);		//AVR_Player::SetOnRifle(false);
			//		EquipedRifle = false;
			//		RifleMainHand = false;
			//		HandAnimation->SetWeaponState(EWeaponState_Code::Idle);
			//		Rifle->Destroy();
			//	}
			//}
			//else if (CatchedComp->GetClass()->IsChildOf(AVR_CrossBowHolder::StaticClass()))
			//{
			//	if (!playerInfo->GetOnCrossBow()&&!EquipedRifle)//다른손에 라이플이 장착 됬는지 확인,현재 손에 라이플이 장착되어있는지 확인.		//AVR_Player::GetOnCrossBow
			//	{
			//		if (!EquipedCrossBow)//내손에 라이플이 장착된 상태가 아니라면.
			//		{
			//			CrossBow = GetWorld()->SpawnActorDeferred<AVR_CrossBow>(AVR_CrossBow::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);//크로스보우 스폰
			//			if (CrossBow)
			//			{
			//				playerInfo->SetOnCrossBow(true);		//AVR_Player::SetOnCrossBow(true);
			//				EquipedCrossBow = true;
			//				CrossBow->FinishSpawning(SpawnTransform);
			//				CrossBow->AttachToComponent(HandMesh, AttachRules, TEXT("WeaponPoint"));
			//			}
			//		}
			//	}
			//	else if (EquipedCrossBow)//내 손에 무기가 장착 되있는지 확인.
			//	{
			//		playerInfo->SetOnCrossBow(false);		//AVR_Player::SetOnCrossBow(false);
			//		EquipedCrossBow = false;
			//		CrossBow->Destroy();
			//	}
			//}
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
	//구버전
	//FTimerManager& CrossBowTimer = GetWorld()->GetTimerManager();
	////GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("RELEASE!!"), true, FVector2D(10.0f, 10.0f));
	//bWantsToGrip = false;
	//if (EquipedCrossBow)
	//{
	//	CrossBowTimer.ClearTimer(CrossBowTimerHandle);
	//}
	//if (CatchedComp && CatchedComp->IsValidLowLevel() && !CatchedComp->IsPendingKill())
	//{
	//	//// Epic Comment :D // Make sure this hand is still holding the Actor (May have been taken by another hand / event)
	//	//if (CatchedComp->GetRootComponent()->GetAttachParent() == MotionController)
	//	//{
	//	//	if (CatchedComp->GetClass()->ImplementsInterface(UVR_InteractionInterface::StaticClass()))
	//	//	{
	//	//		IVR_InteractionInterface::Execute_Drop(CatchedComp); // This is the Execute_* function. The asterisk means your function name. :)
	//	//		RumbleController(0.2f);
	//	//		// Epic Comment :D // Clear the reference
	//	//		CatchedComp = nullptr;
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	// Epic Comment :D // Clear the reference
	//	//	CatchedComp = nullptr;
	//	//}
	//	CatchedComp = nullptr;
	//}
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
		//// Epic Comment :D // Make sure this hand is still holding the Actor (May have been taken by another hand / event)
		//if (CatchedComp->GetRootComponent()->GetAttachParent() == MotionController)
		//{
		//	if (CatchedComp->GetClass()->ImplementsInterface(UVR_InteractionInterface::StaticClass()))
		//	{
		//		IVR_InteractionInterface::Execute_Drop(CatchedComp); // This is the Execute_* function. The asterisk means your function name. :)

		//		RumbleController(0.2f);

		//		// Epic Comment :D // Clear the reference
		//		CatchedComp = nullptr;
		//	}
		//}
		//else
		//{
		//	// Epic Comment :D // Clear the reference
		//	CatchedComp = nullptr;
		//}

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
	if (CatchedComp->GetOwner() == nullptr)
	{
		if (HandOverlappedComps.AddUnique(OtherComp) == 0)
		{
			StartRumbleController(0.1,true);
		}
	}
	else
	{
		if (OtherActor != nullptr)
		{
			if (HandOverlappedComps.AddUnique(OtherComp) == 0)
			{
				StartRumbleController(0.1,true);
			}
		}
	}
}

void AVR_MotionController::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandOverlappedComps.Remove(OtherComp);
	if (HandOverlappedComps.Num() == 0)
	{
		StopRumbleController();
	}
}

void AVR_MotionController::OnComponentHitOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	StartRumbleController(UKismetMathLibrary::MapRangeClamped(NormalImpulse.Size(), 0, 1500, 0, 0), false);
}

void AVR_MotionController::UpdateHandAnimation()
{
	// Epic Comment :D // Update Animation of Hand
	if (CatchedComp || bWantsToGrip)
	{
		//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("GRAB!!"), true, FVector2D(10.0f, 10.0f));
		CurrentGripState = EGrip_Code::Grab;
	}
	else
	{
		AActor* NearActor = GetActorNearHand();

		if (NearActor)
		{
			CurrentGripState = EGrip_Code::CanGrab;
		}
		else
		{
			if (bWantsToGrip)
			{
				CurrentGripState = EGrip_Code::Grab;
				//GripValue = 1;
			}
			else
			{
				CurrentGripState = EGrip_Code::Open;
				//GripValue = 0;
			}
		}
	}

	// Epic Comment :D // Update the animation state of the hand mesh.
	UVR_HandAnimInstance* HandAnimation = Cast<UVR_HandAnimInstance>(HandMesh->GetAnimInstance());
	HandAnimation->SetGripState(CurrentGripState);
	//HandAnimation->SetGripState(GripValue);

	// Epic Comment :D // Only let hand collide with environment while gripping
	// SomWorks :D // Not Use Switch
	if (CurrentGripState == EGrip_Code::Open || CurrentGripState == EGrip_Code::CanGrab)
	{
		HandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (CurrentGripState == EGrip_Code::Grab)
	{
		HandMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

bool AVR_MotionController::ItemIn_Implementation(AActor* Actor, class USceneComponent* Component)
{
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	if (Actor!=nullptr)
	{
		/*Sequence 0*/
		IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(Actor);
		if (Component != nullptr)
		{
			if (CatchableActor && Component == CatchableActor->GetBaseCatchingComp())
			{
				if (CatchableActor->GetHoldingOwner() != nullptr)
				{
					IIN_ItemOwner* ItemOwner = Cast<IIN_ItemOwner>(CatchableActor->GetHoldingOwner());
					if (ItemOwner)
					{
						ItemOwner->ItemOut(Actor);
					}
				}
			}
		}

		/*Sequence 1*/
		if (CatchableActor)
		{
			USceneComponent* RealCatchedComp;
			RealCatchedComp = CatchableActor->Catched(Component, this, AttachingPoint, "", true);
			if (RealCatchedComp->IsValidLowLevel())
			{
				CatchableActor = Cast<IIN_CatchableItem>(RealCatchedComp->GetOwner());
				if (CatchableActor)
				{
					DropWhenReleased = CatchableActor->IsDroppedWhenRelease(RealCatchedComp);
					CatchedComp = RealCatchedComp;
					StartRumbleController(0.7,false);
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return false;
	
}

bool AVR_MotionController::ItemOut_Implementation(AActor* Actor)
{

	if (CatchedComp->GetOwner() == Actor&& CatchedComp !=nullptr)
	{
		IIN_CatchableItem* CatchableActor = Cast<IIN_CatchableItem>(CatchedComp->GetOwner());
		if (CatchableActor)
		{
			CatchableActor->Dropped(this);
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
			TriggerRelease();
			TriggerState = false;
		}
	}
	else
	{
		if (TriggerValue >= TriggerPullLimit)
		{
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

void AVR_MotionController::ThumbBottom(bool T_Down_F_Up)
{
	if (CatchedComp != nullptr)
	{
		IIN_ButtonUsableItem* ButtonUsableItem = Cast<IIN_ButtonUsableItem>(CatchedComp->GetOwner());
		ButtonUsableItem->BottomButton(T_Down_F_Up);		
	}
}