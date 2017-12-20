import QtQuick 2.5

import Governikus.Global 1.0
import Governikus.PinView 1.0
import Governikus.InformationView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.FeedbackView 1.0
import Governikus.DeveloperView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ProviderView 1.0
import Governikus.HistoryView 1.0

Item {
	id: baseItem
	property alias ready: identifyView.ready
	readonly property var visibleItem: visibleChildren[0]
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage

	TabBarView {
		id: identifyView
		anchors.fill: parent
		visible: baseItem.state === "identify"
		sourceComponent: IdentifyView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "provider"
		prefetch: identifyView.ready
		sourceComponent: ProviderView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "history"
		prefetch: identifyView.ready
		sourceComponent: HistoryView {}
	}

	TabBarView {
		id: pinView
		anchors.fill: parent
		visible: baseItem.state === "pin"
		prefetch: identifyView.ready
		sourceComponent: PinView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "remoteservice"
		prefetch: identifyView.ready
		sourceComponent: RemoteServiceView {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "feedback"
		prefetch: identifyView.ready
		sourceComponent: Feedback {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "information"
		prefetch: identifyView.ready
		sourceComponent: Information {}
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "developeroptions"
		prefetch: identifyView.ready
		sourceComponent: DeveloperView {}
	}
}
