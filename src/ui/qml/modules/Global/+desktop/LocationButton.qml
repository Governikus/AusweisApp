/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
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

	property alias image: icon.source
	property string language

	Accessible.checkable: true
	Accessible.checked: checked
	Layout.maximumWidth: Number.POSITIVE_INFINITY
	checked: SettingsModel.language === language
	padding: Constants.groupbox_spacing

	background: GPaneBackground {
		id: pane

		border.color: colors.paneBorder
		color: colors.paneBackground
		drawShadow: false

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: ColumnLayout {
		spacing: Constants.component_spacing

		Image {
			id: icon

			Layout.alignment: Qt.AlignHCenter
			sourceSize.height: Style.dimens.icon_size
		}
		GText {
			id: description

			Layout.alignment: Qt.AlignHCenter
			color: colors.textNormal
			text: root.text
		}
	}

	onClicked: SettingsModel.language = language

	StatefulColors {
		id: colors

		groupMember: true
		statefulControl: root
	}
}
