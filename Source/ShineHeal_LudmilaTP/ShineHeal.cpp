// Fill out your copyright notice in the Description page of Project Settings.
#include "ShineHeal.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "ShineHeal_LudmilaTPCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"

AShineHeal::AShineHeal()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(Mesh);
	SphereCollision->SetSphereRadius(200.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	SetReplicates(true);
	SetReplicateMovement(false);
}

void AShineHeal::BeginPlay()
{
	Super::BeginPlay();
	ActualizarColor();
}

void AShineHeal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShineHeal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShineHeal, bDisponible);
}

void AShineHeal::OnRep_Disponible()
{
	ActualizarColor();
}

void AShineHeal::ActualizarColor()
{
	if (!Mesh) return;

	UMaterialInstanceDynamic* MatDinamico = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0));

	if (!MatDinamico)
	{
		MatDinamico = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (MatDinamico)
	{
		if (bDisponible)
		{
			MatDinamico->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green);
		}
		else
		{
			MatDinamico->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);
		}
	}
}

void AShineHeal::Server_IntentarUsar_Implementation(ACharacter* Personaje)
{
	if (!bDisponible || !Personaje) return;

	AShineHeal_LudmilaTPCharacter* MiPersonaje = Cast<AShineHeal_LudmilaTPCharacter>(Personaje);
	if (!MiPersonaje) return;

	MiPersonaje->AgregarVida(CantidadCuracion);
	MiPersonaje->Client_MostrarMensajeCurado();

	bDisponible = false;
	ActualizarColor();

	Multicast_EfectoActivacion();

	GetWorldTimerManager().SetTimer(CooldownTimer, this, &AShineHeal::FinalizarCooldown, CooldownDuracion, false);
}

void AShineHeal::Multicast_EfectoActivacion_Implementation()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Santuario activado!"));
	}
}

void AShineHeal::FinalizarCooldown()
{
	bDisponible = true;
	ActualizarColor();
}