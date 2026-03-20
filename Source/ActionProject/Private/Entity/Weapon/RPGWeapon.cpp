// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Weapon/RPGWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Manager/RPGDataSubsystem.h"


ARPGWeapon::ARPGWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	// 무기 자체는 물리 충돌을 끄고, 판정용 콜리전이나 트레이스만 사용하도록 설정
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARPGWeapon::Fire(AActor* InInstigator, const FGameplayEventData& Payload)
{
	// 자식 클래스(Sword, Gun)에서 오버라이드하여 실제 로직 구현
	// 예: UE_LOG(LogTemp, Warning, TEXT("Base Weapon Fire 호출"));
}

void ARPGWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARPGWeapon, WeaponData);
}

void ARPGWeapon::InitializeWeapon(FName InRowName)
{
    // 1. 서버 권한 및 RowName 유효성 체크
    if (!HasAuthority() || InRowName.IsNone()) return;

    // 2. 게임 인스턴스 유효성 체크
    UGameInstance* GI = GetGameInstance();
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("InitializeWeapon: GameInstance is Null!"));
        return;
    }

    // 3. 서브시스템 유효성 체크
    URPGDataSubsystem* DataSubsystem = GI->GetSubsystem<URPGDataSubsystem>();
    if (!DataSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("InitializeWeapon: DataSubsystem is Null!"));
        return;
    }

    // 4. 데이터 로우 존재 여부 체크
    const FWeaponData* DataRow = DataSubsystem->GetWeaponDataRow(InRowName);
    if (DataRow)
    {
        WeaponData = *DataRow;
        UE_LOG(LogTemp, Log, TEXT("InitializeWeapon: Success for %s"), *InRowName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeWeapon: Cannot find Row %s in DataTable"), *InRowName.ToString());
    }
}

