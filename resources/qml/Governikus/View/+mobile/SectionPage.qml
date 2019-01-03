import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

Item {
	signal firePush(var pSectionPage)
	signal fireReplace(var pSectionPage)
	signal firePop()
	signal firePopAll()

	property bool pushed: false

	property bool topLevelPage: false

	property var leftTitleBarAction: TitleBarAction {}
	property var headerTitleBarAction: TitleBarAction {}
	property var rightTitleBarAction: Item {}
	property var subTitleBarAction: Item {}

	property color titleBarColor: Constants.blue
	/* if the header component has a property named titleBarOpacity, use it, otherwise use default value*/
	readonly property real titleBarOpacity: header !== null && typeof(header.titleBarOpacity) != "undefined" ? header.titleBarOpacity : 1

	property QtObject header: null
	property QtObject content: null
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
		maximumFlickVelocity: Constants.scrolling_speed
		/* if a header is set, it is shown as background of the TitleBar, so we need to expand the height*/
		height: header !== null ? parent.height + Constants.titlebar_height : parent.height
		onMovementStarted: {
			startContentY = contentY
		}
		onContentYChanged: {
			if (disableFlicking || contentY < 0) { contentY = 0 /* prevent flicking over the top */}
		}
		Column {
			id: flickableContent

			children: [header, content]
		}
	}
}
