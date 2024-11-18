#pragma once

#include "Meta.hpp"
#include "Types/TRange.h"

namespace Acting {

	constexpr TRange<float> kSecBetweenLinesRange{ 0.25,0.35 };
	constexpr TRange<float> kProcLookatSpeakerDelayRange{ 0.0f, 0.15f };
	constexpr TRange<float> kProcLookAtDefaultBeginDelayRange{ 0.25f,0.75f };
	constexpr TRange<float> kProcLookAtDefaultEndDelayRange{ 0.08f,0.25f };
	constexpr TRange<float> kProcLookAtFinalEndDelayRange{ 0.0f,0.2f };
	constexpr TRange<float> sContributionScaleRangeDef{ 1.0f,1.0f };
	constexpr TRange<float> sAnimScaleRangeDef{ 1.0f,1.0f };
	constexpr Symbol kBaseLookAtPriKey{ 0x79E37DC8F4EA91E5 };
	constexpr Symbol kLookAtPriIncrementKey{ 0x0A398C78DEBB2D503 };
	constexpr Symbol kLookAtFadeTimeKey{ 0x31E84EFD0F4F164A };
	constexpr Symbol kLookAtGenerateInProjectKey{ 0x3619B30FCD406D18 };
	constexpr Symbol kPrefUseLegacyLookAtMaxAngle{ 0x9906F5914B07805F };
	constexpr Symbol kNoMoverDataInIdlesKey{ 0x85E2318F7535F345 };
	constexpr Symbol kTalkingDefaultMinInitialDelayKey{ 0x987C4BFD3365F7B4 };
	constexpr Symbol kListeningDefaultMinInitialDelayKey{ 0x6F44D84461CEEE36 };
	constexpr Symbol kAlwaysDefaultMinInitialDelayKey{ 0x37DB3E605513B158 };
	constexpr Symbol kTalkingDefaultMaxInitialDelayKey{ 0x6F001ADEABBDD403 };
	constexpr Symbol kListeningDefaultMaxInitialDelayKey{ 0x9D3C011E171914F7 };
	constexpr Symbol kAlwaysDefaultMaxInitialDelayKey{ 0x7DE1C03C3E2E54EE };
	constexpr Symbol kAnimFadeTimeDefKey{ 0x3AE879EB1625184D };
	constexpr Symbol kLookatStrengthKey{ 0x0EE55C02999ED1F98 };
	constexpr Symbol kAnimPreDelayDefKey{ 0x17C95DFE007ECEB2 };
	constexpr Symbol kAnimPostDelayDefKey{ 0x9A627A4316571A81 };
	constexpr Symbol kAnimPriorityDefKey{ 0x0E204FD2E718CCA1B };
	constexpr Symbol kAnimBlendingDefKey{ 0x0C1E314190D01F26 };
	constexpr Symbol kAnimScaleRangeDefKey{ 0x632C08E64BE46194 };
	constexpr Symbol kContributionScaleRangeDefKey{ 0x6DA3E623CF54498A };
	constexpr Symbol kPauseCommandDefKey{ 0x0FB9EDBBE2BE479B2 };
	constexpr Symbol kValidateEmbeddedCommandsKey{ 0x1CD8B1FCD7E493C8 };
	constexpr Symbol kStyleIdleTransitionTimeKey{ 0x6BDE8E8578A2708F };
	constexpr Symbol kStyleIdleTransitionInTimeOverrideKey{ 0x0A88B3F9F7218A160 };
	constexpr Symbol kStyleIdleTransitionOutTimeOverrideKey{ 0x0B8DEF12AFA1DAB4A };
	constexpr Symbol kStyleBaseIdleTransitionTimeKey{ 0x3974E6A76464C68A };
	constexpr Symbol kValidateVoicePreAAKey{ 0x0EA4E8DD73A00249D };
	constexpr Symbol kUseNewActingKey{ 0x1D7F3F7C5E624254 };
	constexpr Symbol kDisplayLookAtDebugDataKey{ 0x0FB304862D67CE4A4 };
	constexpr Symbol kDisplayDebugPathKey{ 0x0DDE767E8D498D4D1 };
	constexpr Symbol kSetDefaultIntensityKey{ 0x0E29CC88BC83A9DA8 };
	constexpr Symbol kDefaultIntensityValue{ 0x8B0C232138FF0D8A };
	constexpr Symbol kSetDefaultAccentTags{ 0x7F3B16B6CB2F79FE };
	constexpr Symbol kFixPopInAdditiveIdleTransition{ 0x0B72FDFD28B8C051F };
	constexpr Symbol kTalkingIdleTransitionTimeKey{ 0x12C9352DE9F6D9B3 };
	constexpr Symbol kActingIntensityKey{ 0x0DE48259EB8929B6 };
	constexpr Symbol kActingFaceAccentKey{ 0xCBAE23CE3F2B0CCE };
	constexpr Symbol kActingBodyAccentKey{ 0xEFFC9824AB71EF3D };
	constexpr Symbol kActingHead1AccentKey{ 0x7D37DAB358DDE48C };
	constexpr Symbol kActingHead2AccentKey{ 0xC86CE44964FFF54B };
	constexpr Symbol kLookAtStopKey{ 0x66E772BA8E023A4D };
	constexpr Symbol kGenerateAccentsFromLanguageResources{ 0x73AF59848641A183 };
	constexpr Symbol kEnableNewLookats{ 0x69967C61AE2C7675 };
	constexpr Symbol kUseCurvedPath{ 0x8A29C4B8ACC1A104 };
	constexpr Symbol kLegacyEnableTargetedBlockingOnAttachments{ 0x6B8EE0DEAD524B98 };
	constexpr Symbol kLegacyUseOldBGMIdleBehavior{ 0x0E35173FA6BB35AD1 };
	constexpr Symbol kPriorityKey{ 0x19E1083970E1804D };
	constexpr Symbol kFadeTimeKey{ 0x5252B0398028EEE7 };
	constexpr Symbol kPreDelayKey{ 0x49AE373C8672B756 };
	constexpr Symbol kPostDelayKey{ 0x5EAA376E1C28BFEB };
	constexpr Symbol kBlendingKey{ 0x8CD42C9E0BD6AD43 };
	constexpr Symbol kScaleRangeKey{ 0x0D07B6742C1C50C1F };
	constexpr Symbol kStyleMumbleMouthKey{ 0x7D10D6DE5CE9F014 };
	constexpr Symbol kContributionRangeKey{ 0x151FE38B3A415A9D };
	constexpr Symbol kResourceGroupsKey{ 0x89CC3329DF3EC278 };
	constexpr Symbol kStartOffsetRangeKey{ 0x54BF2651351D34B9 };
	constexpr Symbol kStyleTransitionTimeKey{ 0x0B7535A357B22778 };
	constexpr Symbol kPropertyKeyBlockOverrunPercentage{ 0x38AA965019501AF9 };
	constexpr Symbol kPropertyKeyIntensityContributionMultiplierRange{ 0x9525F5C22FC0F5D1 };
	constexpr Symbol kPropertyKeyIntensityScaleMultiplierRange{ 0x272FA01DF8115368 };
	constexpr Symbol kPropertyKeyValidIntensityRange{ 0x6C92A5EB51A06379 };
	constexpr Symbol kPropertyKeyPaletteValidIntensityRange{ 0xDB68FF2D8AE9697B };
	constexpr Symbol kPropertyKeyIntensityTimeBetweenActionsMultiplierRange{ 0x8A21C545508159E };
	constexpr Symbol kRuntimeApplyChoreGenConflictToAllKey{ 0x6890F50FAF563600 };
	constexpr Symbol kRuntimeChoreGenConflictActionKey{ 0x345FF590FDEC8D01 };
}


