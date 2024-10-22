/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

AbstractButton {
	id: root

	property string a11yDisabledText: qsTr("Disabled")
	property string a11yText: ""
	readonly property alias colors: colors
	property real fontScale: 1
	property bool visualPrivacy: SettingsModel.visualPrivacy

	Accessible.name: root.enabled ? (a11yText === "" ? text : a11yText) : a11yDisabledText
	Layout.maximumHeight: implicitHeight
	Layout.maximumWidth: implicitWidth
	Layout.minimumHeight: 1.2 * Style.dimens.textHeadline
	Layout.minimumWidth: Layout.minimumHeight / d.aspectRatio
	implicitHeight: 2 * Style.dimens.textHeadline
	implicitWidth: implicitHeight / d.aspectRatio

	background: GPaneBackground {
		border.color: colors.paneBorder
		color: colors.paneBackground
		drawShadow: false
		radius: Style.dimens.control_radius

		TintableIcon {
			anchors.centerIn: parent
			source: root.icon.source
			sourceSize.height: Math.min(0.8 * root.height, 1.5 * Style.dimens.textHeadline)
			tintColor: contentItem.color
			visible: !contentItem.visible
		}
		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: GText {
		Accessible.ignored: true
		color: colors.textHeadline
		font.pixelSize: fontScale * Style.dimens.textHeadline
		horizontalAlignment: Text.AlignHCenter
		text: root.text
		textStyle: Style.text.headline
		visible: text !== ""
	}

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		hoveredCondition: !root.visualPrivacy && hoverHandler.hovered
		paneStyle: Style.color.paneSublevel
		pressedCondition: !root.visualPrivacy && root.pressed
		statefulControl: root
	}
	QtObject {
		id: d

		readonly property real aspectRatio: 0.7
	}
}
