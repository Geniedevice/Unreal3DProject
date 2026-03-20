// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RPGUserWidget.h"


void URPGUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 초기화 로직 (예: 애니메이션 바인딩 등)
}

void URPGUserWidget::OpenUI()
{
	// 1. 뷰포트에 없다면 추가 (ZOrder를 높게 주어 다른 UI보다 위에 뜨게 할 수도 있습니다)
	if (!IsInViewport())
	{
		AddToViewport();
	}

	// 2. 위젯 보이게 설정
	SetVisibility(ESlateVisibility::Visible);

	// 3. 나를 소유한 플레이어 컨트롤러 가져오기
	if (APlayerController* PC = GetOwningPlayer())
	{
		// 4. 입력 모드를 GameAndUI로 변경 (게임도 돌아가고 UI도 클릭 가능)
		// UIOnly로 하면 캐릭터 조작이 아예 차단됩니다. 기획에 맞게 선택하세요.
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget()); // 포커스를 이 UI로 강제 고정
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);

		// 5. 마우스 커서 표시
		PC->SetShowMouseCursor(true);

		// 주의: 멀티플레이어 환경이므로 UGameplayStatics::SetGamePaused(this, true)는 사용하지 않습니다.
	}
}

void URPGUserWidget::CloseUI()
{
	// 1. 뷰포트에서 완전히 제거 (메모리 관리 차원. 만약 자주 열고 닫는다면 RemoveFromParent 대신 SetVisibility(Hidden)만 유지해도 됩니다)
	//RemoveFromParent();

	// 2. 나를 소유한 플레이어 컨트롤러 가져오기
	if (APlayerController* PC = GetOwningPlayer())
	{
		// 3. 입력 모드를 다시 게임 전용으로 원상복구
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		// 4. 마우스 커서 숨기기
		PC->SetShowMouseCursor(false);
	}
}