namespace RenderPostMaterial {
	constexpr Symbol kPropKeyMaterialTime("Post - Material Time");
	constexpr Symbol kPropKeyMaterial("Post - Material");
	constexpr Symbol kPropKeyRenderLayer("Post - Render Layer");
}

namespace T3MaterialUtil {
	constexpr Symbol kPropKeyAlphaMeshCullsLines("Material - Force Linear Culls Lines");
	constexpr Symbol kPropKeyBlendMode("Material - Blend Mode");
	constexpr Symbol kPropKeyShadowCastEnable("Material - Shadow Cast Enable");
	constexpr Symbol kPropKeyCellBands("Material - Cell Bands");
	constexpr Symbol kPropKeyClothOffsetScale("Material - Cloth Offset Fix");
	constexpr Symbol kPropKeyConformNormal("Material - Conform Normal");
	constexpr Symbol kPropKeyDecalNormalOpacity("Material - Decal Normal Opacity");
	constexpr Symbol kPropKeyDoubleSided("Material - Double Sided");
	constexpr Symbol kPropKeyDoubleSidedNormals("Material - Double Sided Normals");
	constexpr Symbol kPropKeyDrawHiddenLines("Material - Draw Hidden Lines");
	constexpr Symbol kPropKeyEnlightenAlbedoColor("Material - Enlighten Albedo Color");
	constexpr Symbol kPropKeyEnlightenAlbedoIntensity("Material - Enlighten Albedo Intensity");
	constexpr Symbol kPropKeyEnlightenEmissiveColor("Material - Enlighten Emissive Color");
	constexpr Symbol kPropKeyEnlightenEmissiveIntensity("Material - Enlighten Emissive Intensity");
	constexpr Symbol kPropKeyEnlightenPrecompute("Material - Enlighten Precompute");
	constexpr Symbol kPropKeyEnlightenTransparency("Material - Enlighten Transparency");
	constexpr Symbol kPropKeyGlossExponent("Material - Gloss Exponent");
	constexpr Symbol kPropKeyGlowIntensity("Material - Glow Intensity");
	constexpr Symbol kPropKeyHairTerms("Material - Hair Terms");
	constexpr Symbol kPropKeyHorizonFade("Material - Horizon Fade");
	constexpr Symbol kPropKeyLODFullyRough("Material - LOD Fully Rough");
	constexpr Symbol kPropKeyLightGroupKey("Material - Light Group Key");
	constexpr Symbol kPropKeyLightModel("Material - Light Model");
	constexpr Symbol kPropKeyLightWrap("Material - Light Wrap");
	constexpr Symbol kPropKeyLineCreaseAngle("Material - Crease Angle");
	constexpr Symbol kPropKeyLineGenerateBoundaries("Material - Generate Boundaries");
	constexpr Symbol kPropKeyLineGenerateCreases("Material - Generate Creases");
	constexpr Symbol kPropKeyLineGenerateJagged("Material - Generate Jagged");
	constexpr Symbol kPropKeyLineGenerateSilhouette("Material - Generate Silhouette");
	constexpr Symbol kPropKeyLineGenerateSmooth("Material - Generate Smooth");
	constexpr Symbol kPropKeyLineLightingId("Material - Line Width Light Id");
	constexpr Symbol kPropKeyLineLightingType("Material - Line Width Lighting Type");
	constexpr Symbol kPropKeyLineMaxWidth("Material - Maximum Line Width");
	constexpr Symbol kPropKeyLineMinWidth("Material - Minimum Line Width");
	constexpr Symbol kPropKeyLinePatternRepeat("Material - Line Patttern Repeat");
	constexpr Symbol kPropKeyLineSmoothJaggedCreaseAngle("Material - Smooth/Jagged Crease Angle");
	constexpr Symbol kPropKeyLineWidthFromLighting("Material - Line Width From Lighting");
	constexpr Symbol kPropKeyLitLineBias("Material - Lit Line Bias");
	constexpr Symbol kPropKeyLitLineScale("Material - Lit Line Scale");
	constexpr Symbol kPropKeyNPRLineAlphaFalloff("Material - Line Alpha Falloff Overrides");
	constexpr Symbol kPropKeyNPRLineFalloff("Material - Line Falloff Overrides");
	constexpr Symbol kPropKeyNormalSpace("Material - Normal Space");
	constexpr Symbol kPropKeyOutlineColor("Material - Outline Color");
	constexpr Symbol kPropKeyOutlineInvertColor("Material - Outline Invert Color");
	constexpr Symbol kPropKeyOutlineZRange("Material - Outline Z Range");
	constexpr Symbol kPropKeyParticleAgent3DRotation("Material - Particle Agent 3D Rotation");
	constexpr Symbol kPropKeyParticleFaceDirection("Material - Particle Face Direction");
	constexpr Symbol kPropKeyParticleGeometryOrientByRotation("Material - Particle Geometry Oreint By Rotation");
	constexpr Symbol kPropKeyParticleGeometryType("Material - Particle GeometryType");
	constexpr Symbol kPropKeyParticleQuantizeLightmap("Material - Lightmap Quantize");
	constexpr Symbol kPropKeyRimLightEnable("Material - Light Rim Enable");
	constexpr Symbol kPropKeyShadowEnable("Material - Shadow Receive Enable");
	constexpr Symbol kPropKeySpecularCellBand("Material - Specular Cell Band");
	constexpr Symbol kPropKeySpecularPower("Material - Specular Power");
	constexpr Symbol kPropKeyToonShades("Material - Toon Shades");
	constexpr Symbol kPropKeyToonTexture("Material - Toon Texture");
	constexpr Symbol kPropKeyUseArtistNormal("Material - Smooth Lines From Artist Normal");
	constexpr Symbol kPropKeyVisible("Material - Visible");
	constexpr Symbol kPropKeyDiffuseTexture("Material - Diffuse Texture");
}

