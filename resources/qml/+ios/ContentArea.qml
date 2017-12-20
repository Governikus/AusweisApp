import QtQuick 2.5

import Governikus.Global 1.0
import Governikus.PinView 1.0
import Governikus.MoreView 1.0
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
		visible: baseItem.state === "more"
		prefetch: identifyView.ready
		sourceComponent: MoreView {}
	}

}
