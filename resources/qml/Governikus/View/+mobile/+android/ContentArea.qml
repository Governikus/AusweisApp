import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.ChangePinView 1.0
import Governikus.InformationView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.TutorialView 1.0
import Governikus.FeedbackView 1.0
import Governikus.DeveloperView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.HistoryView 1.0
import Governikus.View 1.0

Item {
	id: baseItem
	property bool ready: settingsModel.showTutorialOnStart ? tutorialView.ready : identifyView.ready
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
		visible: baseItem.state === "history"
		prefetch: baseItem.ready
		sourceComponent: HistoryView {}
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
		visible: baseItem.state === "remoteservice"
		prefetch: baseItem.ready
		sourceComponent: RemoteServiceView {}
	}

	TabBarView {
		id: tutorialView
		anchors.fill: parent
		visible: baseItem.state === "tutorial"
		prefetch: baseItem.ready
		sourceComponent: TutorialView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "feedback"
		prefetch: baseItem.ready
		sourceComponent: Feedback {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "information"
		prefetch: baseItem.ready
		sourceComponent: Information {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "developeroptions"
		prefetch: baseItem.ready
		sourceComponent: DeveloperView {}
	}
}