constexpr Symbol kAlignBottom("Bottom");
constexpr Symbol kAlignCenter("Center");
constexpr Symbol kAlignLeft("Left");
constexpr Symbol kAlignMiddle("Middle");
constexpr Symbol kNone("None");
constexpr Symbol kAlignRight("Right");
constexpr Symbol kAlignTop("Top");
constexpr Symbol kRuntimDlgLogicName("runtime_dialog_logic.prop");
constexpr Symbol kUITestCursorTexName("cursor_point.d3dtx");
constexpr Symbol kDlgStateLogicKey("all_dlg_state.prop");

namespace DlgUtils {
	constexpr Symbol kDlgSystemInfoPropName("dialog_system_info.prop");
}

constexpr Symbol kPropKeyLightInternalData("LightEnv - Internal Data");
constexpr const char* kHashUnknown = "unknown";
constexpr const char* kTelltaleBackendServerCohortKey = "Telltale Server Cohort";
constexpr const char* kTelltaleBackendServerURLDefault = "https:://services.telltalegames.com";
constexpr const char* kHDScreenShotResolution = "Screenshot resolution";

constexpr Symbol kIncomingContribution("Incoming Contribution");
constexpr Symbol kOutgoingContribution("Outgoing Contribution");
constexpr Symbol kOwningAgentKey("Owning Agent");
constexpr Symbol kStyleGuideKey("Style Guide Type");
constexpr Symbol kStyleIdleTransitionsPropKey("module_style_idle_transitions.prop");
constexpr const char* kTransitionEndTag(":End Transition");
constexpr const char* kTransitionKey("Transition");
constexpr const char* kTransitionStartTag(":Start Transition");
constexpr Symbol kPropKeyForceVisibleInEnlighten("Render Enlighten Force Visible");
constexpr Symbol kPropKeyForceVisibleInShadow("Render Shadow Force Visible");
constexpr const char* kSceneChoreCameraLayer = "Chore";
constexpr const char* k3dSoundParametersPropName = "module_sound_3d_params.prop";
constexpr Symbol kNeutralPhoneme(6675906533782001351i64);
constexpr Symbol kPropKeyBoundingVolumeScalingFactor("Bounding Volume Scaling Factor");
constexpr Symbol kPropKeyBoundingVolumeType("Bounding Volume Type");
constexpr Symbol kPropKeyCollisionType("Collision Type");
constexpr Symbol kPropKeyCollisionsEnabled("Collisions Enabled");
constexpr Symbol kComputeStage("Compute Stage");
constexpr Symbol kConstraints("Constraints");
constexpr Symbol kHostNodeKey("Host Agent Node");
constexpr Symbol kMaxAngleIncrement("Maximum Per Second Angle Increment");
constexpr Symbol kProceduralLookAtPropName("module_procedual_look_at.prop");
constexpr Symbol kRotateHostNode("Rotate Host Node");
constexpr Symbol kTargetKey("Target Agent");
constexpr Symbol kTargetNodeKey("Target Agent Node");
constexpr Symbol kTargetNodeOffKey("Target Node Agent Offset");
constexpr Symbol kUsePrivateNode("Use Private Node");
constexpr Symbol kXAxisChore("X Axis  Chore");
constexpr Symbol kYAxisChore("Y Axis Chore");

