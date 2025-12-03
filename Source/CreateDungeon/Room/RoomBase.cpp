// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"


// Sets default values
ARoomBase::ARoomBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultScenRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScenRoot"));
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	Cube_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE1"));
	Cube_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE2"));
	Cube_3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE3"));
	Cube_4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE4"));
	Cube_5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE5"));
	Cube_6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE6"));
	Cube_7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE7"));
	Cube_8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE8"));
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	DefaultScenRoot->SetupAttachment(RootComponent);
	GeometryFolder->SetupAttachment(DefaultScenRoot);
	OverlapFolder->SetupAttachment(DefaultScenRoot);

	Arrow->SetupAttachment(GeometryFolder);
	Cube_1->SetupAttachment(GeometryFolder);
	Cube_2->SetupAttachment(GeometryFolder);
	Cube_3->SetupAttachment(GeometryFolder);
	Cube_4->SetupAttachment(GeometryFolder);
	Cube_5->SetupAttachment(GeometryFolder);
	Cube_6->SetupAttachment(GeometryFolder);
	Cube_7->SetupAttachment(GeometryFolder);
	Cube_8->SetupAttachment(GeometryFolder);
	Floor->SetupAttachment(GeometryFolder);

	BoxCollision->SetupAttachment(OverlapFolder);

	Arrow->bHiddenInGame = false;


}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

