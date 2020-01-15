/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.ChangePinView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.TutorialView 1.0
import Governikus.MoreView 1.0
import Governikus.DeveloperView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.HistoryView 1.0
import Governikus.View 1.0
import Governikus.SettingsView 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	property bool ready: SettingsModel.showSetupAssistantOnStart ? tutorialView.ready : identifyView.ready
	readonly property var visibleItem: visibleChildren[0]
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage

	TabBarView {
		id: identifyView

		visible: baseItem.state === "identify"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: IdentifyView {}
	}

	TabBarView {
		visible: baseItem.state === "provider"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: ProviderView {}
	}

	TabBarView {
		visible: baseItem.state === "history"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: HistoryView {}
	}

	TabBarView {
		id: pinView

		visible: baseItem.state === "pin"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: ChangePinView {}
	}

	TabBarView {
		visible: baseItem.state === "remoteservice"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: RemoteServiceView {
			width: baseItem.width
			height: baseItem.height
		}
	}

	TabBarView {
		id: tutorialView

		visible: baseItem.state === "tutorial"
		anchors.fill: parent

		sourceComponent: TutorialView {}
	}

	TabBarView {
		visible: baseItem.state === "feedback"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: MoreView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "settings"
		prefetch: baseItem.ready
		sourceComponent: SettingsView {}
	}

	TabBarView {
		visible: baseItem.state === "developeroptions"
		anchors.fill: parent

		prefetch: baseItem.ready
		sourceComponent: DeveloperView {}
	}
}
