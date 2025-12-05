// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGanarator.h"
#include "CreateDungeon/Room/RoomBase.h"
#include "CreateDungeon/Room/DungeonRoom1.h"
#include "CreateDungeon/Logic/ClosingWall.h"
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
    InitialRoomAmount = RoomAmount;
	Super::BeginPlay();
    FTimerHandle UnusedHandle;
    //시작 방 생성
    SpawnStarterRooms();

    //다음 방 생성
	SpawnNextRoom();

    //방이 모두 생성되면 1초(임의로 정할 수 있음)후 실행(보스방 만들기, 벽 막기 등등)
    GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ADungeonGanarator::AfterEndedSpawnNomalRooms, 1.0f, false);
}

// Called every frame
void ADungeonGanarator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADungeonGanarator::SpawnStarterRooms()
{
    //시작 방 생성
	ADungeonRoom1* SpawnStartRoom = this->GetWorld()->SpawnActor<ADungeonRoom1>(StartRoom);
	if (SpawnStartRoom)
	{
		SpawnStartRoom->SetActorLocation(this->GetActorLocation());

		SpawnStartRoom->ExitPointsFolder->GetChildrenComponents(false, Exits);

        //리셋을 위해 따로 저장
        GeneratedActors.Add(SpawnStartRoom);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("notfoundstartroom"));
	}
}

void ADungeonGanarator::SpawnNextRoom()
{
    //RoomAmount가 0이거나 저장된 CorridorRooms의 요소가 없으면 리턴
    if (RoomAmount <= 0 || CorridorRooms.Num() == 0) return;
    bCanSpawn = true;
    USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

    //방 생성 전 복도 생성해서 연결
    int32 RandomCorridorIndex = rand() % CorridorRooms.Num();
    TSubclassOf<ARoomBase> SelectedCorridorClass = CorridorRooms[RandomCorridorIndex];
    ARoomBase* SpawnedCorridor = this->GetWorld()->SpawnActor<ARoomBase>(SelectedCorridorClass);

    if (SpawnedCorridor)
    {
        //복도도 생성 목록에 등록
        GeneratedActors.Add(SpawnedCorridor);
    }
    else
    {
        return;
    }

    // 위치/회전 설정
    SpawnedCorridor->SetActorLocation(SelectedExitPoint->GetComponentLocation());
    SpawnedCorridor->SetActorRotation(SelectedExitPoint->GetComponentRotation());

    // 복도 오버랩 검사
    LastestSpawnRoom = SpawnedCorridor;
    RemoveOverlappingRooms();//오버랩 있으면 제거

    // 복도가 겹쳐서 파괴되었다면 리턴
    if (!IsValid(SpawnedCorridor))
    {
        FTimerHandle TimerHandle;
        // 0.01초 뒤에 SpawnNextRoom을 다시 호출 (즉시 호출 아님 -> 스택 초기화됨)
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADungeonGanarator::SpawnNextRoom, 0.01f, false);
        return;//0.01초 사이에 아래 있는 방 생성 코드가 호출됨 그래서 retrun으로 빠르게 컷 해야함
    }

    TArray<USceneComponent*> CorridorExits;
    SpawnedCorridor->ExitPointsFolder->GetChildrenComponents(false, CorridorExits);

    if (CorridorExits.Num() == 0)
    {
        SpawnedCorridor->Destroy();
        return;
    }

    USceneComponent* CorridorExitPoint = CorridorExits[0];

    //방 생성
    ARoomBase* SpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand() % RoomsToBeSpawned.Num()]);

    if (SpawnedRoom)
    {
        // [추가] 방도 생성 목록에 등록
        GeneratedActors.Add(SpawnedRoom);
    }

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

    //재귀호출로 다음 방 생성
    if (RoomAmount > 0)
    {
        FTimerHandle TimerHandle;
        // 0.01초 뒤에 SpawnNextRoom을 다시 호출 (즉시 호출 아님 -> 스택 초기화됨)
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADungeonGanarator::SpawnNextRoom, 0.01f, false);
        // 그냥 SpawnNextRoom() 바로 불러도 상관 없는데 방 많아지면 너무 불안정해진다고 함
    }


    
}

