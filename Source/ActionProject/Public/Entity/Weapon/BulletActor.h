// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "BulletActor.generated.h"

class ABulletActor;
class UGameplayEffect;

UCLASS()
class ACTIONPROJECT_API ABulletActor : public AActor
{
    GENERATED_BODY()

public:
    ABulletActor();

    void InitializeProjectile(const FGameplayEffectSpecHandle& InSpecHandle);

protected:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    class UStaticMeshComponent* BulletMesh;
    UPROPERTY(VisibleAnywhere) 
    class USphereComponent* CollisionComp;
    UPROPERTY(VisibleAnywhere) 
    class UProjectileMovementComponent* ProjectileMovement;

private:
    // 어빌리티로부터 전달받은 "데미지 설계도"
    FGameplayEffectSpecHandle DamageSpecHandle;
};
