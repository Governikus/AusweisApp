/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type
import Governikus.View

Item {
	id: root

	property int activeModule
	readonly property var currentSectionPage: if (visibleItem)
		visibleItem.currentItem
	readonly property var visibleItem: visibleChildren.filter(item => item instanceof TabBarView)[0]

	TabBarView {
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
		anchors.fill: parent
		clip: true
		visible: root.activeModule === UiModule.DEFAULT

		initialItem: MainView {
			skipFocusUpdate: true
		}
	}
	TabBarView {
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
		anchors.fill: parent
		visible: root.activeModule === UiModule.REMOTE_SERVICE

		initialItem: RemoteServiceView {
			skipFocusUpdate: true
		}
	}
	TabBarView {
		anchors.fill: parent
		visible: root.activeModule === UiModule.SETTINGS

		initialItem: SettingsView {
			skipFocusUpdate: true
		}
	}
	TabBarView {
		anchors.fill: parent
		visible: root.activeModule === UiModule.HELP

		initialItem: MoreView {
			skipFocusUpdate: true

			onStartOnboarding: show(UiModule.ONBOARDING, true)
		}
	}
	TabBarView {
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
			//: MOBILE
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
