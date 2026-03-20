// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPGPlayerController.h"
#include "Widget/RPGUserWidget.h"

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 멀티플레이어 환경에서 서버나 다른 클라이언트의 UI를 띄우지 않도록 로컬 플레이어인지 반드시 확인!
	if (IsLocalPlayerController() && InGameMenuClass)
	{
		// 게임 시작 시 UI를 미리 메모리에 생성해 둡니다 (보이진 않음)
		InGameMenuInstance = CreateWidget<URPGUserWidget>(this, InGameMenuClass);

		if (InGameMenuInstance)
		{
			// 화면에 추가하되, 처음에는 숨겨둡니다.
			InGameMenuInstance->AddToViewport();
			InGameMenuInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 구형 입력 방식 예시 (Enhanced Input을 쓴다면 InputAction에 바인딩하세요)
	// InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &ARPGPlayerController::ToggleInGameMenu);
}

void ARPGPlayerController::ToggleInGameMenu()
{
	// 위젯 인스턴스가 없으면 생성 (최초 1회)
	if (!InGameMenuInstance && InGameMenuClass)
	{
		InGameMenuInstance = CreateWidget<URPGUserWidget>(this, InGameMenuClass);
	}

	if (!InGameMenuInstance) return;

	bIsMenuOpen = !bIsMenuOpen;

	if (bIsMenuOpen)
	{
		// UI 클래스 내부에 구현된 완벽한 Open 로직 실행
		InGameMenuInstance->OpenUI();
	}
	else
	{
		// UI 클래스 내부에 구현된 완벽한 Close 로직 실행
		InGameMenuInstance->CloseUI();
	}
}