void ADungeonGanarator::RemoveOverlappingRooms()
{
    //오버랩 된 방 제거
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

void ADungeonGanarator::AfterEndedSpawnNomalRooms()
{
    //보물방 같은 특수방 지정하는 함수, 단순히 현재 생성된 방들 중 고를거라 보스방 생성 전 돌려야함
    SelectedSpecialRoom();
    //보스방 생성
    if (!SpawnBossRoom())
    {
        return;
    }
    //안쓰는 통로 막기
    ClosingUnuusedWall();
}

void ADungeonGanarator::ClosingUnuusedWall()
{
    //모든 출구에 대해 검사
    for (USceneComponent* Element : Exits)
    {
        //막을 벽 설정
        AClosingWall* LastestClosingWallSpawned = GetWorld()->SpawnActor<AClosingWall>(ClosingWall);
        FVector RelativeOffset(0.0f, -400.0f, 100.0f);
        FVector WorldOffset = Element->GetComponentRotation().RotateVector(RelativeOffset);

        LastestClosingWallSpawned->SetActorLocation(Element->GetComponentLocation() + WorldOffset);
        LastestClosingWallSpawned->SetActorRotation(Element->GetComponentRotation() + FRotator(0.0f, 90.0f, 0.0f));
    }
    UE_LOG(LogTemp, Log, TEXT("ClosingUnuusedWall"));

}

bool ADungeonGanarator::SpawnBossRoom()
{
    //보스방 생성
    if (BossRoomClass && IsValid(LastestSpawnRoom))//안전검사
    {
        USceneComponent* BossExit = nullptr;
        for (USceneComponent* Exit : Exits)
        {
            if (Exit->GetOwner() == LastestSpawnRoom)
            {
                BossExit = Exit;
                break;
            }
        }

        if (BossExit)
        {
            ARoomBase* BossRoom = GetWorld()->SpawnActor<ARoomBase>(BossRoomClass);
            if (BossRoom)
            {
                BossRoom->SetActorLocation(BossExit->GetComponentLocation());
                BossRoom->SetActorRotation(BossExit->GetComponentRotation());

                ARoomBase* TempLast = LastestSpawnRoom;
                LastestSpawnRoom = BossRoom;
                RemoveOverlappingRooms();

                if (!IsValid(BossRoom)) // 겹쳐서 파괴됨 -> 리셋 시도
                {
                    LastestSpawnRoom = TempLast; // 복구

                    if (CurrentResetCount < MaxResetLimit)
                    {
                        //만약 보스방이 다른 방과 겹쳐서 생성에 문제가 생기면 그냥 맵 리셋 처음부터 다시 생성
                        ResetDungeon();
                        return false;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Max Retries Reached."));
                    }
                }
                else
                {
                    // 성공
                    GeneratedActors.Add(BossRoom);
                    Exits.Remove(BossExit);
                    UE_LOG(LogTemp, Warning, TEXT("Boss Room Spawned Successfully!"));
                    CurrentResetCount = 0;
                }
            }
        }
    }
    return true;
}

void ADungeonGanarator::SelectedSpecialRoom()
{
    UE_LOG(LogTemp, Log, TEXT("SelectedSpecialRoom"));

}

void ADungeonGanarator::ResetDungeon()
{
    // 1. 현재 진행 중인 타이머 모두 중지 (중요: 생성 중인 로직 멈춤)
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    // 2. 지금까지 생성된 모든 방/복도 파괴
    for (AActor* Actor : GeneratedActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    GeneratedActors.Empty(); // 목록 비우기
    Exits.Empty();           // 출구 목록 비우기

    // 3. 변수 초기화
    RoomAmount = InitialRoomAmount; // 방 개수 복구
    bCanSpawn = false;

    // 4. 다시 생성 시작
    UE_LOG(LogTemp, Warning, TEXT("!!! Dungeon Generation Failed. Retrying... (%d/%d) !!!"), CurrentResetCount + 1, MaxResetLimit);

    CurrentResetCount++;
    SpawnStarterRooms();
    SpawnNextRoom();

    // 5. 종료 타이머 다시 설정
    FTimerHandle UnusedHandle;
    GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ADungeonGanarator::AfterEndedSpawnNomalRooms, 1.0f, false);
}