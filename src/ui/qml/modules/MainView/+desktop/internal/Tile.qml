/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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

	Accessible.name: ApplicationModel.stripHtmlTags(text)
	Accessible.role: Accessible.Button
	Layout.fillHeight: true
	Layout.fillWidth: true
	implicitHeight: 1
	implicitWidth: 1
	padding: 2 * Style.dimens.pane_padding

	background: GPaneBackground {
		border.color: colors.paneBorder
		color: colors.paneBackground
		opacity: SettingsModel.showBetaTesting ? 0.9 : 1.0

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: Column {
		spacing: Style.dimens.pane_spacing

		TintableIcon {
			source: root.icon.source
			sourceSize.height: Style.dimens.huge_icon_size
			tintColor: Style.color.textTitle.checked
		}
		GText {
			Accessible.ignored: true
			activeFocusOnTab: false
			color: root.activeFocus && UiPluginModel.showFocusIndicator ? Style.color.textTitle.checked : Style.color.textTitle.basic
			horizontalAlignment: Text.AlignLeft
			text: root.text
			textStyle: Style.text.tile
			width: parent.width
		}
	}

	StatefulColors {
		id: colors

		statefulControl: root
	}
}
