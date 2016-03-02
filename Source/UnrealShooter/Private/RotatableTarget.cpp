// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealShooter.h"
#include "BasicSpawnPoint.h"
#include "RotatableTarget.h"
#include "UnrealShooterDataSingleton.h"
#include "Engine/DestructibleMesh.h"

ARotatableTarget::ARotatableTarget()
{
	ARotatableTarget::InitTarget();
}

void ARotatableTarget::InitTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	this->SetRootComponent(DefaultSceneRoot);

	//I had to simulate physics and disable gravity because of a bug where the DM is not visible after being spawned by code
	ConstructorHelpers::FObjectFinder<UDestructibleMesh> meshHead(TEXT("DestructibleMesh'/Game/UnrealShooter/Mesh/Target/TargetMesh_Cube_DM.TargetMesh_Cube_DM'"));
	HeadMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("HeadMesh"));
	HeadMesh->SetDestructibleMesh(meshHead.Object);
	HeadMesh->SetMobility(EComponentMobility::Movable);
	HeadMesh->AttachTo(DefaultSceneRoot);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshBody(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetStaticMesh(meshBody.Object);
	BodyMesh->AttachTo(DefaultSceneRoot);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshBase(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetStaticMesh(meshBase.Object);
	BaseMesh->AttachTo(DefaultSceneRoot);

	ARotatableTarget::InitMaterialInstance();

	//listen for my destructible's onFracture signal
	//FScriptDelegate OnHeadFractured;
	//OnHeadFractured.BindUFunction(this, "OnHeadFractured");
	//HeadMesh->OnComponentFracture.AddUnique(OnHeadFractured

	//default bool
	bool bRaiseTarget = false;
	bool bLowerTarget = false;
	bool bVanish = false;
	bool bMoveTarget = false;

	RotationalRate = DEFAULT_ROTATIONAL_RATE;
}

//load the desired material instance
void ARotatableTarget::InitMaterialInstance()
{
	ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> DefaultMaterialObj(TEXT("MaterialInstanceConstant'/Game/UnrealShooter/Materials/Instances/Targets/BasicMaterial_Inst_DefaultTarget.BasicMaterial_Inst_DefaultTarget'"));
	DefaultMaterialInst = DefaultMaterialObj.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> TransparentMaterialObj(TEXT("MaterialInstanceConstant'/Game/UnrealShooter/Materials/Instances/Targets/BasicTransparentMaterial_Inst.BasicTransparentMaterial_Inst'"));
	TransparentMaterialInst = TransparentMaterialObj.Object;
}

// Called when the game starts or when spawned
void ARotatableTarget::BeginPlay()
{
	Super::BeginPlay();
	ARotatableTarget::RaiseTarget();
}

void ARotatableTarget::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//by default all targets spawn on the ground
	this->SetActorRotation(FRotator{ LOWERED_ROTATION, 0.0f, 0.0f });

	//create dynamic instance and apply it to all the meshes
	ARotatableTarget::UpdateMaterialInstance();
}

void ARotatableTarget::UpdateMaterialInstance(bool bisTranslucent)
{
	DynamicInstance = UMaterialInstanceDynamic::Create(bisTranslucent? TransparentMaterialInst : DefaultMaterialInst, this);
	DynamicInstance->SetVectorParameterValue("BaseColor", ARotatableTarget::GetMaterialColor());
	HeadMesh->SetMaterial(0, DynamicInstance);
	HeadMesh->SetMaterial(1, DynamicInstance);
	BodyMesh->SetMaterial(0, DynamicInstance);
	BaseMesh->SetMaterial(0, DynamicInstance);
}

FLinearColor ARotatableTarget::GetMaterialColor()
{
	switch (TargetProperties.TargetType)
	{
		case ETargetType::MaleTarget:
			return LOWTARGET_COLOR;
			break;
		case ETargetType::FemaleTarget:
			return MIDTARGET_COLOR;
			break;
		case ETargetType::InnocentTarget:
			return FALSETARGET_COLOR;
			break;
		case ETargetType::SpecialTarget:
		default:
			return DEFAULTTARGET_COLOR;
			break;
	}
}

void ARotatableTarget::ApplyProperties(FRotatableTargetProperties TargetProperties, int32 TimeToLive)
{
	this->TargetProperties = TargetProperties;
	this->SetActorLocation(GetSpawnPoint(TargetProperties.InitialLocation));
	this->TimeToLive = TimeToLive;
	ARotatableTarget::SetNewLocation();
	ARotatableTarget::UpdateMaterialInstance();
}

// Called every frame
void ARotatableTarget::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bRaiseTarget)
	{
		ARotatableTarget::DoTargetUp();
	}
	if (bLowerTarget)
	{
		ARotatableTarget::DoTargetDown();
	}
	if (bVanish)
	{
		ARotatableTarget::Vanish();
	}
	if (bMoveTarget)
	{
		ARotatableTarget::UpdateTargetLocation();
	}
}

//activates the raise target animation
void ARotatableTarget::RaiseTarget()
{
	bRaiseTarget = true;
}

