// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShineHeal.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class SHINEHEAL_LUDMILATP_API AShineHeal : public AActor
{
	GENERATED_BODY()

public:

	AShineHeal();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Disponible();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EfectoActivacion();

	UFUNCTION(Server, Reliable)
	void Server_IntentarUsar(ACharacter* Personaje);

	UPROPERTY(EditAnywhere, Category = "Santuario")
	float CooldownDuracion = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Santuario")
	float CantidadCuracion = 30.0f;

protected:

	virtual void BeginPlay() override;

	void FinalizarCooldown();
	void ActualizarColor();

public:

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UStaticMeshComponent* Mesh;

	UPROPERTY(ReplicatedUsing = OnRep_Disponible)
	bool bDisponible = true;

	FTimerHandle CooldownTimer;
};