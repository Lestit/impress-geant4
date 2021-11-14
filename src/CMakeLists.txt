target_sources(${EXC_OUT}
    PRIVATE
        ImpActionInitialization.cc
        ImpAnalysis.cc
        ImpDetectorPayload.cc
        ImpEnergyPicker.cc
        ImpEnergyPickerMessenger.cc
        ImpEventAction.cc
        ImpGpsConfig.cc
        ImpHafxChannel.cc
        ImpMaterials.cc
        ImpOnlyDetectorConstruction.cc
        ImpPayloadFrontPanel.cc
        ImpPhysicsList.cc
        ImpPrimaryGeneratorAction.cc
        ImpRunAction.cc
        ImpScintCrystalHit.cc
        ImpScintCrystalSensitiveDetector.cc
        ImpSiHit.cc
        ImpSiSensitiveDetector.cc
        ImpSteppingAction.cc
        ImpSurfacePointPicker.cc
        ImpTempLogVol.cc
        ImpTrackingAction.cc
        ImpTrajectory.cc
        ImpVDetectorConstruction.cc
        ImpVHit.cc
        ImpWholeSatelliteConstruction.cc
)

target_sources(rand_test PRIVATE ImpSurfacePointPicker.cc)

target_sources(flare_picker_test PRIVATE ImpEnergyPicker.cc ImpEnergyPickerMessenger.cc)
target_sources(mono_flat_energy_test PRIVATE ImpEnergyPicker.cc ImpEnergyPickerMessenger.cc)
