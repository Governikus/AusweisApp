/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style
import Governikus.View

Item {
	property int alignment: Text.AlignLeft
	property alias bodyElide: bodyText.elide
	readonly property double focusFrameMargins: focusFrame.anchors.margins
	property alias label: labelText.text
	property alias labelStyle: labelText.textStyle
	property alias maximumBodyLineCount: bodyText.maximumLineCount
	property alias text: bodyText.text
	property alias textFormat: bodyText.textFormat
	property alias textUppercase: bodyText.font.capitalization

	signal linkActivated(string link)

	Accessible.name: labelText.text + d.effectiveSeparator + bodyText.text
	Accessible.role: Accessible.StaticText
	activeFocusOnTab: true
	implicitHeight: column.height
	implicitWidth: Math.max(labelText.implicitWidth, bodyText.implicitWidth)

	QtObject {
		id: d

		readonly property string accessibleSeparator: labelText.text.trim().endsWith(":") ? " " : ": "
		readonly property string effectiveSeparator: labelText.visible && bodyText.visible ? accessibleSeparator : ""
	}
	FocusFrame {
		id: focusFrame

	}
	Column {
		id: column

		spacing: Constants.subtext_spacing

		anchors {
			left: parent.left
			right: parent.right
		}
		GText {
			id: labelText

			Accessible.ignored: true
			horizontalAlignment: alignment
			textStyle: Style.text.subline
			visible: !!text
			width: parent.width
		}
		GText {
			id: bodyText

			Accessible.ignored: true
			horizontalAlignment: alignment
			visible: !!text
			width: parent.width

			onLinkActivated: parent.linkActivated(link)
		}
	}
}