namespace Environment {
	constexpr Symbol kPropKeyEnabledOnHigh("Env - Enabled On High");
	constexpr Symbol kPropKeyEnabledOnLow("Env - Enabled On Low");
	constexpr Symbol kPropKeyEnabledOnMedium("Env - Enabled On Medium");
	constexpr Symbol kPropKeyEnabled("Env - Enabled");
	constexpr Symbol kPropKeyFogHeightFalloff("Env - Fog Height Fallofff");
	constexpr Symbol kPropKeyFogDensity("Env - Fog Density");
	constexpr Symbol kPropKeyFogColor("Env - Fog Color");
	constexpr Symbol kPropKeyFogHeight("Env - Fog Height");
	constexpr Symbol kPropKeyFogMaxOpacity("Env - Fog Max Opacity");
	constexpr Symbol kPropKeyFogStartDistance("Env - Fog Start Distance");
	constexpr Symbol kPropKeyLightGroupSet("Env - Groups");
	constexpr Symbol kPropKeyPriority("Env - Priority");
}

constexpr Symbol kPropKeyEnableCamCuts("Enable Camera Cuts");

namespace EnvironmentTile {
	constexpr Symbol kPropKeyLightProbeData("EnvTile - Light Probe Data");
	constexpr Symbol kPropKeyReflectionLocalEnable("EnvTile - Reflection Local Enable");
	constexpr Symbol kPropKeyReflectionTexture("EnvTile - Reflection Texture");
}

