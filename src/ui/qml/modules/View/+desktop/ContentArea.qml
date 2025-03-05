/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.MainView
import Governikus.MoreView
import Governikus.SelfAuthenticationView
import Governikus.SettingsView
import Governikus.OnboardingView
import Governikus.Type
import Governikus.UpdateView

StackView {
	id: root

	readonly property int activeModule: depth === 1 ? UiModule.DEFAULT : requestedModule
	property int requestedModule

	function setUiModule(pModule) {
		if (pModule === activeModule || pModule === UiModule.CURRENT) {
			return;
		}
		pop(null);
		switch (pModule) {
		case UiModule.DEFAULT:
			break;
		case UiModule.PINMANAGEMENT:
			push(changePinView);
			break;
		case UiModule.SELF_AUTHENTICATION:
			push(selfAuthView);
			break;
		case UiModule.SETTINGS:
			push(settingsView);
			break;
		case UiModule.HELP:
			push(moreView);
			break;
		case UiModule.IDENTIFY:
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE) {
				push(selfAuthView);
			}
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.AUTHENTICATION || ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
				push(authView);
			}
			break;
		case UiModule.ONBOARDING:
			push(onboarding);
			break;
		case UiModule.UPDATEINFORMATION:
			push(updateView);
			break;
		default:
			console.warn("No suitable Component for UiModule request", pModule);
			return;
		}
		requestedModule = pModule;
	}

	Accessible.ignored: Qt.platform.os === "windows"
	popEnter: null
	popExit: null
	pushEnter: null
	pushExit: null
	replaceEnter: null
	replaceExit: null

	initialItem: MainView {
		onRequestUiModule: pModule => root.setUiModule(pModule)
	}

	Component {
		id: changePinView

		ChangePinView {
			onLeaveView: root.setUiModule(UiModule.DEFAULT)
		}
	}
	Component {
		id: selfAuthView

		SelfAuthenticationView {
		}
	}
	Component {
		id: settingsView

		SettingsView {
		}
	}
	Component {
		id: moreView

		MoreView {
			onRequestUiModule: pModule => root.setUiModule(pModule)
		}
	}
	Component {
		id: authView

		AuthView {
			onBackToSelfAuthView: root.setUiModule(UiModule.SELF_AUTHENTICATION)
			onBackToStartPage: root.setUiModule(UiModule.DEFAULT)
			onChangeTransportPin: push(changeTransportPinController)

			Component {
				id: changeTransportPinController

				ChangePinController {
					Component.onCompleted: ChangePinModel.startWorkflow(true, false)
					onWorkflowFinished: root.setUiModule(UiModule.DEFAULT)
				}
			}
		}
	}
	Component {
		id: onboarding

		OnboardingView {
			onLeaveView: root.setUiModule(UiModule.DEFAULT)
		}
	}
	Component {
		id: updateView

		UpdateView {
			onLeaveView: root.setUiModule(UiModule.DEFAULT)
		}
	}
}