//activates the lower target animation
void ARotatableTarget::LowerTarget()
{
	bLowerTarget = true;
}

//Tick's function raise target until desired angle
void ARotatableTarget::DoTargetUp()
{
	FRotator actorRotation = GetActorRotation();
	if (actorRotation.Pitch > RAISED_ROTATION)
	{
		FRotator newRotation = FRotator{ actorRotation.Pitch - RotationalRate, GetActorRotation().Yaw, GetActorRotation().Roll };
		SetActorRotation(newRotation);
	}
	else
	{
		bRaiseTarget = false;
	}
}

void ARotatableTarget::DoTargetDown()
{
	FRotator actorRotation = GetActorRotation();
	if (actorRotation.Pitch < LOWERED_ROTATION)
	{
		FRotator newRotation = FRotator{ actorRotation.Pitch + RotationalRate, GetActorRotation().Yaw, GetActorRotation().Roll };
		SetActorRotation(newRotation);
	}
	else
	{
		bLowerTarget = false;
		ARotatableTarget::Die();
	}
}

FVector ARotatableTarget::GetSpawnPoint(FVector SpawnPosition)
{
	for (TActorIterator<ABasicSpawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->SpawnPosition == SpawnPosition)
		{
			return ActorItr->GetActorLocation();
		}
	}
	return FVector();
}

//if we hold a location on our properties we will move there at a certain speed, after that the target goes down
void ARotatableTarget::SetNewLocation()
{
	if (TargetProperties.Locations.Num() > 0)
	{
		FVector location = TargetProperties.Locations[0];
		NextLocation = GetSpawnPoint(location);
		TargetProperties.Locations.Remove(location);
		bMoveTarget = true;
	}
	else
	{
		//no more locations to go? then lower our target
		GetWorld()->GetTimerManager().SetTimer(TargetTimerHandle, this, &ARotatableTarget::LowerTarget, this->TimeToLive, false);
	}
}

void ARotatableTarget::UpdateTargetLocation()
{
	//shall we move on X or Y?
	FVector actorLocation = this->GetActorLocation();
	bool bMoveX = actorLocation.Y == NextLocation.Y;
	bool bMoveY = actorLocation.X == NextLocation.X;

	//in which direction are we moving?
	float xMovementRate = actorLocation.X <= NextLocation.X ? TargetProperties.Speed : TargetProperties.Speed * -1;
	float yMovementRate = actorLocation.Y <= NextLocation.Y ? TargetProperties.Speed : TargetProperties.Speed * -1;

	//we must check if we arrived at our destination
	if (bMoveX && xMovementRate > 0)
	{
		if (actorLocation.X >= NextLocation.X)
		{
			bMoveTarget = false;
		}
	}
	else if (bMoveX && xMovementRate < 0)
	{
		if (actorLocation.X <= NextLocation.X)
		{
			bMoveTarget = false;
		}
	}
	else if (bMoveY && yMovementRate > 0)
	{
		if (actorLocation.Y >= NextLocation.Y)
		{
			bMoveTarget = false;
		}
	}
	else
	{
		if (actorLocation.Y <= NextLocation.Y)
		{
			bMoveTarget = false;
		}
	}

	if (bMoveTarget)
	{
		FVector newLocation = { bMoveX ? actorLocation.X + xMovementRate : actorLocation.X,
								bMoveY ? actorLocation.Y + yMovementRate : actorLocation.Y,
								actorLocation.Z };

		this->SetActorLocation(newLocation);
	}
	else
	{
		//just o be sure, set the location to exactly the target's location
		this->SetActorLocation(NextLocation);

		//get a new location and continue moving on
		ARotatableTarget::SetNewLocation();
	}
}

//calback fired whenever the destructible mesh is destroyed, unused
/*void ARotatableTarget::OnHeadFractured(const FVector& HitPoint, const FVector& HitDirection)
{
	ARotatableTarget::startVanish();
}*/

void ARotatableTarget::OnTargetHit()
{
	ARotatableTarget::UpdateMaterialInstance(true);
	ARotatableTarget::LowerTarget();
	ARotatableTarget::startVanish();
}

void ARotatableTarget::startVanish()
{
	//start vanishing actor
	bVanish = true;
}

//vanish this actor
void ARotatableTarget::Vanish()
{
	float opacityVal;
	DynamicInstance->GetScalarParameterValue("OpacityModifier", opacityVal);
	
	if (opacityVal > 0.0f)
	{
		DynamicInstance->SetScalarParameterValue("OpacityModifier", opacityVal - 0.01f); //0.005f
	}
	else
	{
		//target has been vanished!
		bVanish = false;
	}
}

void ARotatableTarget::Die()
{
	//clear timer
	GetWorld()->GetTimerManager().ClearTimer(TargetTimerHandle);

	//launch a signal to update our sequence
	UUnrealShooterDataSingleton* DataInstance = Cast<UUnrealShooterDataSingleton>(GEngine->GameSingleton);
	DataInstance->OnTargetDestroyed.Broadcast();

	//destroy
	Destroy();
}