namespace EnvironmentLight {
	constexpr Symbol kPropKeyAllowBaseOnStatic("EnvLight - Bake Allowed on Static");
	constexpr Symbol kPropKeyColor("EnvLight - Color");
	constexpr Symbol kPropKeyDiffuseIntensity("EnvLight - Intensity Diffuse");
	constexpr Symbol kPropKeyDimmer("EnvLight - Intensity Dimmer");
	constexpr Symbol kPropKeyDistanceFalloff("EnvLight - Distance Falloff");
	constexpr Symbol kPropKeyEnabled("EnvLight - Enabled");
	constexpr Symbol kPropKeyEnlightenBakeBehaviour("EnvLight - Enlighten Bake Behaviour");
	constexpr Symbol kPropKeyEnlightenLightIntensity("EnvLight - Enlighten Intensity");
	constexpr Symbol kPropKeyGroupEnabled("EnvLight - Enabled Group");
	constexpr Symbol kPropKeyHBAOParticipationType("EnvLight - HBAO Participation Type");
	constexpr Symbol kPropKeyInnerConeAngle("EnvLight - Spot Angle Inner");
	constexpr Symbol kPropKeyIntensity("EnvLight - Intensity");
	constexpr Symbol kPropKeyInternalData("__T3LightEnvInternalData__");
	constexpr Symbol kPropKeyLODBehaviour("EnvLight - LOD Behaviour");
	constexpr Symbol kPropKeyLOD("EnvLight - LOD Active");
	constexpr Symbol kPropKeyLightGroupSet("EnvLight - Groups");
	constexpr Symbol kPropKeyLocalPosition("EnvLight - Local Position");
	constexpr Symbol kPropKeyLocalRotation("EnvLight - Local Rotation");
	constexpr Symbol kPropKeyMobility("EnvLight - Mobility");
	constexpr Symbol kPropKeyNPRBandThresholds("EnvLight - NPR Band Thresholds");
	constexpr Symbol kPropKeyNPRBanding("EnvLight - NPR Banding");
	constexpr Symbol kPropKeyOpacity("EnvLight - Opacity");
	constexpr Symbol kPropKeyOuterConeAngle("EnvLight - Spot Angle Outer");
	constexpr Symbol kPropKeyPriority("EnvLight - Priority");
	constexpr Symbol kPropKeyRadius("EnvLight - Radius");
	constexpr Symbol kPropKeyShadowDepthBias("EnvLight - Shadow Depth Bias");
	constexpr Symbol kPropKeyShadowGoboName("EnvLight - Shadow Gobo");
	constexpr Symbol kPropKeyShadowGoboScaleU("EnvLight - Shadow Gobo Scale U");
	constexpr Symbol kPropKeyShadowGoboScaleV("EnvLight - Shadow Gobo Scale V");
	constexpr Symbol kPropKeyShadowGoboTranslateU("EnvLight - Shadow Gobo Translate U");
	constexpr Symbol kPropKeyShadowGoboTranslateV("EnvLight - Shadow Gobo Translate V");
	constexpr Symbol kPropKeyShadowMapQualityDistanceScale("EnvLight - Shadow Map Quality Distance Scale");
	constexpr Symbol kPropKeyShadowModulatedIntensity("EnvLight - Shadow Modulated Intensity");
	constexpr Symbol kPropKeyShadowNearClip("EnvLight - Shadow Near Clip");
	constexpr Symbol kPropKeyShadowQuality("EnvLight - Shadow Quality");
	constexpr Symbol kPropKeyShdowSoftness("EnvLight - Shadow Softness");
	constexpr Symbol kPropKeyShadowType("EnvLight - Shadow Type");
	constexpr Symbol kPropKeyVisibleThresholdScale("EnvLight - Visible Threshold Scale");
	constexpr Symbol kPropKeyWrap("EnvLight - Wrap");
}

