/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Style 1.0
import Governikus.View 1.0

Item {
	property alias label: labelText.text
	property alias labelStyle: labelText.textStyle
	property alias text: bodyText.text
	property alias textFormat: bodyText.textFormat
	property alias textUppercase: bodyText.font.capitalization
	property alias maximumBodyLineCount: bodyText.maximumLineCount
	property alias bodyElide: bodyText.elide
	readonly property double focusFrameMargins: focusFrame.anchors.margins

	signal linkActivated(string link)

	Accessible.role: Accessible.StaticText
	Accessible.name: labelText.text + d.effectiveSeparator + bodyText.text

	implicitHeight: column.height
	implicitWidth: Math.max(labelText.implicitWidth, bodyText.implicitWidth)
	activeFocusOnTab: true

	QtObject {
		id: d

		readonly property string accessibleSeparator: labelText.text.trim().endsWith(":") ? " " : ": "
		readonly property string effectiveSeparator: labelText.visible && bodyText.visible ? accessibleSeparator : ""
	}

	FocusFrame {
		id: focusFrame
		borderColor: Style.color.focus_indicator
	}

	Column {
		id: column

		anchors {
			left: parent.left
			right: parent.right
		}

		GText {
			id: labelText

			Accessible.ignored: true

			visible: !!text
			width: parent.width

			textStyle: Style.text.normal_accent
		}

		GText {
			id: bodyText

			Accessible.ignored: true

			visible: !!text
			width: parent.width

			textStyle: Constants.is_desktop ? Style.text.normal : Style.text.normal_secondary
			onLinkActivated: parent.linkActivated(link)
		}
	}
}
