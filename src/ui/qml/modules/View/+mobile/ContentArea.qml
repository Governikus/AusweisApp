/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.CheckIDCardView
import Governikus.ChangePinView
import Governikus.MainView
import Governikus.MoreView
import Governikus.AuthView
import Governikus.SettingsView
import Governikus.SelfAuthenticationView
import Governikus.SmartView
import Governikus.RemoteServiceView
import Governikus.View
import Governikus.Type

Item {
	id: baseItem

	property int activeModule
	readonly property var currentSectionPage: if (visibleItem)
		visibleItem.currentItem
	readonly property var visibleItem: visibleChildren[0]

	TabBarView {
		id: authView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.IDENTIFY

		initialItem: AuthView {
			onShowChangePinView: {
				show(UiModule.PINMANAGEMENT);
				popAll();
			}
			onWorkflowFinished: {
				show(UiModule.DEFAULT);
				popAll();
			}
		}
	}
	TabBarView {
		id: mainView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.DEFAULT

		initialItem: MainView {
		}
	}
	TabBarView {
		id: selfAuthenticationView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.SELF_AUTHENTICATION

		initialItem: SelfAuthenticationView {
			onBack: show(UiModule.DEFAULT)
			onWorkflowFinished: pModuleToShow => {
				show(pModuleToShow);
				popAll();
			}
		}
	}
	TabBarView {
		id: checkIDCardView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.CHECK_ID_CARD

		initialItem: CheckIDCardView {
		}
	}
	TabBarView {
		id: smartView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.SMART_EID

		initialItem: SmartView {
		}
	}
	TabBarView {
		id: changePinView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.PINMANAGEMENT

		initialItem: ChangePinView {
			onClose: show(UiModule.DEFAULT)
			onWorkflowFinished: {
				show(UiModule.DEFAULT);
				popAll();
			}
		}
	}
	TabBarView {
		id: remoteView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.REMOTE_SERVICE

		initialItem: RemoteServiceView {
		}
	}
	TabBarView {
		id: settingsView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.SETTINGS

		initialItem: SettingsView {
		}
	}
	TabBarView {
		id: helpView

		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.HELP || baseItem.activeModule === UiModule.TUTORIAL

		initialItem: MoreView {
		}
	}
}
