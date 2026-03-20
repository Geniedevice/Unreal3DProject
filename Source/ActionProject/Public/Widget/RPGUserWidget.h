// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API URPGUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OpenUI();

	UFUNCTION(BlueprintCallable)
	void CloseUI();

protected:
	
	virtual void NativeConstruct() override;
};
