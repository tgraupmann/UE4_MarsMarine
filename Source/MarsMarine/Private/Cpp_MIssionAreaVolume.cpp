// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_MIssionAreaVolume.h"
#include "Cpp_Marine.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACpp_MIssionAreaVolume::ACpp_MIssionAreaVolume()
{
	SecondsBeforePlayerDeath = 5.0f;

}

void ACpp_MIssionAreaVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (IsValid(OtherActor))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
		if (Marine)
		{
			Marine->SetOutsideMissionArea(false);

			GetWorldTimerManager().ClearTimer(TimerHandleKillPlayer);
			TimerHandleKillPlayer.Invalidate();
		}
	}
}

void ACpp_MIssionAreaVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (IsValid(OtherActor))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
		if (Marine)
		{
			Marine->SetOutsideMissionArea(true);

			if (!TimerHandleKillPlayer.IsValid())
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, "KillPlayer");
				GetWorldTimerManager().SetTimer(TimerHandleKillPlayer, TimerDel, SecondsBeforePlayerDeath, false);
			}
		}
	}
}

void ACpp_MIssionAreaVolume::KillPlayer()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(Character))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(Character);
		if (Marine)
		{
			UGameplayStatics::ApplyDamage(Marine, Marine->GetHealth(), nullptr, nullptr, nullptr);
		}
	}
}