namespace EnvironmentLightGroup {
	constexpr Symbol kPropKeyEnabled("EnvLightGroup - Enabled");
	constexpr Symbol kPropKeyEnlightenLightIntensity("EnvLightGroup - Enlighten Intensity");
	constexpr Symbol kPropKeyLightGroupSet("EnvLightGroup - Groups");
	constexpr Symbol kPropKeyPriority("EnvLightGroup - Priority");
}

enum MeshDebugRenderType {
	eMeshRender_Solid = 1,
	eMeshRender_Wireframe = 2
};

struct EnumMeshDebugRenderType : EnumBase {
	MeshDebugRenderType mVal;
};

namespace CinematicLight {
	constexpr Symbol kPropKeyColor("CinLight - Color");
	constexpr Symbol kPropKeyDiffuseIntensity("CinLight - Intensity Diffuse");
	constexpr Symbol kPropKeyDimmer("CinLight - Dimmer");
	constexpr Symbol kPropKeyEnabled("CinLight - Enabled");
	constexpr Symbol kPropKeyHBAOParticipationType("CinLight - HBAO Participation Type");
	constexpr Symbol kPropKeyIntensity("CinLight - Intensity");
	constexpr Symbol kPropKeyNPRBandThresholds("CinLight - NPR Band Thresholds");
	constexpr Symbol kPropKeyNPRBanding("CinLight - NPR Banding");
	constexpr Symbol kPropKeyOpacity("CinLight - Opacity");
	constexpr Symbol kPropKeyShadowIntensity("CinLight - Shadow Intensity");
	constexpr Symbol kPropKeyShadowTraceLength("CinLight - Shadow Trace Length");
	constexpr Symbol kPropKeySpecularIntensity("CinLight - Intensity Specular");
	constexpr Symbol kPropKeyWrap("CinLight - Wrap");
}

namespace CinematicLightRig {
	constexpr Symbol kPropKeyBackFOV("CinRig - Back FOV");
	constexpr Symbol kPropKeyEnlightenIntensity("CinRig - Enlighten Intensity");
	constexpr Symbol kPropKeyEnlightenSaturation("CinRig - Enlighten Saturation");
	constexpr Symbol kPropKeyLOD("CinRig - LOD");
	constexpr Symbol kPropKeyLightCinIntensity("CinRig - Light Cin Intensity");
	constexpr Symbol kPropKeyLightEnvIntensity("CinRig - Intensity EnvLight");
	constexpr Symbol kPropKeyLightNameFill("CinRig - Light Agent Fill");
	constexpr Symbol kPropKeyLightNameKey("CinRig - Light Agent Key");
	constexpr Symbol kPropKeyNameRim("CinRig - Light Agent Rim");
	constexpr Symbol kPropKeySelfShadowing("CinRig - Shadow Enable");
}

namespace ChorecordingParameters {
	constexpr Symbol kChorecordingCutName("Chorecording");
}

namespace SoundSystemInternal {
	constexpr Symbol kAudioAgentPrefix("agent_");
	constexpr Symbol kAudioLogicPrefix("logic_");
	constexpr Symbol kBusAmbient("ambient");
	constexpr Symbol kBusMaster("master");
	constexpr Symbol kBusMusic("music");
	constexpr Symbol kBusSfx("sfx");
	constexpr Symbol kVox("voice");
	constexpr Symbol kMasterBusFileName("master.audiobus");
	constexpr Symbol kSoundDataPropName("module_sound_data.prop");
}

struct Agent {

	static inline constexpr Symbol kGroupVisibilityKey = Symbol("Group - Visible");
	static inline constexpr Symbol kRuntimeVisibilityKey = Symbol("Runtime: Visible");

	char __r;

};

namespace TTSQL {
	constexpr Symbol kDatabaseNameKey("SQL Database Name");
	constexpr Symbol kDatabasePasswordKey("SQL Password");
	constexpr Symbol kDatabaseUserNameKey("SQL User Name");
	constexpr Symbol kLocalHost("localhost");
	constexpr Symbol kPortKey("SQL Port");
	constexpr Symbol kServerNameKey("SQL Server Name");
	constexpr Symbol kTunnelHost("SSH Tunnel Host");
	constexpr Symbol kTunnelPassword("SSH Tunnel Password");
	constexpr Symbol kTunneledUsername("SSH Tunnel Username");
	constexpr Symbol kTunneledServerName("SSH Tunneled Server Name");
	constexpr Symbol kTunnelingEnabled("SSH Tunneling Enabled");
	constexpr Symbol kUseLocalDataKey("SQL Use Only Local Database");
}

