/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/engines/awan/functions.h"

namespace Engines::AlanWakesAmericanNightmare {

const std::map<std::string, Functions::NativeFunction<AlanWakesAmericanNightmare::Functions>> Functions::_functions = {
		{"MODESWITCH.HintNextVideo"                    , {nullptr                             , {kString}}},
		{"MODESWITCH.PlayVideo"                        , {nullptr                             , {kString}}},
		{"MODESWITCH.PlayVideoLooping"                 , {nullptr                             , {kString}}},
		{"MODESWITCH.PlayVideoWithDelay"               , {nullptr                             , {kString, kFloat, kFloat}}},
		{"MODESWITCH.PlayVideoWithDelayCut"            , {nullptr                             , {kString, kFloat, kFloat}}},
		{"MODESWITCH.ActivateMainMenu"                 , {nullptr                             , {kString}}},

		{"GAME.SetTaskComplete"                        , {nullptr                             , {kEntity}}},
		{"GAME.ActivateTask"                           , {&Functions::activateTask            , {kEntity}}},
		{"GAME.SetTaskCompleteDuringNextVideo"         , {nullptr                             , {kEntity}}},
		{"GAME.TeleportPlayerToWaypointDuringNextVideo", {nullptr                             , {kEntity}}},
		{"GAME.StartEpisode"                           , {nullptr                             , {kString}}},
		{"GAME.StartVideoCommentary"                   , {nullptr                             , {kString}}},
		{"GAME.StopVideoCommentary"                    , {nullptr                             , {}}},
		{"GAME.FadeInSounds"                           , {nullptr                             , {kFloat}}},
		{"GAME.FadeOutSounds"                          , {nullptr                             , {kFloat}}},
		{"GAME.EnablePlayerControls"                   , {nullptr                             , {kBool}}},
		{"GAME.SpawnBirds"                             , {nullptr                             , {kInt, kFloat, kFloat, kFloat}}},
		{"GAME.EnableBirds"                            , {nullptr                             , {kBool}}},
		{"GAME.SetSunMoonIntensityMultiplier"          , {nullptr                             , {kFloat, kFloat}}},
		{"GAME.EnableRevealCamera"                     , {nullptr                             , {kBool}}},
		{"GAME.SetTime"                                , {&Functions::setTime                 , {kInt, kInt, kFloat}}},
		{"GAME.SetSunYRotation"                        , {&Functions::setSunYRotation         , {kFloat}}},
		{"GAME.SetWind"                                , {nullptr                             , {kFloat, kFloat}}},
		{"GAME.PlayMusic"                              , {&Functions::playMusic               , {kEntity}}},
		{"GAME.PlayMusicAt"                            , {nullptr                             , {kEntity, kFloat, kFloat}}},
		{"GAME.FadeInMusic"                            , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.FadeOutMusic"                           , {nullptr                             , {kFloat}}},
		{"GAME.StopMusic"                              , {nullptr                             , {}}},
		{"GAME.PlayAmbientTrack"                       , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.FadeInAmbientTrack"                     , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.FadeOutAmbientTrack"                    , {nullptr                             , {kFloat}}},
		{"GAME.StopAmbientTrack"                       , {nullptr                             , {}}},
		{"GAME.PlayTemporaryAmbientTrack"              , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.StopTemporaryAmbientTrack"              , {nullptr                             , {kFloat}}},
		{"GAME.HasPlayerShotDuringLevel"               , {nullptr                             , {}}},
		{"GAME.HasPlayerDiedDuringLevel"               , {nullptr                             , {}}},
		{"GAME.IsAutoPlaying"                          , {nullptr                             , {}}},
		{"GAME.EnableRenderShadows"                    , {nullptr                             , {}}},
		{"GAME.EnableGodrays"                          , {nullptr                             , {}}},
		{"GAME.GetTimeHours"                           , {nullptr                             , {}}},
		{"GAME.GetTimeMinutes"                         , {nullptr                             , {}}},
		{"GAME.SetReverbPreset"                        , {nullptr                             , {kEntity}}},
		{"GAME.SetSoundPreset"                         , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.RemoveSoundPreset"                      , {nullptr                             , {kFloat}}},
		{"GAME.SetSoundPresetWithTimer"                , {nullptr                             , {kEntity, kFloat, kFloat, kFloat}}},
		{"GAME.CancelSoundPreset"                      , {nullptr                             , {kEntity, kFloat}}},
		{"GAME.SetPitchMultiplier"                     , {nullptr                             , {kFloat}}},
		{"GAME.SetTimeMultiplier"                      , {nullptr                             , {kFloat, kFloat}}},
		{"GAME.SetTemporaryTimeMultiplier"             , {nullptr                             , {kFloat, kFloat, kFloat}}},
		{"GAME.BlockAIShooting"                        , {nullptr                             , {kFloat}}},
		{"GAME.EnableRumble"                           , {nullptr                             , {kBool, kFloat, kFloat}}},
		{"GAME.SetMaximumFPS"                          , {nullptr                             , {kInt}}},
		{"GAME.SetGravityScale"                        , {nullptr                             , {kFloat}}},
		{"GAME.EnableFlareCinematicMoment"             , {nullptr                             , {kBool}}},
		{"GAME.SetAmbiance"                            , {&Functions::setAmbiance             , {kString, kFloat}}},
		{"GAME.SetAmbianceWithTimer"                   , {nullptr                             , {kString, kFloat, kFloat, kFloat}}},
		{"GAME.PlayCinematicEffect"                    , {nullptr                             , {kFloat, kFloat, kFloat}}},
		{"GAME.SetShadowReferenceObject"               , {nullptr                             , {kEntity}}},
		{"GAME.ResetShadowReferenceObject"             , {nullptr                             , {}}},
		{"GAME.SetDarkLight"                           , {nullptr                             , {kFloat, kFloat, kFloat, kFloat}}},
		{"GAME.GetBoost"                               , {nullptr                             , {}}},
		{"GAME.TornadoStart"                           , {nullptr                             , {kEntity}}},
		{"GAME.TornadoStop"                            , {nullptr                             , {}}},
		{"GAME.PropBirdsEnable"                        , {nullptr                             , {kEntity}}},
		{"GAME.PropBirdsDisable"                       , {nullptr                             , {}}},
		{"GAME.ChestFound"                             , {nullptr                             , {kInt}}},
		{"GAME.UnlockAchievement"                      , {nullptr                             , {kInt}}},
		{"GAME.UnlockAvatarItem"                       , {nullptr                             , {kInt}}},
		{"GAME.UnlockGamerPic"                         , {nullptr                             , {kInt}}},
		{"GAME.GetActiveEnemyCount"                    , {nullptr                             , {}}},
		{"GAME.GetStoryModeRound"                      , {&Functions::getStoryModeRound       , {}}},
		{"GAME.StoryModeRoundCompleted"                , {nullptr                             , {}}},
		{"GAME.TrialGameCompleted"                     , {nullptr                             , {}}},
		{"GAME.GameCompleted"                          , {nullptr                             , {}}},
		{"GAME.ArcadeCompleted"                        , {nullptr                             , {}}},
		{"GAME.ArcadeIntro"                            , {nullptr                             , {}}},
		{"GAME.ArcadeStart"                            , {nullptr                             , {kFloat}}},
		{"GAME.Difficulty"                             , {nullptr                             , {kFloat}}},
		{"GAME.EnableStorm"                            , {nullptr                             , {kBool}}},
		{"GAME.SetStormRisingEffect"                   , {nullptr                             , {kFloat, kFloat}}},
		{"GAME.HideCell"                               , {nullptr                             , {kBool, kInt, kInt}}},
		{"GAME.HideCell128"                            , {nullptr                             , {kBool, kInt, kInt}}},
		{"GAME.GetGameMode"                            , {nullptr                             , {}}},
		{"GAME.IsTrial"                                , {&Functions::isTrial                 , {}}},
		{"GAME.IsTrialModeB"                           , {&Functions::isTrialModeB            , {}}},
		{"GAME.UnlockCollectible"                      , {nullptr                             , {kEntity, kInt}}},
		{"GAME.IsCollectibleUnlocked"                  , {nullptr                             , {kEntity, kInt}}},
		{"GAME.GetUnlockedCollectibleCount"            , {nullptr                             , {kEntity}}},
		{"GAME.UnlockRadioShow"                        , {nullptr                             , {kEntity}}},
		{"GAME.UnlockManuscriptPage"                   , {nullptr                             , {kEntity}}},
		{"GAME.IsManuscriptPageUnlocked"               , {&Functions::isManuscriptPageUnlocked, {kEntity}}},
		{"GAME.GetUnlockedManuscriptPageCount"         , {nullptr                             , {}}},
		{"GAME.KillAllEnemies"                         , {nullptr                             , {}}},
		{"GAME.SetupSpawnDistances"                    , {nullptr                             , {kFloat, kFloat, kFloat, kFloat, kFloat}}},
		{"GAME.HasPlayedArcade"                        , {nullptr                             , {}}},
		{"GAME.TrialModeDeviationStarts"               , {nullptr                             , {}}},

		{"CAMERA.AttachTo"                             , {nullptr                             , {kEntity, kFloat}}},
		{"CAMERA.Detach"                               , {nullptr                             , {kFloat}}},
		{"CAMERA.LookAt"                               , {nullptr                             , {kEntity, kFloat}}},
		{"CAMERA.LookAt2"                              , {nullptr                             , {kEntity, kFloat, kFloat, kFloat, kFloat}}},
		{"CAMERA.StopLooking"                          , {nullptr                             , {kFloat}}},
		{"CAMERA.AlignTo"                              , {nullptr                             , {kEntity, kFloat}}},
		{"CAMERA.AbortAlign"                           , {nullptr                             , {}}},
		{"CAMERA.Setup"                                , {nullptr                             , {kEntity}}},
		{"CAMERA.SetupWithoutBlend"                    , {nullptr                             , {kEntity}}},
		{"CAMERA.SetFOVMultiplier"                     , {nullptr                             , {kFloat, kFloat}}},
		{"CAMERA.Shake"                                , {nullptr                             , {kFloat, kFloat}}},
		{"CAMERA.HandShake"                            , {nullptr                             , {kFloat, kFloat, kFloat}}},
		{"CAMERA.PlayPath"                             , {nullptr                             , {kEntity, kEntity, kFloat, kFloat, kBool, kBool}}},
		{"CAMERA.LookAtCharacter"                      , {nullptr                             , {kEntity, kString, kFloat}}},
		{"CAMERA.AbortLooking"                         , {nullptr                             , {}}},
		{"CAMERA.AttachToPath"                         , {nullptr                             , {kEntity, kEntity, kEntity, kFloat}}},

		{"HUD.Print"                                   , {nullptr                             , {kString}}},
		{"HUD.PrintFloat"                              , {nullptr                             , {kFloat}}},
		{"HUD.PrintInt"                                , {nullptr                             , {kInt}}},
		{"HUD.PrintInfo"                               , {nullptr                             , {kString}}},
		{"HUD.FadeInHUD"                               , {nullptr                             , {}}},
		{"HUD.FadeOutHUD"                              , {nullptr                             , {}}},
		{"HUD.AddFlash"                                , {nullptr                             , {kString, kFloat, kFloat, kFloat}}},
		{"HUD.SetFade"                                 , {nullptr                             , {kString, kFloat}}},
		{"HUD.HideMap"                                 , {nullptr                             , {kBool}}},
		{"HUD.MapZoom"                                 , {nullptr                             , {kFloat}}},
		{"HUD.EnableActiveReloadHint"                  , {nullptr                             , {kBool}}},
		{"HUD.SetGoal"                                 , {nullptr                             , {kString, kBool}}},
		{"HUD.RemoveGoal"                              , {nullptr                             , {}}},
		{"HUD.ChangeDestinationEntity"                 , {nullptr                             , {kEntity}}},
		{"HUD.SetDestinationEntity"                    , {nullptr                             , {kEntity, kBool, kBool, kBool}}},
		{"HUD.RemoveDestinationEntity"                 , {nullptr                             , {}}},
		{"HUD.AddMapEntity"                            , {nullptr                             , {kEntity, kString, kInt, kBool, kBool}}},
		{"HUD.AddMapEntity2"                           , {nullptr                             , {kEntity, kString, kInt, kBool, kBool, kBool, kBool}}},
		{"HUD.RemoveMapEntity"                         , {nullptr                             , {kEntity}}},
		{"HUD.ShowTimedCinematicPlaceholderText"       , {nullptr                             , {kString, kFloat}}},
		{"HUD.DrawProtoBar"                            , {nullptr                             , {kBool}}},
		{"HUD.SetProtoBarFillRate"                     , {nullptr                             , {kFloat}}},
		{"HUD.ShowIcon"                                , {nullptr                             , {kInt, kInt, kInt}}},
		{"HUD.HideIcon"                                , {nullptr                             , {kInt}}},
		{"HUD.Hide"                                    , {nullptr                             , {kBool}}},

		{"LockDoor"                                    , {nullptr                             , {kBool}}},
		{"Animate"                                     , {&Functions::animate                 , {}}},
		{"EnablePointLight"                            , {&Functions::enablePointLight        , {kBool}}},
		{"EnableTrigger"                               , {nullptr                             , {kBool}}},
		{"StartMinigame"                               , {nullptr                             , {kEntity}}},
		{"StopMinigame"                                , {nullptr                             , {}}},
		{"StartTutorial"                               , {nullptr                             , {kString}}},
		{"StartCustomTutorial"                         , {nullptr                             , {kString, kBool, kBool}}},
		{"StartCustomTutorialWithSkipButton"           , {nullptr                             , {}}},
		{"StopCustomTutorial"                          , {nullptr                             , {kString}}},
		{"StopCustomTutorialWithSkipButton"            , {nullptr                             , {}}},
		{"StopTutorialByName"                          , {nullptr                             , {kString}}},
		{"StartPictureTutorial"                        , {nullptr                             , {kString}}},
		{"StartPictureTutorialWithExitButton"          , {nullptr                             , {kString}}},
		{"StopPictureTutorial"                         , {nullptr                             , {}}},
		{"SetUseInstruction"                           , {nullptr                             , {kString}}},
		{"ResetUseInstruction"                         , {nullptr                             , {}}},
		{"TriggerSayLine"                              , {nullptr                             , {kEntity, kInt}}},
		{"TriggerCancelLine"                           , {nullptr                             , {kEntity}}},
		{"TriggerCanceAllLines"                        , {nullptr                             , {}}},
		{"TriggerAbortLine"                            , {nullptr                             , {kEntity}}},
		{"TriggerAbortAllLines"                        , {nullptr                             , {}}},
		{"PlayRadio"                                   , {nullptr                             , {kEntity}}},
		{"GetDistanceToPlayer"                         , {nullptr                             , {}}},
		{"StartTimer"                                  , {&Functions::startTimer              , {kInt, kFloat, kInt}}},
		{"StartTimerWithDuration"                      , {&Functions::startTimerWithDuration  , {kInt, kFloat, kInt, kFloat}}},
		{"AbortTimer"                                  , {nullptr                             , {}}},
		{"PauseTimer"                                  , {nullptr                             , {kString}}},
		{"ResumeTimer"                                 , {nullptr                             , {}}},
		{"Hide"                                        , {&Functions::hide                    , {kBool}}},
		{"EnableEnergyThreshold"                       , {nullptr                             , {kBool}}},
		{"EnableSingleEnergyThreshold"                 , {nullptr                             , {kFloat, kBool}}},
		{"AlertAsPlayerPosition"                       , {nullptr                             , {kFloat}}},
		{"TurnAllMotorsOn"                             , {nullptr                             , {kBool}}},
		{"SetMotorParamsForAllMotors"                  , {nullptr                             , {kFloat, kFloat}}},
		{"EnableVelocityTrackingForVectorBlur"         , {nullptr                             , {kBool}}},
		{"MakeDynamic"                                 , {nullptr                             , {}}},
		{"MakeDynamicAtAnimationEnd"                   , {nullptr                             , {}}},
		{"EnableShadeEffect"                           , {nullptr                             , {kBool}}},
		{"EnableShadeEffectWithPrimitiveMask"          , {nullptr                             , {kBool, kInt}}},
		{"ShowShaderTweakables"                        , {nullptr                             , {kBool, kInt}}},
		{"CreatePressureWave"                          , {nullptr                             , {kFloat, kFloat, kFloat, kBool}}},
		{"CreateExplosionPressureWave"                 , {nullptr                             , {kFloat, kFloat, kFloat, kBool}}},
		{"PlayParticleSystem"                          , {nullptr                             , {kEntity}}},
		{"PlayParticleFloatingSystem"                  , {nullptr                             , {kEntity}}},
		{"StopParticleSystem"                          , {nullptr                             , {kEntity}}},
		{"StopAllParticleSystems"                      , {nullptr                             , {}}},
		{"FadeInSound"                                 , {nullptr                             , {kEntity, kFloat}}},
		{"FadeOutSound"                                , {nullptr                             , {kEntity, kFloat}}},
		{"CharacterAnimate"                            , {nullptr                             , {kEntity, kBool, kBool}}},
		{"PlaySound"                                   , {nullptr                             , {kEntity}}},
		{"PlaySoundWithoutOcclusion"                   , {nullptr                             , {kEntity}}},
		{"StopSound"                                   , {nullptr                             , {kEntity}}},
		{"StopAllSounds"                               , {nullptr                             , {}}},
		{"CreateThread"                                , {nullptr                             , {kFloat, kFloat}}},
		{"Poltergeist"                                 , {nullptr                             , {kBool}}},
		{"PoltergeistTarget"                           , {nullptr                             , {kBool}}},
		{"KillPoltergeist"                             , {nullptr                             , {}}},
		{"StrongShield"                                , {nullptr                             , {kBool}}},
		{"EnableAutoAim"                               , {nullptr                             , {kBool}}},
		{"Poltergeist"                                 , {nullptr                             , {kBool}}},
		{"DisablePoltergeistAchievement"               , {nullptr                             , {kBool}}},
		{"SetShieldStrength"                           , {nullptr                             , {kFloat}}},
		{"SetShadowShieldVisualStrength"               , {nullptr                             , {kFloat}}},
		{"SetShake"                                    , {nullptr                             , {kFloat, kFloat}}},
		{"PlayerControlledTurret"                      , {nullptr                             , {kBool}}},
		{"LaunchBearTrap"                              , {nullptr                             , {}}},
		{"DeactivateBearTrap"                          , {nullptr                             , {}}},
		{"EnablePickup"                                , {nullptr                             , {kBool}}},
		{"SetUnlimitedPickup"                          , {nullptr                             , {kBool}}},

		{"AI_SetDestination"                           , {nullptr                             , {kEntity}}},
		{"AI_UnsetDestination"                         , {nullptr                             , {}}},
		{"AI_ClearCommands"                            , {nullptr                             , {}}},
		{"AI_SnapClearCommands"                        , {nullptr                             , {}}},
		{"AI_SetProperty"                              , {nullptr                             , {kString, kString}}},
		{"AI_SetProfile"                               , {nullptr                             , {}}},
		{"AI_SetDisableEvent"                          , {nullptr                             , {kString}}},
		{"AI_SetEnableEvent"                           , {nullptr                             , {kString}}},
		{"AI_Add_Walk"                                 , {nullptr                             , {kEntity}}},
		{"AI_Add_Run"                                  , {nullptr                             , {kEntity}}},
		{"AI_Add_Teleflank"                            , {nullptr                             , {kEntity}}},
		{"AI_Add_DirectMove"                           , {nullptr                             , {kEntity}}},
		{"AI_Add_Turn"                                 , {nullptr                             , {kEntity}}},
		{"AI_Add_Wait"                                 , {nullptr                             , {kFloat}}},
		{"AI_Add_DirectMove"                           , {nullptr                             , {kEntity}}},
		{"AI_Add_SetProperty"                          , {nullptr                             , {kString, kString}}},
		{"AI_Add_SetProfile"                           , {nullptr                             , {kString}}},
		{"AI_Add_Animate"                              , {&Functions::aiAddAnimate            , {kEntity}}},
		{"AI_Add_AnimateLooping"                       , {&Functions::aiAddAnimateLooping     , {kEntity}}},
		{"AI_Add_Ambush"                               , {nullptr                             , {kFloat, kFloat, kBool}}},
		{"AI_Add_Throw"                                , {nullptr                             , {}}},
		{"AI_Add_Sense"                                , {nullptr                             , {kEntity}}},
		{"AI_Add_SenseAll"                             , {nullptr                             , {kEntity}}},
		{"AI_Add_Use"                                  , {nullptr                             , {}}},
		{"AI_Set_Augment_TurnHead"                     , {nullptr                             , {kEntity}}},
		{"AI_Set_Augment_TurnBody"                     , {nullptr                             , {kEntity}}},
		{"AI_Set_Augment_AimLight"                     , {nullptr                             , {kEntity}}},
		{"AI_Set_Augment_AimGun"                       , {nullptr                             , {kEntity}}},
		{"AI_Set_Augment_Attack"                       , {nullptr                             , {kEntity}}},
		{"AI_Set_Augment_AttackEnemies"                , {nullptr                             , {}}},
		{"AI_Set_Augment_None"                         , {nullptr                             , {}}},
		{"AI_Add_Augment_TurnHead"                     , {nullptr                             , {kEntity}}},
		{"AI_Add_Augment_TurnBody"                     , {nullptr                             , {kEntity}}},
		{"AI_Add_Augment_AimLight"                     , {nullptr                             , {kEntity}}},
		{"AI_Add_Augment_AimGun"                       , {nullptr                             , {kEntity}}},
		{"AI_Add_Augment_Attack"                       , {nullptr                             , {kEntity}}},
		{"AI_Add_Augment_AttackEnemies"                , {nullptr                             , {}}},
		{"AI_Add_Augment_None"                         , {nullptr                             , {}}},
		{"AI_Add_SendCustomEvent"                      , {nullptr                             , {kString}}},
		{"AI_Push_Walk"                                , {nullptr                             , {kEntity}}},
		{"AI_Push_Run"                                 , {nullptr                             , {kEntity}}},
		{"AI_Push_Teleflank"                           , {nullptr                             , {kEntity}}},
		{"AI_Push_DirectMove"                          , {nullptr                             , {kEntity}}},
		{"AI_Push_Turn"                                , {nullptr                             , {kEntity}}},
		{"AI_Push_Wait"                                , {nullptr                             , {kEntity}}},
		{"AI_Push_SetProperty"                         , {nullptr                             , {kEntity, kEntity}}},
		{"AI_Push_SetProfile"                          , {nullptr                             , {kEntity}}},
		{"AI_Push_Animate"                             , {nullptr                             , {kEntity}}},
		{"AI_Push_AnimateLooping"                      , {nullptr                             , {kEntity}}},
		{"AI_Push_Ambush"                              , {nullptr                             , {kFloat, kFloat, kBool}}},
		{"AI_Push_Throw"                               , {nullptr                             , {}}},
		{"AI_Push_Sense"                               , {nullptr                             , {kEntity}}},
		{"AI_Push_SenseAll"                            , {nullptr                             , {}}},
		{"AI_Push_Use"                                 , {nullptr                             , {kEntity}}},
		{"AI_Push_AnimateUpperBody"                    , {nullptr                             , {kEntity}}},
		{"AI_Push_AnimateUpperBodyLooping"             , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_TurnHead"                    , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_TurnBody"                    , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_AimLight"                    , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_AimGun"                      , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_Attack"                      , {nullptr                             , {kEntity}}},
		{"AI_Push_Augment_AttackEnemies"               , {nullptr                             , {}}},
		{"AI_Push_Augment_None"                        , {nullptr                             , {}}},
		{"AI_Push_Augment_TurnBody"                    , {nullptr                             , {kEntity}}},
		{"AI_Behavior_Follow"                          , {nullptr                             , {kEntity}}},
		{"AI_Behavior_StayPut"                         , {nullptr                             , {}}},
		{"AI_Behavior_Guide"                           , {nullptr                             , {kEntity, kEntity}}},
		{"AI_Behavior_Wander"                          , {nullptr                             , {kEntity, kFloat}}},
		{"AI_Behavior_GuardArea"                       , {nullptr                             , {kEntity, kFloat}}},
		{"AI_CombatBehavior_Flee"                      , {nullptr                             , {}}},
		{"AI_CombatBehavior_Assault"                   , {nullptr                             , {}}},


};

}

