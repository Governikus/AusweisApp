/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

Rectangle {
	id: root

	property bool boldFont: false
	required property int index
	required property string message
	required property string modelData
	required property string origin

	Accessible.focusable: true
	Accessible.name: modelData
	Accessible.role: Utils.useSpecialAppleTabRole(Accessible.ListItem)
	color: colors.paneBackground
	implicitHeight: content.implicitHeight + content.anchors.topMargin + content.anchors.bottomMargin
	implicitWidth: content.implicitWidth

	GSeparator {
		visible: root.index !== 0

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	ColumnLayout {
		id: content

		spacing: 0

		anchors {
			bottomMargin: Style.dimens.text_spacing / 2
			fill: parent
			leftMargin: Style.dimens.pane_padding
			rightMargin: anchors.leftMargin
			topMargin: anchors.bottomMargin
		}
		LogText {
			color: Style.color.textSubline.basic_unchecked
			maximumLineCount: 8
			text: root.origin
		}
		LogText {
			maximumLineCount: 64
			text: root.message
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent
		hoverEnabled: UiPluginModel.isChromeOS

		onPressAndHold: {
			ApplicationModel.setClipboardText(root.modelData);
			//: MOBILE Toast message used to confirm the copy of a log entry.
			ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
		}
	}
	StatefulColors {
		id: colors

		checkedCondition: false
		hoveredCondition: mouseArea.containsMouse
		statefulControl: mouseArea
	}

	component LogText: GText {
		Accessible.ignored: true
		font.family: UiPluginModel.fixedFontFamily
		font.weight: root.boldFont ? Style.font.bold : Style.font.normal
	}
}
