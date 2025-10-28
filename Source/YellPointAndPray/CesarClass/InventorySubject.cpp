// Fill out your copyright notice in the Description page of Project Settings.


#include "CesarClass/InventorySubject.h"


//Cesar Stuff ----------------------------------------------------------------------

//void IInventorySubject::AddObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer)
//{
//    if (Observer && !Observers.Contains(Observer))
//    {
//        Observers.Add(Observer);
//    }
//}
//
//void IInventorySubject::RemoveObserver_Implementation(const TScriptInterface<IInventoryObserver>& Observer)
//{
//    Observers.Remove(Observer);
//}
//
//void IInventorySubject::NotifyObservers_Implementation(const FString& ItemName)
//{
//    for (auto& Observer : Observers)
//    {
//        if (Observer.GetObject())
//        {
//            IInventoryObserver::Execute_OnItemAdded(Observer.GetObject(), ItemName);
//        }
//    }
//}
