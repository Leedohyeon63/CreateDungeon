// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGanarator.h"
#include "CreateDungeon/Room/RoomBase.h"
#include "CreateDungeon/Room/DungeonRoom1.h"
#include "Components/BoxComponent.h"
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
		UE_LOG(LogTemp, Error, TEXT("·ë ¸øÃ£À½"));
	}
}

void ADungeonGanarator::SpawnNextRoom()
{
	bCanSpawn = true;
	LastestSpawnRoom = 
		this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand()% RoomsToBeSpawned.Num()]);

	USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

	LastestSpawnRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
	LastestSpawnRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());

	RemoveOverlappingRooms();
	if (bCanSpawn)
	{
		Exits.Remove(SelectedExitPoint);
		TArray<USceneComponent*> LastestExitPoints;
		LastestSpawnRoom->ExitPointsFolder->GetChildrenComponents(false, LastestExitPoints);
		Exits.Append(LastestExitPoints);
	}
	RoomAmount = RoomAmount - 1;

	if (RoomAmount > 0)
	{
		SpawnNextRoom();
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
		RoomAmount++;
		LastestSpawnRoom->Destroy();
	}
}
