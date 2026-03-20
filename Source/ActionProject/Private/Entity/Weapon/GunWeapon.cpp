// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Weapon/GunWeapon.h"
#include "Entity/Weapon/BulletActor.h"
#include "Components/SkeletalMeshComponent.h"

void AGunWeapon::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = MaxAmmo;

    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        CachedPC = Cast<APlayerController>(OwnerPawn->GetController());
    }
}


ABulletActor* AGunWeapon::SpawnProjectile(const FGameplayEffectSpecHandle& InSpecHandle)
{
    if (!BulletClass || !GetWorld()) return nullptr;

    // 1. 시각적인 시작점 (총구 소켓)
    FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
    FRotator SpawnRotation;

    // 2. 조준점(Target Point) 계산
    if (CachedPC)
    {
        FVector CamLoc;
        FRotator CamRot;
        CachedPC->GetPlayerViewPoint(CamLoc, CamRot);

        // 카메라 정면으로 길게 레이(Line Trace)를 쏴서 충돌 지점 확인
        FVector TraceEnd = CamLoc + (CamRot.Vector() * 10000.f);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner()); // 발사자 무시
        Params.AddIgnoredActor(this);     // 무기 자신 무시

        FVector TargetPoint = TraceEnd;

        // ECC_Visibility 혹은 프로젝트에서 설정한 Bullet전용 채널 사용
        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
        {
            TargetPoint = Hit.ImpactPoint;
        }

        // [핵심] 총구에서 카메라가 가리킨 TargetPoint를 바라보는 회전값 계산
        SpawnRotation = (TargetPoint - MuzzleLocation).Rotation();
    }
    else
    {
        // 컨트롤러가 없는 경우(NPC 등)는 소켓의 정면 방향 사용
        SpawnRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }

    // 3. 액터 스폰 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 4. 총알 생성
    ABulletActor* Bullet = GetWorld()->SpawnActor<ABulletActor>(BulletClass, MuzzleLocation, SpawnRotation, SpawnParams);

    if (Bullet)
    {
        // 탄환의 충돌체가 발사자나 무기에 걸리지 않도록 설정
        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Bullet->GetRootComponent()))
        {
            RootComp->IgnoreActorWhenMoving(GetOwner(), true);
            RootComp->IgnoreActorWhenMoving(this, true);
        }

        // 총알 내부의 데이터 초기화 (데미지 정보 등 전달)
        // BulletActor 내부에 InitializeProjectile(InSpecHandle) 함수가 있다고 가정합니다.
        Bullet->InitializeProjectile(InSpecHandle);

        return Bullet;
    }

    return nullptr;
}

void AGunWeapon::InitializeWeapon(FName InRowName)
{
    Super::InitializeWeapon(InRowName);
}
