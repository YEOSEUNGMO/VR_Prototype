// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_RifleAnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include"VR_Rifle.h"

UVR_RifleAnimInstance::UVR_RifleAnimInstance()
{

	TriggerPulled = 0.0f;
	HammerPulled = 1.0f;
	HammerWillBePushed = false;
	useOneHandRecharging = false;
	useTwoHandRecharging = false;
}

void UVR_RifleAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	/*Then 0*/
	if (HammerPulled && HammerPulled >= 0.0f)
	{
		HammerPulled = HammerPulled + (DeltaTime*(-100.0f));
		if (HammerPulled <= 0.0f)
		{
			HammerPulled = 0.0f;
			HammerWillBePushed = false;
			Rifle->Shot();
		}
	}
	
	/*Then 1*/
	/*static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimBP_RifleMontage(TEXT("AnimBlueprint'/Game/Graphics/Rifle/RifleBP.RifleBP_C'"));
	
	if (Montage_IsPlaying(AnimBP_RifleMontage))
	{

	}*/
}

void UVR_RifleAnimInstance::setRifle(AVR_Rifle* val)
{
	Rifle = val;
}

void UVR_RifleAnimInstance::setTriggerPulled(float val)
{
	TriggerPulled = val;
}