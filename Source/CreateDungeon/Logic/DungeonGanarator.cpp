// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGanarator.h"
#include "CreateDungeon/Room/RoomBase.h"
#include "CreateDungeon/Room/DungeonRoom1.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
// Sets default values
ADungeonGanarator::ADungeonGanarator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGanarator::BeginPlay()
{
	Super::BeginPlay();
	SpawnStarterRooms();
	SpawnNextRoom();
}

// Called every frame
void ADungeonGanarator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADungeonGanarator::SpawnStarterRooms()
{
	ADungeonRoom1* SpawnStartRoom = this->GetWorld()->SpawnActor<ADungeonRoom1>(StartRoom);
	if (SpawnStartRoom)
	{
		SpawnStartRoom->SetActorLocation(this->GetActorLocation());

		SpawnStartRoom->ExitPointsFolder->GetChildrenComponents(false, Exits);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("notfoundstartroom"));
	}
}

void ADungeonGanarator::SpawnNextRoom()
{
    if (RoomAmount <= 0 || CorridorRooms.Num() == 0) return;
    bCanSpawn = true;
    USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

    int32 RandomCorridorIndex = rand() % CorridorRooms.Num();
    TSubclassOf<ARoomBase> SelectedCorridorClass = CorridorRooms[RandomCorridorIndex];
    ARoomBase* SpawnedCorridor = this->GetWorld()->SpawnActor<ARoomBase>(SelectedCorridorClass);

    if (!SpawnedCorridor) return;

    // 위치/회전 설정
    SpawnedCorridor->SetActorLocation(SelectedExitPoint->GetComponentLocation());
    SpawnedCorridor->SetActorRotation(SelectedExitPoint->GetComponentRotation());

    // 복도 오버랩 검사
    LastestSpawnRoom = SpawnedCorridor;
    RemoveOverlappingRooms();

    // 복도가 겹쳐서 파괴되었다면 리턴
    if (!IsValid(SpawnedCorridor))
    {
        FTimerHandle TimerHandle;
        // 0.01초 뒤에 SpawnNextRoom을 다시 호출 (즉시 호출 아님 -> 스택 초기화됨)
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADungeonGanarator::SpawnNextRoom, 0.01f, false);
        return;
    }

    TArray<USceneComponent*> CorridorExits;
    SpawnedCorridor->ExitPointsFolder->GetChildrenComponents(false, CorridorExits);

    if (CorridorExits.Num() == 0)
    {
        SpawnedCorridor->Destroy();
        return;
    }

    USceneComponent* CorridorExitPoint = CorridorExits[0];

    // 실제 방 생성
    ARoomBase* SpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand() % RoomsToBeSpawned.Num()]);

    SpawnedRoom->SetActorLocation(CorridorExitPoint->GetComponentLocation());
    SpawnedRoom->SetActorRotation(CorridorExitPoint->GetComponentRotation());

    // 방 오버랩 검사
    LastestSpawnRoom = SpawnedRoom;
    RemoveOverlappingRooms();

    if (IsValid(SpawnedRoom))
    {
        // 성공: 기존 출구 제거 및 새 방 출구 추가
        Exits.Remove(SelectedExitPoint);
        RoomAmount--;
        TArray<USceneComponent*> NewRoomExits;
        SpawnedRoom->ExitPointsFolder->GetChildrenComponents(false, NewRoomExits);
        Exits.Append(NewRoomExits);
    }
    else
    {
        // 실패: 방이 겹치면 연결된 복도도 같이 파괴
        if (IsValid(SpawnedCorridor))
        {
           SpawnedCorridor->Destroy();

        }
    }

    // 다음 방 생성 (타이머 사용)
    if (RoomAmount > 0)
    {
        FTimerHandle TimerHandle;
        // 0.01초 뒤에 SpawnNextRoom을 다시 호출 (즉시 호출 아님 -> 스택 초기화됨)
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADungeonGanarator::SpawnNextRoom, 0.01f, false);
    }


    
}

void ADungeonGanarator::RemoveOverlappingRooms()
{
	TArray<USceneComponent*> OverlappedRooms;
	LastestSpawnRoom->OverlapFolder->GetChildrenComponents(false, OverlappedRooms);

	TArray<UPrimitiveComponent*> OverlapingCompoenets;

	for (USceneComponent* Element : OverlappedRooms)
	{
		Cast<UBoxComponent>(Element)->GetOverlappingComponents(OverlapingCompoenets);
	}

	for (USceneComponent* Element : OverlapingCompoenets)
	{
		bCanSpawn = false;
		//RoomAmount++;
		LastestSpawnRoom->Destroy();
		return;
	}
}
