/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

Item {
	property alias label: labelText.text
	property alias text: bodyText.text
	property alias textFormat: bodyText.textFormat
	property int margin

	signal linkActivated(string link)

	height: childrenRect.height + margin

	Accessible.role: Accessible.StaticText
	Accessible.name: label + ": " + text

	GText {
		id: labelText

		Accessible.ignored: true

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin

		textStyle: Style.text.normal_accent
	}

	GText {
		id: bodyText

		Accessible.ignored: true

		anchors.top: labelText.bottom
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin
		textStyle: Style.text.normal_secondary
		onLinkActivated: parent.linkActivated(link)
	}
}
