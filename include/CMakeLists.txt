target_sources(${EXC_OUT}
    PRIVATE
        AluminumProperties.hh
        BerylliumProperties.hh
        CeBr3Properties.hh
        ChannelConstants.hh
        HousingAlloyProperties.hh
        ImpActionInitialization.hh
        ImpAnalysis.hh
        ImpDetectorPayload.hh
        ImpEnergyPicker.hh
        ImpEnergyPickerMessenger.hh
        ImpEventAction.hh
        ImpGlobalConfigs.hh
        ImpHafxChannel.hh
        ImpMaterials.hh
        ImpOnlyDetectorConstruction.hh
        ImpPayloadFrontPanel.hh
        ImpPhysicsList.hh
        ImpPrimaryGeneratorAction.hh
        ImpRunAction.hh
        ImpScintCrystalHit.hh
        ImpScintCrystalSensitiveDetector.hh
        ImpSiHit.hh
        ImpSiSensitiveDetector.hh
        ImpSteppingAction.hh
        ImpSurfacePointPicker.hh
        ImpTempLogVol.hh
        ImpTrackingAction.hh
        ImpTrajectory.hh
        ImpVDetectorConstruction.hh
        ImpVHit.hh
        ImpWholeSatelliteConstruction.hh
        PdmsProperties.hh
        QuartzProperties.hh
        SiProperties.hh
        TeflonProperties.hh
)

target_sources(rand_test
    PRIVATE ImpSurfacePointPicker.hh)

target_sources(flare_picker_test PRIVATE ImpEnergyPicker.hh ImpEnergyPickerMessenger.hh)

target_sources(mono_flat_energy_test PRIVATE ImpEnergyPicker.hh ImpEnergyPickerMessenger.hh)
