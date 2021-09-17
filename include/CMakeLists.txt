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
        ImpEventAction.hh
        ImpGlobalConfigs.hh
        ImpGpsConfig.hh
        ImpHafxChannel.hh
        ImpMaterials.hh
        ImpOnlyDetectorConstruction.hh
        ImpPayloadFrontPanel.hh
        ImpPhysicsList.hh
        ImpPrimaryGeneratorAction.hh
        ImpRunAction.hh
        ImpScintCrystalHit.hh
        ImpScintCrystalSensitiveDetector.hh
        ImpSurfacePointPicker.hh
        ImpTempLogVol.hh
        ImpTrackingAction.hh
        ImpTrajectory.hh
        ImpVDetectorConstruction.hh
        ImpVHit.hh
        ImpWholeSatelliteConstruction.hh
        QuartzProperties.hh
        TeflonProperties.hh
)

target_sources(rand_test
    PRIVATE ImpSurfacePointPicker.hh)

target_sources(energy_picker_test
    PRIVATE ImpEnergyPicker.hh)
