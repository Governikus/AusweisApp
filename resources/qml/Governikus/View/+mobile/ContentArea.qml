/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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
	readonly property var visibleItem: visibleChildren[0]
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentItem

	TabBarView {
		id: authView

		visible: baseItem.activeModule === UiModule.IDENTIFY
		anchors.fill: parent

		initialItem: AuthView {}
	}

	TabBarView {
		id: mainView

		visible: baseItem.activeModule === UiModule.DEFAULT
		anchors.fill: parent

		initialItem: MainView {}
	}

	TabBarView {
		id: selfAuthenticationView

		visible: baseItem.activeModule === UiModule.SELF_AUTHENTICATION
		anchors.fill: parent

		initialItem: SelfAuthenticationView {}
	}

	TabBarView {
		id: checkIDCardView

		visible: baseItem.activeModule === UiModule.CHECK_ID_CARD
		anchors.fill: parent

		initialItem: CheckIDCardView {}
	}

	TabBarView {
		id: smartView

		visible: baseItem.activeModule === UiModule.SMART
		anchors.fill: parent

		initialItem: SmartView {}
	}

	TabBarView {
		id: providerView

		visible: baseItem.activeModule === UiModule.PROVIDER
		anchors.fill: parent

		initialItem: ProviderView {}
	}

	TabBarView {
		id: historyView

		visible: baseItem.activeModule === UiModule.HISTORY
		anchors.fill: parent

		initialItem: HistoryView {}
	}

	TabBarView {
		id: changePinView

		visible: baseItem.activeModule === UiModule.PINMANAGEMENT
		anchors.fill: parent

		initialItem: ChangePinView {}
	}

	TabBarView {
		id: remoteView

		visible: baseItem.activeModule === UiModule.REMOTE_SERVICE
		anchors.fill: parent

		initialItem: RemoteServiceView {}
	}

	TabBarView {
		id: settingsView

		visible: baseItem.activeModule === UiModule.SETTINGS
		anchors.fill: parent

		initialItem: SettingsView {}
	}

	TabBarView {
		id: helpView

		visible: baseItem.activeModule === UiModule.HELP
		anchors.fill: parent

		initialItem: MoreView {}
	}

}
