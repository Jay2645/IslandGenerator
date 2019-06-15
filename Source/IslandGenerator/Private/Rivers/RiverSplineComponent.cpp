// Copyright 2018 Jay Stevens

#include "RiverSplineComponent.h"
#include "NamedIslandRivers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

USplineMeshComponent* URiverSplineComponent::CreateRiver(UNamedRiver* River, int32 SplineIndex, AActor* Parent, const UNamedIslandRivers* Rivers)
{
	// Store variables
	UStaticMesh* riverMesh = Rivers->RiverMesh;
	UMaterialInterface* riverMaterial = Rivers->RiverMaterial;
	float pointScale = Rivers->FlowScale;
	float minWidth = Rivers->MinRiverWidth;

	// Create unique mesh component name
	FString objectName = GetName() + " Mesh ";
	objectName.AppendInt(SplineIndex);
	// Create the component and set starting parameters
	USplineMeshComponent* riverSpline = NewObject<USplineMeshComponent>(Parent, FName(*objectName), RF_Transient);
	riverSpline->SetMobility(EComponentMobility::Movable);
	riverSpline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	riverSpline->SetStaticMesh(riverMesh);
	riverSpline->bCastDynamicShadow = false;
	riverSpline->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Create the river material
	RiverDynamicMaterial = CreateMaterial(riverSpline, riverMaterial);

	// Set the river scale
	FVector rawFirstScale = GetScaleAtSplinePoint(SplineIndex);
	FVector rawSecondScale = GetScaleAtSplinePoint(SplineIndex + 1);
	FVector scaleAtFirstPoint = (minWidth * rawFirstScale) + (pointScale * River->RiverFlow[SplineIndex]);
	riverSpline->SetStartScale(FVector2D(scaleAtFirstPoint.X, scaleAtFirstPoint.Y));
	FVector scaleAtSecondPoint = (minWidth * rawSecondScale) + (pointScale * River->RiverFlow[SplineIndex + 1]);
	riverSpline->SetEndScale(FVector2D(scaleAtSecondPoint.X, scaleAtSecondPoint.Y));

	// Attach it to the parent island
	riverSpline->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	// Make the river flow between tangents in the spline
	FVector startLocation;
	FVector startTangent;
	FVector endLocation;
	FVector endTangent;
	GetLocalLocationAndTangentAtSplinePoint(SplineIndex, startLocation, startTangent);
	GetLocalLocationAndTangentAtSplinePoint(SplineIndex + 1, endLocation, endTangent);
	riverSpline->SetStartAndEnd(startLocation, startTangent, endLocation, endTangent);

	// Finish setup
	riverSpline->RegisterComponent();

	return riverSpline;
}

void URiverSplineComponent::CreateTangentWaterfalls(AActor* Parent, int32 SplineIndex, const UNamedIslandRivers* Rivers)
{
	if (Rivers == NULL || Rivers->WaterfallParticles == NULL)
	{
		return;
	}

	FVector splineTangent = GetTangentAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	if (splineTangent.Z <= (-1.0f * Rivers->MinWaterfallHeight))
	{
		UParticleSystemComponent* waterfall = UGameplayStatics::SpawnEmitterAttached(Rivers->WaterfallParticles, this, NAME_None, GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local), GetRotationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local), EAttachLocation::KeepRelativeOffset);
		Waterfalls.Add(waterfall);
	}
}

UMaterialInstanceDynamic* URiverSplineComponent::CreateMaterial(USplineMeshComponent* SplineMesh, UMaterialInterface* RiverMaterial)
{
	UMaterialInstanceDynamic* riverMat = SplineMesh->CreateDynamicMaterialInstance(0, RiverMaterial);
	return riverMat;
}

TArray<USplineMeshComponent*> URiverSplineComponent::CreateRiverMeshes(UNamedRiver* River, AActor* Parent, const UNamedIslandRivers* Rivers)
{
	TArray<USplineMeshComponent*> meshes;
	if (River == NULL)
	{
		return meshes;
	}
	for (int32 i = 0; i < GetNumberOfSplinePoints() - 1; i++)
	{
		meshes.Add(CreateRiver(River, i, Parent, Rivers));
		CreateTangentWaterfalls(Parent, i, Rivers);
	}
	UE_LOG(LogWorldGen, Log, TEXT("Created %d river meshes for %s, with %d waterfalls."), meshes.Num(), *River->RiverName, Waterfalls.Num());
	return meshes;
}