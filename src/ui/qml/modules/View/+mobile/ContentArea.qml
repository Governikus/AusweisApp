/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.CheckIDCardView
import Governikus.MainView
import Governikus.MoreView
import Governikus.OnboardingView
import Governikus.RemoteServiceView
import Governikus.SelfAuthenticationView
import Governikus.SettingsView
import Governikus.SmartView
import Governikus.Type
import Governikus.View

Item {
	id: root

	property int activeModule
	readonly property var currentSectionPage: if (visibleItem)
		visibleItem.currentItem
	readonly property var visibleItem: visibleChildren[0]

	TabBarView {
		id: authView

		anchors.fill: parent
		visible: root.activeModule === UiModule.IDENTIFY

		initialItem: AuthView {
			onChangeTransportPin: {
				changeTransportPinController.createObject(this, {
					stackView: this.stackView
				});
			}
			onWorkflowFinished: pSuccess => {
				show(UiModule.DEFAULT);
				popAll();
			}
		}
	}
	TabBarView {
		id: mainView

		anchors.fill: parent
		visible: root.activeModule === UiModule.DEFAULT

		initialItem: MainView {
		}
	}
	TabBarView {
		id: selfAuthenticationView

		anchors.fill: parent
		visible: root.activeModule === UiModule.SELF_AUTHENTICATION

		initialItem: SelfAuthenticationView {
			onBack: show(UiModule.DEFAULT)
			onChangeTransportPin: {
				changeTransportPinController.createObject(this, {
					stackView: this.stackView
				});
			}
			onWorkflowFinished: pModuleToShow => {
				show(pModuleToShow);
				popAll();
			}
		}
	}
	TabBarView {
		id: checkIDCardView

		anchors.fill: parent
		visible: root.activeModule === UiModule.CHECK_ID_CARD

		initialItem: CheckIDCardView {
			onChangeTransportPin: {
				changeTransportPinController.createObject(this, {
					stackView: this.stackView
				});
			}
		}
	}
	TabBarView {
		id: smartView

		anchors.fill: parent
		visible: root.activeModule === UiModule.SMART_EID

		initialItem: SmartView {
		}
	}
	TabBarView {
		id: changePinView

		anchors.fill: parent
		visible: root.activeModule === UiModule.PINMANAGEMENT

		initialItem: ChangePinView {
			onClose: show(UiModule.DEFAULT)
			onWorkflowFinished: pSuccess => {
				show(UiModule.DEFAULT);
				popAll();
			}
		}
	}
	TabBarView {
		id: remoteView

		anchors.fill: parent
		visible: root.activeModule === UiModule.REMOTE_SERVICE

		initialItem: RemoteServiceView {
		}
	}
	TabBarView {
		id: settingsView

		anchors.fill: parent
		visible: root.activeModule === UiModule.SETTINGS

		initialItem: SettingsView {
		}
	}
	TabBarView {
		id: helpView

		anchors.fill: parent
		visible: root.activeModule === UiModule.HELP

		initialItem: MoreView {
			onStartOnboarding: show(UiModule.ONBOARDING, true)
		}
	}
	TabBarView {
		id: onboardingView

		anchors.fill: parent
		visible: root.activeModule === UiModule.ONBOARDING

		initialItem: OnboardingView {
			onLeaveView: {
				show(UiModule.DEFAULT);
				popAll();
			}
		}
	}
	Component {
		id: changeTransportPinController

		ChangePinController {
			//: LABEL ANDROID IOS
			title: qsTr("Change PIN")

			Component.onCompleted: ChangePinModel.startWorkflow(true, false)
			onWorkflowFinished: {
				popAll();
				show(UiModule.DEFAULT);
				setLockedAndHidden(false);
				this.destroy();
			}

			Connections {
				function onFireWorkflowStarted() {
					ChangePinModel.setInitialPluginType();
				}

				target: ChangePinModel
			}
		}
	}
}
