// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"

class URPGUserWidget;

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	// ESC 키를 눌렀을 때 메뉴를 껐다 켰다 하는 함수
	void ToggleInGameMenu();

protected:
	// 블루프린트에서 할당할 메인 메뉴 UI 클래스 (WBP_InGameMenu를 여기에 넣음)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URPGUserWidget> InGameMenuClass;

	// 화면에 띄운 메인 메뉴 UI의 실제 인스턴스를 담아둘 포인터
	UPROPERTY()
	URPGUserWidget* InGameMenuInstance;

	// 현재 메뉴가 열려있는지 상태를 추적
	bool bIsMenuOpen = false;
};
