import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Type.ApplicationModel 1.0


Rectangle {
	id: root
	property alias title: titleText.text
	default property alias paneChildren: paneContent.children

	anchors.left: parent.left
	anchors.right: parent.right
	height: childrenRect.height
	color: Constants.white
	radius: ApplicationModel.scaleFactor * 10

	Column {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding
		topPadding: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing

		PaneTitle {
			id: titleText
		}

		Column {
			id: paneContent
			width: parent.width
			spacing: Constants.pane_spacing
		}
	}

	layer.enabled: true
	layer.effect: DropShadow {
					radius: 8
					samples: 8
					source: root
					color: Qt.darker(Constants.grey, 1.2)
					verticalOffset: 2
				}
}
