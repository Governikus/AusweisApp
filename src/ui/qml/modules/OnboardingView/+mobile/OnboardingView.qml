/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.CheckIDCardView
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.RemoteServiceView
import Governikus.SelfAuthenticationView
import Governikus.TitleBar
import Governikus.Type

OnboardingStartView {
	id: root

	readonly property var readerPlugin: ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE ? ReaderManagerPluginType.REMOTE_IFD : ReaderManagerPluginType.NFC

	onContinueOnboarding: ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE ? push(preparationInfoView) : push(primaryDeviceDecisionView)
	onSkipOnboarding: exitOnboarding()

	ProgressTracker {
		id: progressTracker

		enabled: true
		stages: 2
	}
	Component {
		id: primaryDeviceDecisionView

		PrimaryDeviceDecisionView {
			title: root.title

			onContinueOnboarding: push(preparationInfoView)
			onLeaveView: root.pop()
			onSetupDesktopUsage: push(setupDesktopUsage)
		}
	}
	Component {
		id: setupDesktopUsage

		SetupDesktopUsage {
			title: root.title

			onContinueToPairing: root.exitOnboarding()
			onLeaveView: root.pop()
		}
	}
	Component {
		id: preparationInfoView

		PreparationInfoView {
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 1
				steps: 6
			}

			onAbortWithUnknownPin: root.push(onboardingFailedView)
			onContinueOnboarding: root.push(multiInfoViewOnboardingSetupMobile)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: multiInfoViewOnboardingSetupMobile

		MultiInfoView {
			//: MOBILE
			continueButtonText: qsTr("Continue")
			title: root.title

			infoContent: MultiInfoData {
				contentType: MultiInfoData.Type.ONBOARDING_SETUP_MOBILE
			}
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 2
				steps: 6
			}

			onContinueClicked: {
				if (ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE) {
					root.push(noNfcSuggestion);
				} else {
					SettingsModel.preferredTechnology = ReaderManagerPluginType.NFC;
					root.push(checkIDCardWorkflow);
				}
			}
			onLeaveView: root.pop()
		}
	}
	Component {
		id: noNfcSuggestion

		CheckIDCardNoNfcSuggestion {
			title: root.title
			usedInOnboarding: true

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 3
				steps: 6
			}

			onAbortOnboarding: root.push(onboardingFailedView)
			onConnectSmartphone: root.push(remoteServiceSettings)

			Component {
				id: remoteServiceSettings

				RemoteServiceSettings {
					allowUsage: true

					progress: ProgressTracker {
						baseProgressTracker: progressTracker
						currentStage: 1
						currentStep: 3
						steps: 6
					}

					Component.onCompleted: RemoteServiceModel.startDetection()
					Component.onDestruction: RemoteServiceModel.stopDetection(true)
					onPairedDeviceFound: {
						SettingsModel.preferredTechnology = ReaderManagerPluginType.REMOTE_IFD;
						root.push(checkIDCardSaC);
					}
				}
			}
		}
	}
	Component {
		id: checkIDCardWorkflow

		CheckIDCardWorkflow {
			readerType: SettingsModel.preferredTechnology
			title: root.title
			usedInOnboarding: true

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 4
				steps: 6
			}

			onCheckSuccess: root.push(onboardingConfirmStageView)
			onLeaveView: root.pop()
			onPinDeactivated: root.push(onboardingFailedView)
			onRestartCheck: {
				root.pop();
				root.push(checkIDCardWorkflow, {
					readerType: SettingsModel.preferredTechnology
				});
			}
		}
	}
	Component {
		id: checkIDCardSaC

		OnboardingCheckIDView {
			pluginType: SettingsModel.preferredTechnology
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 4
				steps: 6
			}

			onContinueOnboarding: {
				root.pop();
				root.push(onboardingConfirmStageView);
			}
			onLeaveView: {
				let itemToPop = root.find(pItem => pItem instanceof CheckIDCardNoNfcSuggestion);
				if (itemToPop) {
					root.pop(itemToPop);
				}
			}
			onPinDeactivated: root.push(onboardingFailedView)
		}
	}
	Component {
		id: onboardingConfirmStageView

		OnboardingConfirmStageView {
			stage: OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 6
				steps: 6
			}

			onContinueOnboarding: root.push(changePinView)
			onLeaveView: {
				if (ApplicationModel.nfcState !== ApplicationModel.NfcState.UNAVAILABLE) {
					root.pop();
				} else {
					let itemToPop = root.find(pItem => pItem instanceof CheckIDCardNoNfcSuggestion);
					if (itemToPop) {
						root.pop(itemToPop);
					}
				}
			}
		}
	}
	Component {
		id: changePinView

		ChangePinView {
			id: changePinViewInstance

			property bool abortOnboarding: false
			property bool pukBlocked: false

			activateUI: false
			initialPlugin: root.readerPlugin
			navigationActionType: NavigationAction.Action.Back
			onlyCheckPin: true
			skipInfoView: false
			skipProgressView: true
			title: root.title
			usedInOnboarding: true

			cardNotActivatedDelegate: CardNotActivatedView {
				continueButtonVisible: true
				title: root.title

				navigationAction: NavigationAction {
					action: NavigationAction.Action.Back
					enabled: false
				}
				progress: ProgressTracker {
					baseProgressTracker: progressTracker
					currentStage: 2
					relativeProgress: 1
				}

				onContinueClicked: {
					ChangePinModel.continueWorkflow();
					changePinViewInstance.abortOnboarding = true;
				}
			}
			errorViewDelegate: OnboardingPinErrorView {
				title: root.title

				navigationAction: NavigationAction {
					action: NavigationAction.Action.Back
					enabled: false
				}
				progress: ProgressTracker {
					baseProgressTracker: progressTracker
					currentStage: 2
					relativeProgress: 1
				}

				onAbort: {
					ChangePinModel.continueWorkflow();
					changePinViewInstance.abortOnboarding = true;
				}
				onPukBlocked: {
					ChangePinModel.continueWorkflow();
					changePinViewInstance.pukBlocked = true;
				}
				onTryAgain: ChangePinModel.continueWorkflow()
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 2
				from: 0.1
			}
			successViewDelegate: OnboardingConfirmStageView {
				stage: ChangePinModel.requestTransportPin ? OnboardingConfirmationViewData.Stage.CHANGE_PIN : OnboardingConfirmationViewData.Stage.PIN_CORRECT
				title: root.title

				navigationAction: NavigationAction {
					action: NavigationAction.Action.Back
					enabled: false
				}
				progress: ProgressTracker {
					baseProgressTracker: progressTracker
					currentStage: 2
					relativeProgress: 1
				}

				onContinueOnboarding: ChangePinModel.continueWorkflow()
				onLeaveView: ChangePinModel.continueWorkflow()
			}

			onAbortWithUnknownPin: root.push(onboardingFailedView)
			onClose: root.pop()
			onWorkflowFinished: pSuccess => {
				if (pSuccess) {
					root.push(selfAuthStartView);
					return;
				}
				if (changePinViewInstance.abortOnboarding) {
					root.exitOnboarding(false);
				} else if (changePinViewInstance.pukBlocked) {
					root.push(onboardingFailedView);
				}
			}
		}
	}
	Component {
		id: onboardingFailedView

		OnboardingFailedView {
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: root.exitOnboarding(false)
			}

			onContinueOnboarding: root.exitOnboarding(false)
		}
	}
	Component {
		id: selfAuthStartView

		SelfAuthStartView {
			//: MOBILE
			secondaryButton.text: qsTr("Skip this step")
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back
				enabled: false
			}

			onSecondaryButtonClicked: root.push(onboardingCompletionView)
			onStartSelfAuth: {
				SelfAuthModel.startWorkflow(false);
				root.push(authView);
			}
		}
	}
	Component {
		id: authView

		AuthView {
			property bool backRequested: false

			initialPlugin: root.readerPlugin
			startedByOnboarding: true

			onRequestBack: backRequested = true
			onWorkflowFinished: pWorkflowSuccess => {
				root.pop();
				if (pWorkflowSuccess && !backRequested) {
					root.push(onboardingCompletionView);
				}
			}
		}
	}
	Component {
		id: onboardingCompletionView

		OnboardingCompletionView {
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onPrimaryButtonClicked: root.exitOnboarding()
		}
	}
}
