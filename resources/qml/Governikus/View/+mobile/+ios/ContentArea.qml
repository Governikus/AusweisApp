/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.ChangePinView 1.0
import Governikus.MoreView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.TutorialView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.View 1.0

Item {
	id: baseItem
	property bool ready: SettingsModel.showSetupAssistantOnStart ? tutorialView.ready : identifyView.ready
	readonly property var visibleItem: visibleChildren[0]
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage

	TabBarView {
		id: identifyView
		anchors.fill: parent
		visible: baseItem.state === "identify"
		prefetch: baseItem.ready
		sourceComponent: IdentifyView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "provider"
		prefetch: baseItem.ready
		sourceComponent: ProviderView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "remoteservice"
		prefetch: baseItem.ready
		sourceComponent: RemoteServiceView {
			width: baseItem.width
			height: baseItem.height
		}
	}

	TabBarView {
		id: pinView
		anchors.fill: parent
		visible: baseItem.state === "pin"
		prefetch: baseItem.ready
		sourceComponent: ChangePinView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "more"
		prefetch: baseItem.ready
		sourceComponent: MoreView {}
	}

	TabBarView {
		id: tutorialView
		visible: baseItem.state === "tutorial"
		anchors.fill: parent
		sourceComponent: TutorialView {}
	}
}
