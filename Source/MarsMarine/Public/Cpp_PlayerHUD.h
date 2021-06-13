// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Cpp_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class MARSMARINE_API UCpp_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	class ACpp_Marine* GetMarine() const;
	
	class ACpp_Marine* Marine;
};
