/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style
import Governikus.Type
import Governikus.View

Item {
	id: root

	property int alignment: Text.AlignLeft
	property alias bodyElide: bodyText.elide
	readonly property double focusFrameMargins: focusFrame.anchors.margins
	required property string label
	property alias labelColor: labelText.color
	property alias labelStyle: labelText.textStyle
	property alias maximumBodyLineCount: bodyText.maximumLineCount
	required property string text
	property alias textColor: bodyText.color
	property alias textFormat: bodyText.textFormat
	property alias textUppercase: bodyText.font.capitalization

	Accessible.name: labelText.text + d.effectiveSeparator + bodyText.text
	Accessible.role: Accessible.StaticText
	activeFocusOnTab: ApplicationModel.isScreenReaderRunning
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

		spacing: Style.dimens.subtext_spacing

		anchors {
			left: parent.left
			right: parent.right
		}
		GText {
			id: labelText

			Accessible.ignored: true
			activeFocusOnTab: false
			horizontalAlignment: root.alignment
			text: root.label
			textStyle: Style.text.subline
			visible: !!text
			width: parent.width
		}
		GText {
			id: bodyText

			Accessible.ignored: !hasLink
			activeFocusOnTab: hasLink
			horizontalAlignment: root.alignment
			text: root.text
			visible: !!text
			width: parent.width
		}
	}
}
