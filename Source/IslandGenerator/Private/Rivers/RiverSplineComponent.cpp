// Copyright 2018 Jay Stevens

#include "RiverSplineComponent.h"
#include "NamedIslandRivers.h"

USplineMeshComponent* URiverSplineComponent::CreateRiver(UNamedRiver* River, int32 SplineIndex, AActor* Parent, const UNamedIslandRivers* Rivers)
{
	// Store variables
	UStaticMesh* riverMesh = Rivers->RiverMesh;
	UMaterialInterface* riverMaterial = Rivers->RiverMaterial;
	TMap<FName, UTexture*> textureMap = Rivers->RiverTextures;
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
	CreateMaterial(riverSpline, riverMaterial, textureMap);
	SetupMaterialParameters(riverSpline);

	// Set the river scale
	FVector scaleAtFirstPoint = (minWidth * GetScaleAtSplinePoint(SplineIndex)) + (pointScale * River->RiverFlow[SplineIndex]);
	riverSpline->SetStartScale(FVector2D(scaleAtFirstPoint.X, scaleAtFirstPoint.Y));
	FVector scaleAtSecondPoint = (minWidth * GetScaleAtSplinePoint(SplineIndex + 1)) + (pointScale * River->RiverFlow[SplineIndex + 1]);
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

void URiverSplineComponent::CreateTangentWaterfalls()
{

}

UMaterialInstanceDynamic* URiverSplineComponent::CreateMaterial(USplineMeshComponent* SplineMesh, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap)
{
	UMaterialInstanceDynamic* riverMat = SplineMesh->CreateDynamicMaterialInstance(0, RiverMaterial);
	for (auto kvp : TextureMap)
	{
		riverMat->SetTextureParameterValue(kvp.Key, kvp.Value);
	}
	return riverMat;
}

void URiverSplineComponent::SetupMaterialParameters(USplineMeshComponent* SplineMesh)
{

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
		CreateTangentWaterfalls();
	}
	UE_LOG(LogWorldGen, Log, TEXT("Created %d river meshes for %s."), meshes.Num(), *River->RiverName);
	return meshes;
}