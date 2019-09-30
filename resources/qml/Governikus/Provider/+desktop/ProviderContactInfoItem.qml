/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	id: baseItem

	property alias imageSource: image.source
	property alias itemText: text.text
	property string accessibleText
	property url link
	property string label

	implicitHeight: layout.height

	Accessible.name: label
	Accessible.description: accessibleText
	Accessible.role: Accessible.ListItem

	activeFocusOnTab: true

	RowLayout {
		id: layout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.groupbox_spacing

		Image {
			id: image

			sourceSize.height: ApplicationModel.scaleFactor * 40
			sourceSize.width: ApplicationModel.scaleFactor * 40
			fillMode: Image.PreserveAspectFit
		}

		GText {
			id: text

			Layout.fillWidth: true

			linkColor: color

			onLinkActivated: Qt.openUrlExternally(link)
		}
	}

	FocusFrame {
		dynamic: false
		framee: layout
	}
}
