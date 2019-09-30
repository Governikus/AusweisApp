/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.View 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	id: baseItem
	signal linkActivated(string link)

	property alias label: label.text
	property alias text: body.text
	property alias textFormat: body.textFormat
	property alias textUppercase: body.font.capitalization
	property alias underlineLabel: label.font.underline
	readonly property double focusFrameMargins: focusFrame.anchors.margins

	implicitHeight: column.implicitHeight
	implicitWidth: Math.max(label.implicitWidth, body.implicitWidth)

	Accessible.role: Accessible.Section
	Accessible.name: label.text + (label.text.trim().endsWith(":") ? " " : ": ") + body.text

	FocusFrame {
		id: focusFrame
		dynamic: false
		border.color: Constants.black
	}

	Column {
		id: column

		anchors.fill: parent

		GText {
			id: label

			visible: label.text !== ""

			width: parent.width

			textStyle: Style.text.normal_accent
		}

		GText {
			id: body

			visible: body.text !== ""

			width: parent.width

			textStyle: Style.text.normal_inverse
			onLinkActivated: baseItem.linkActivated(link)
		}
	}
}
