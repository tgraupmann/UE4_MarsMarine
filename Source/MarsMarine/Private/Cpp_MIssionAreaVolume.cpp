// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_MIssionAreaVolume.h"
#include "Cpp_Marine.h"
#include "Cpp_MarsMarine_GameMode.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACpp_MIssionAreaVolume::ACpp_MIssionAreaVolume()
{
	SecondsBeforePlayerDeath = 5.0f;

}

void ACpp_MIssionAreaVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsValid(OtherActor))
		{
			ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
			if (Marine)
			{
				Marine->SetOutsideMissionArea(false);

				MakeTimers();
				GetWorldTimerManager().ClearTimer(GetTimer(Marine));
				GetTimer(Marine).Invalidate();
			}
		}
	}
}

void ACpp_MIssionAreaVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsValid(OtherActor))
		{
			ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
			if (Marine)
			{
				Marine->SetOutsideMissionArea(true);

				MakeTimers();
				if (!GetTimer(Marine).IsValid())
				{
					FTimerDelegate TimerDel;
					TimerDel.BindUFunction(this, "KillPlayer", Marine);
					GetWorldTimerManager().SetTimer(GetTimer(Marine), TimerDel, SecondsBeforePlayerDeath, false);
				}
			}
		}
	}
}

void ACpp_MIssionAreaVolume::KillPlayer(ACpp_Marine* Marine)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Marine)
		{
			UGameplayStatics::ApplyDamage(Marine, Marine->GetHealth(), nullptr, nullptr, nullptr);
		}
	}
}

ACpp_Marine* ACpp_MIssionAreaVolume::GetMarine(int32 PlayerIndex) const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), PlayerIndex);
	if (IsValid(Character))
	{
		return Cast<ACpp_Marine>(Character);
	}
	return nullptr;
}

int32 ACpp_MIssionAreaVolume::GetPlayerIndex(class ACpp_Marine* Marine) const
{
	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (IsValid(BaseGameMode))
	{
		for (int PlayerIndex = 0; PlayerIndex < BaseGameMode->GetNumPlayers(); ++PlayerIndex)
		{
			ACpp_Marine* RefMarine = GetMarine(PlayerIndex);
			if (IsValid(RefMarine))
			{
				if (Marine == RefMarine)
				{
					return PlayerIndex;
				}
			}
		}
	}
	return 0;
}

void ACpp_MIssionAreaVolume::MakeTimers()
{
	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (IsValid(BaseGameMode))
	{
		while (TimersHandleKillPlayer.Num() < BaseGameMode->GetNumPlayers())
		{
			TimersHandleKillPlayer.Add(FTimerHandle());
		}
	}
}

FTimerHandle& ACpp_MIssionAreaVolume::GetTimer(ACpp_Marine* Marine)
{
	MakeTimers();
	int PlayerIndex = GetPlayerIndex(Marine);
	return TimersHandleKillPlayer[PlayerIndex];
}
