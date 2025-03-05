/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.Global
import Governikus.SelfAuthenticationView
import Governikus.TitleBar
import Governikus.Type
import Governikus.MultiInfoView

OnboardingStartView {
	id: root

	property bool onboardingAborted: false
	property bool onboardingFailed: false

	function continueToAutostartView() {
		if (SettingsModel.autoStartAvailable && !SettingsModel.autoStartSetByAdmin) {
			root.push(setupAutostart);
			return;
		}

		root.push(setupTrayIcon);
	}
	function continueToSelfAuth() {
		if (root.onboardingAborted || root.onboardingFailed) {
			root.exitOnboarding(false);
			return;
		}
		root.push(startSelfAuth);
	}

	titleBarSettings: TitleBarSettings {
		startEnabled: false
	}

	onContinueOnboarding: push(preparationInfoView)
	onSkipOnboarding: exitOnboarding()

	ProgressTracker {
		id: progressTracker

		enabled: true
		stages: 3
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

			onAbortWithUnknownPin: {
				root.onboardingFailed = true;
				root.push(onboardingFailedView);
			}
			onContinueOnboarding: root.push(setupInfoView)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: setupInfoView

		MultiInfoView {
			//: LABEL DESKTOP
			continueButtonText: qsTr("Continue")
			title: root.title

			infoContent: MultiInfoData {
				contentType: MultiInfoData.Type.ONBOARDING_SETUP_DESKTOP
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 2
				steps: 6
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: push(chooseReaderType)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: chooseReaderType

		ChooseReaderType {
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 3
				steps: 6
			}

			onLeaveView: root.pop()
			onUseSmartPhone: root.push(pairSacStart)
			onUseUsbReader: root.push(connectUsb)
		}
	}
	Component {
		id: connectUsb

		OnboardingConnectUsbReaderView {
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 4
				steps: 6
			}

			onContinueOnboarding: root.push(confirmReaderDetected)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: confirmReaderDetected

		OnboardingConfirmStageView {
			stage: OnboardingConfirmationViewData.Stage.READER_DETECTED
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 6
				steps: 6
			}

			onContinueOnboarding: root.push(checkIDCardUsb)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: checkIDCardUsb

		OnboardingCheckIDView {
			pluginType: ReaderManagerPluginType.PCSC
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 2
				currentStep: 1
				steps: 3
			}

			onContinueOnboarding: {
				root.pop();
				root.push(confirmCheckIDWithUsb);
			}
			onLeaveView: root.pop()
			onPinDeactivated: {
				root.onboardingFailed = true;
				root.push(onboardingFailedView);
			}
		}
	}
	Component {
		id: confirmCheckIDWithUsb

		OnboardingConfirmStageView {
			stage: OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_CARD_READER
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 2
				currentStep: 3
				steps: 3
			}

			onContinueOnboarding: root.push(changePinView)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: pairSacStart

		OnboardingPairSacStartView {
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 4
				steps: 6
			}

			onContinueOnboarding: root.push(pairSac)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: pairSac

		OnboardingPairSacView {
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 5
				steps: 6
			}

			onContinueOnboarding: root.push(confirmPairingSac)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: confirmPairingSac

		OnboardingConfirmStageView {
			stage: OnboardingConfirmationViewData.Stage.PAIR_DEVICE
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 1
				currentStep: 6
				steps: 6
			}

			onContinueOnboarding: root.push(checkIDCardSaC)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: checkIDCardSaC

		OnboardingCheckIDView {
			pluginType: ReaderManagerPluginType.REMOTE_IFD
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 2
				currentStep: 1
				steps: 3
			}

			onContinueOnboarding: {
				root.pop();
				root.push(confirmCheckIDWithSac);
			}
			onLeaveView: root.pop()
			onPinDeactivated: {
				root.onboardingFailed = true;
				root.push(onboardingFailedView);
			}
		}
	}
	Component {
		id: confirmCheckIDWithSac

		OnboardingConfirmStageView {
			stage: OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 2
				currentStep: 3
				steps: 3
			}

			onContinueOnboarding: root.push(changePinView)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: changePinView

		ChangePinView {
			activateUI: false
			navigationActionType: NavigationAction.Back
			onlyCheckPin: true
			skipInfoView: false
			title: root.title
			usedInOnboarding: true

			errorViewDelegate: OnboardingPinErrorView {
				title: root.title

				progress: ProgressTracker {
					baseProgressTracker: progressTracker
					currentStage: 3
					relativeProgress: 1
				}
				titleBarSettings: TitleBarSettings {
					navigationAction: NavigationAction.Back
					navigationEnabled: false
				}

				onAbort: {
					root.onboardingAborted = true;
					ChangePinModel.continueWorkflow();
				}
				onPukBlocked: {
					root.onboardingFailed = true;
					ChangePinModel.continueWorkflow();
				}
				onTryAgain: ChangePinModel.continueWorkflow()
			}
			progress: ProgressTracker {
				baseProgressTracker: progressTracker
				currentStage: 3
				from: 0.1
			}
			successViewDelegate: OnboardingConfirmStageView {
				stage: ChangePinModel.requestTransportPin ? OnboardingConfirmationViewData.Stage.CHANGE_PIN : OnboardingConfirmationViewData.Stage.PIN_CORRECT
				title: root.title

				progress: ProgressTracker {
					baseProgressTracker: progressTracker
					currentStage: 3
					relativeProgress: 1
				}
				titleBarSettings: TitleBarSettings {
					navigationAction: NavigationAction.Back
					navigationEnabled: false
				}

				onContinueOnboarding: ChangePinModel.continueWorkflow()
				onLeaveView: ChangePinModel.continueWorkflow()
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onAbortWithUnknownPin: {
				root.onboardingFailed = true;
				root.push(onboardingFailedView);
			}
			onWorkflowFinished: pSuccess => {
				if (pSuccess || root.onboardingAborted) {
					root.continueToAutostartView();
				} else if (root.onboardingFailed) {
					root.push(onboardingFailedView);
				}
			}
		}
	}
	Component {
		id: onboardingFailedView

		OnboardingFailedView {
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Cancel
				startEnabled: false

				onNavigationActionClicked: root.continueToAutostartView()
			}

			onContinueOnboarding: root.continueToAutostartView()
		}
	}
	Component {
		id: setupAutostart

		AutostartView {
			title: root.title

			onLeaveView: root.push(setupTrayIcon)
		}
	}
	Component {
		id: setupTrayIcon

		TrayIconView {
			title: root.title

			onLeaveView: root.continueToSelfAuth()
		}
	}
	Component {
		id: startSelfAuth

		SelfAuthStartView {
			//: LABEL DESKTOP
			disagreeButtonText: qsTr("Skip this step")
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				navigationEnabled: false
				startEnabled: false
			}

			onDisagreeClicked: root.push(completionView)
			onStartSelfAuth: {
				root.push(authView);
				SelfAuthModel.startWorkflow(false);
			}
		}
	}
	Component {
		id: authView

		AuthView {
			startedByOnboarding: true

			onBackToSelfAuthView: root.pop()
			onBackToStartPage: pWorkflowSuccess => {
				root.pop();
				if (pWorkflowSuccess) {
					root.push(completionView);
				}
			}
		}
	}
	Component {
		id: completionView

		OnboardingCompletionView {
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onAgreeClicked: root.exitOnboarding()
		}
	}
}
