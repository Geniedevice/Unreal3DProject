// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// 1. 상호작용 UI에 띄울 데이터를 정의하는 구조체입니다.
USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	// 상호작용 행동 이름 (예: "열기", "줍기", "대화하기")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionName;

	// (선택사항) 필요하다면 아이콘 이미지도 넣을 수 있습니다.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	// UTexture2D* ActionIcon;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONPROJECT_API IInteractInterface
{
    GENERATED_BODY()

public:
	// 1. 상호작용 실행 (E키)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(APawn* InstigatorPawn);

	// 2. 시선이 닿았을 때 (하이라이트 켜기 / UI 띄우기)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnFocus();

	// 3. 시선이 벗어났을 때 (하이라이트 끄기 / UI 숨기기)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEndFocus();

	// 4. 상호작용 정보 가져오기 (UI 표시용)
	// 문이면 "열기", 아이템이면 "획득" 같은 텍스트를 반환합니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FInteractionData GetInteractPrompt();
};