constexpr Symbol kPropKeyCurElemIndex("cur elem index");
constexpr Symbol kPropKeyElemData("elem data");
constexpr Symbol kPropKeyElemUses("elem uses in cycle");
constexpr Symbol kPropKeyFinalShuffe("final shuffle");
constexpr Symbol kPropKeyFinished("finished");
constexpr Symbol kPropKeyNumCompleteCycles("complete cycles");
constexpr Symbol kPropKeyVisDiesOff("vis dies off");

namespace T3MaterialInternal {
	constexpr Symbol kPropKeyExpressionTree("__T3MaterialExpressionTree__");
	constexpr Symbol kPropKeyLegacyParams("__T3LegacyMaterialParams__");
	constexpr Symbol kPropKeyMaterialData("__T3MaterialData__");
}

namespace RenderDecal {
	constexpr Symbol kPropKeyDoubleSided("Decal - Double Sided");
	constexpr Symbol kPropKeyMaterialTime("Decal - Material Time");
	constexpr Symbol kPropKeyMaterial("Decal - Material");
	constexpr Symbol kPropKeyNormalThreshold("Decal - Normal Threshold");
	constexpr Symbol kPropKeyRenderLayer("Decal - Render Layer");
	constexpr Symbol kPropKeyScale("Decal - Scale");
	constexpr Symbol kPropKeySize("Decal - Size");
	constexpr Symbol kPropKeyStatic("Decal - Static");
	constexpr Symbol kPropKeyVisibileThresholdScale("Decal - Visible Threshold Scale");
}

struct ScriptEnum {

	constexpr static const char* ENUM_LIGHT_COMPOSER_NODE_LOCATION = "ScriptEnum:LightComposerNodeLocation";
	constexpr static const char* ENUM_TEXT_COLOUR_STYLE = "ScriptEnum:TextColorStyle";
	constexpr static const char* ENUM_GAMEPAD_BUTTON = "ScriptEnum:GamepadButton";
	constexpr static const char* ENUM_LIGHT_COMPOSER_LIGHT_SOURCE_QUADRANT = "ScriptEnum:LightComposerLightSourceQuadrant";
	constexpr static const char* ENUM_LIGHT_COMPOSER_CAMERA_ZONE = "ScriptEnum:LightComposerCameraZone";
	constexpr static const char* ENUM_AI_DUMMY_POSITION = "ScriptEnum:AIDummyPos";
	constexpr static const char* ENUM_BLEND_TYPE = "ScriptEnum:BlendTypes";
	constexpr static const char* ENUM_RETICLE_ACTIONS = "ScriptEnum:ReticleActions";
	constexpr static const char* ENUM_STRUGGLE_TYPE = "ScriptEnum:StruggleType";
	constexpr static const char* ENUM_AI_PATROL_TYPE = "ScriptEnum:AIPatrolType";
	constexpr static const char* ENUM_MENU_ALIGN = "ScriptEnum:MenuAlignment";
	constexpr static const char* ENUM_MENU_VERTICAL_ALIGN = "ScriptEnum:MenuVerticalAlignment";
	constexpr static const char* ENUM_UI_COLOUR = "ScriptEnum:UIColor";
	constexpr static const char* ENUM_CHASE_FORWARD_VECTOR = "ScriptEnum:ChaseForwardVector";
	constexpr static const char* ENUM_RETICLE_DISPLAY_MODE = "ScriptEnum:ReticleDisplayMode";
	constexpr static const char* ENUM_CONTROLLER_BUTTONS = "ScriptEnum:ControllerButtons";
	constexpr static const char* ENUM_DIALOG_MODE = "ScriptEnum:DialogMode";
	constexpr static const char* ENUM_USEABLE_TYPE = "ScriptEnum:UseableType";
	constexpr static const char* ENUM_AI_AGENT_STATE = "ScriptEnum:AIAgentState";
	constexpr static const char* ENUM_QUICK_TIME_EVENT_TYPE = "ScriptEnum:QTE_Type";

	String mCurValue;
};