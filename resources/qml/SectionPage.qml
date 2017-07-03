import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "global"

Item {
	property TitleBarMenuAction leftTitleBarAction: TitleBarMenuAction {}
	property TitleBarAction headerTitleBarAction: TitleBarAction {}
	property Item rightTitleBarAction: Item {}

	property color titleBarColor: Constants.blue
	/* if the header component has a property named titleBarOpacity, use it, otherwise use default value*/
	readonly property real titleBarOpacity: headerLoader.item && typeof(headerLoader.item.titleBarOpacity) != "undefined" ? headerLoader.item.titleBarOpacity : 1

	property alias header: headerLoader.sourceComponent
	property alias content: contentLoader.sourceComponent
	property alias contentY: flickable.contentY
	property bool disableFlicking: false

	Flickable {
		property real startContentY: 0
		id: flickable
		clip: true
		flickableDirection: Flickable.VerticalFlick
		contentWidth: flickableContent.width
		contentHeight: flickableContent.height
		anchors.bottom: parent.bottom
		width: parent.width
		/* if a header is set, it is shown as background of the TitleBar, so we need to expand the height*/
		height: headerLoader.item ? parent.height + Constants.titlebar_height : parent.height
		onMovementStarted: {
			startContentY = contentY
		}
		onContentYChanged: {
			if (disableFlicking || contentY < 0) { contentY = 0 /* prevent flicking over the top */}

			if (!headerLoader.item || typeof(headerLoader.item.stopFlickOn) === "undefined") {
				return
			}
			if (startContentY > headerLoader.item.stopFlickOn && contentY < headerLoader.item.stopFlickOn) {
				cancelFlick()
				contentY = headerLoader.item.stopFlickOn
			}
			if (startContentY < headerLoader.item.stopFlickOn && contentY > headerLoader.item.stopFlickOn) {
				cancelFlick()
				contentY = headerLoader.item.stopFlickOn
			}
		}
		Column {
			id: flickableContent
			Loader {
				id: headerLoader
				readonly property alias contentY: flickable.contentY
			}
			Loader {
				id: contentLoader
				readonly property alias contentY: flickable.contentY
			}
		}
	}
}
