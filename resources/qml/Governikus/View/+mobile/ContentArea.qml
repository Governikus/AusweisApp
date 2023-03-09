/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.CheckIDCardView 1.0
import Governikus.ChangePinView 1.0
import Governikus.HistoryView 1.0
import Governikus.MainView 1.0
import Governikus.MoreView 1.0
import Governikus.AuthView 1.0
import Governikus.ProviderView 1.0
import Governikus.SettingsView 1.0
import Governikus.SelfAuthenticationView 1.0
import Governikus.SmartView 1.0
import Governikus.TutorialView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

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
		visible: baseItem.activeModule === UiModule.SMART

		initialItem: SmartView {
		}
	}
	TabBarView {
		id: providerView
		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.PROVIDER

		initialItem: ProviderView {
		}
	}
	TabBarView {
		id: historyView
		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.HISTORY

		initialItem: HistoryView {
		}
	}
	TabBarView {
		id: changePinView
		anchors.fill: parent
		visible: baseItem.activeModule === UiModule.PINMANAGEMENT

		initialItem: ChangePinView {
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
		visible: baseItem.activeModule === UiModule.HELP

		initialItem: MoreView {
		}
	}
}
