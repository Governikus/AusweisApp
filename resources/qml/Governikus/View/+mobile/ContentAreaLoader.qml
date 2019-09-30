/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0

// This proxy Component interrupts the QML parsing process by using a Loader.
// The parsing of the included file is continued in a background
// process by the loader. This way the splash screen is shown while the Loader
// parses the given source.
Item {
	id: baseItem

	function reselectedState() {
		visibleItem.popAll()
	}

	property bool ready: false
	readonly property var visibleItem: if (loader.item) loader.item.visibleChildren[0]
	readonly property var currentSectionPage: if (visibleItem) visibleItem.currentSectionPage

	Loader {
		id: loader
		anchors.fill: parent

		asynchronous: true
		sourceComponent:  ContentArea {}
		onStatusChanged: {
			if (status === Loader.Ready){
				baseItem.ready = Qt.binding(function() {return loader.item.ready})
				item.state = Qt.binding(function() {return baseItem.state})
			}
		}
	}
}
