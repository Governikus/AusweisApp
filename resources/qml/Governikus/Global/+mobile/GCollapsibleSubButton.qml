/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

AbstractButton {
	id: root

	property alias image: icon.source
	property alias tintIcon: icon.tintEnabled
	property alias title: label.text

	implicitHeight: layout.implicitHeight + Constants.component_spacing
	implicitWidth: layout.implicitWidth + 2 * Constants.component_spacing

	background: Rectangle {
		color: root.pressed ? Style.color.control_content_pressed : Style.color.transparent
	}
	contentItem: RowLayout {
		id: layout

		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing * 2
		anchors.verticalCenter: parent.verticalCenter
		spacing: Constants.component_spacing

		TintableIcon {
			id: icon

			sourceSize.height: Style.dimens.small_icon_size
			tintColor: label.color
			tintEnabled: false
		}
		GText {
			id: label

			Layout.maximumWidth: Number.POSITIVE_INFINITY
			color: root.pressed ? Style.color.control_content_hover : Style.color.text
		}
	}
}
