/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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

	GText {
		id: bodyText
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin
		onLinkActivated: parent.linkActivated(link)
	}

	GText {
		id: labelText
		anchors.top: bodyText.bottom
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin
		textStyle: Style.text.hint_accent
	}
}
