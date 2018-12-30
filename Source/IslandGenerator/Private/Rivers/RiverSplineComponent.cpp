// Copyright 2018 Jay Stevens

#include "RiverSplineComponent.h"
#include "UObjectGlobals.h"

URiverSplineComponent::URiverSplineComponent()
{
	PointScale = 10.0f;
}

USplineMeshComponent* URiverSplineComponent::CreateRiver(UNamedRiver* River, int32 SplineIndex, AActor* Parent, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap)
{
	FString objectName = GetName() + " Mesh ";
	objectName.AppendInt(SplineIndex);
	USplineMeshComponent* riverSpline = NewObject<USplineMeshComponent>(Parent, FName(*objectName), RF_Transient);
	riverSpline->SetMobility(EComponentMobility::Movable);
	riverSpline->CreationMethod = EComponentCreationMethod::UserConstructionScript;

	riverSpline->SetStaticMesh(RiverMesh);
	riverSpline->bCastDynamicShadow = false;
	riverSpline->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CreateMaterial(riverSpline, RiverMaterial, TextureMap);
	SetupMaterialParameters(riverSpline);

	FVector scaleAtFirstPoint = GetScaleAtSplinePoint(SplineIndex) * PointScale * River->RiverFlow[SplineIndex];
	riverSpline->SetStartScale(FVector2D(scaleAtFirstPoint.X, scaleAtFirstPoint.Y));
	FVector scaleAtSecondPoint = GetScaleAtSplinePoint(SplineIndex + 1) * PointScale * River->RiverFlow[SplineIndex + 1];
	riverSpline->SetEndScale(FVector2D(scaleAtSecondPoint.X, scaleAtSecondPoint.Y));

	riverSpline->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	FVector startLocation;
	FVector startTangent;
	FVector endLocation;
	FVector endTangent;
	GetLocalLocationAndTangentAtSplinePoint(SplineIndex, startLocation, startTangent);
	GetLocalLocationAndTangentAtSplinePoint(SplineIndex + 1, endLocation, endTangent);

	riverSpline->SetStartAndEnd(startLocation, startTangent, endLocation, endTangent);

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

TArray<USplineMeshComponent*> URiverSplineComponent::CreateRiverMeshes(UNamedRiver* River, AActor* Parent, UMaterialInterface* RiverMaterial, TMap<FName, UTexture*> TextureMap)
{
	TArray<USplineMeshComponent*> meshes;
	for (int32 i = 0; i < GetNumberOfSplinePoints() - 1; i++)
	{
		meshes.Add(CreateRiver(River, i, Parent, RiverMaterial, TextureMap));
		CreateTangentWaterfalls();
	}
	UE_LOG(LogWorldGen, Log, TEXT("Created %d river meshes for %s."), meshes.Num(), *River->RiverName);
	return meshes;
}