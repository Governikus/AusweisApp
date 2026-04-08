/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

GAbstractButton {
	id: root

	Accessible.name: ApplicationModel.stripHtmlTags(text)
	Accessible.role: Accessible.Button
	Layout.fillHeight: true
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
			tintColor: Style.color.textTitle.basic_checked
		}
		GText {
			Accessible.ignored: true
			color: root.activeFocus && UiPluginModel.showFocusIndicator ? Style.color.textTitle.basic_checked : Style.color.textTitle.basic_unchecked
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
