/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

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

	Row {
		id: layout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.groupbox_spacing

		TintableIcon {
			id: image

			sourceSize.height: ApplicationModel.scaleFactor * 40
			sourceSize.width: ApplicationModel.scaleFactor * 40
			tintColor: text.textStyle.textColor
		}

		GText {
			id: text

			width: parent.width - image.width

			linkColor: color
		}
	}

	FocusFrame {
		framee: layout
	}
}
