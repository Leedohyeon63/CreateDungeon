// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
    WieldWeapon();

}

void AEnemyCharacter::WieldWeapon()
{
    //무기 없으면 리턴
    if (bIsWieldingWeapon || !WeaponClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    // 무기 스폰
    CurrentWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);

    if (CurrentWeapon)
    {
        // 소켓에 부착
        CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
        bIsWieldingWeapon = true;
    }
}

void AEnemyCharacter::DefaultAttack()
{
    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);//아직은 몽타주만
    }
}

bool AEnemyCharacter::PlayerFocus(AActor* TargetActor, float DeltaTime, float TurnSpeed)
{
    FVector Start = GetActorLocation();
    FVector End = TargetActor->GetActorLocation();
    FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(Start, End);

    FRotator CurrentRot = GetActorRotation();

    FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, TurnSpeed);

    SetActorRotation(FRotator(CurrentRot.Pitch, NewRot.Yaw, CurrentRot.Roll));

    float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw));

    return DeltaYaw < 5.0f;
}

void AEnemyCharacter::UpdateMovementSpeed(float NewSpeed)
{
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}


float AEnemyCharacter::SetMovementSpeed_Implementation(EEnemySpeed State)
{
    float TargetSpeed = 0.0f;

    //EEnemySpeed에 따라서 속도 조절
    switch (State)
    {
    case EEnemySpeed::Idle:
        TargetSpeed = 0.0f;
        break;
    case EEnemySpeed::Walking:
        TargetSpeed = 200.0f;
        break;
    case EEnemySpeed::Jumping:
        TargetSpeed = 300.0f;
        break;
    case EEnemySpeed::Sprint:
        TargetSpeed = 500.0f;
        break;
    default:
        TargetSpeed = 100.0f; // 기본값
        break;
    }

    // 실제 이동 속도 적용
    GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;

    // 변경된 속도 반환
    return TargetSpeed;
}

