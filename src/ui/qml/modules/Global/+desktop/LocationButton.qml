/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
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

	required property string a11yDescription
	required property string a11yName
	required property url image
	required property string language
	required property string languageText

	Accessible.checkable: true
	Accessible.checked: checked
	Accessible.description: a11yDescription
	Accessible.name: a11yName
	Layout.maximumWidth: Number.POSITIVE_INFINITY
	checked: SettingsModel.language === language
	padding: Style.dimens.groupbox_spacing

	background: GPaneBackground {
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
		spacing: Style.dimens.pane_spacing

		Image {
			Layout.alignment: Qt.AlignHCenter
			source: root.image
			sourceSize.height: Style.dimens.icon_size
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			activeFocusOnTab: false
			color: colors.textNormal
			text: root.languageText
		}
	}

	onClicked: SettingsModel.language = language
	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	StatefulColors {
		id: colors

		groupMember: true
		statefulControl: root
	}
}
