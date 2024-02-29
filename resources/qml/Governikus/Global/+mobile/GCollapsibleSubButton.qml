/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
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

	horizontalPadding: Constants.component_spacing * 2
	verticalPadding: Constants.component_spacing / 2

	background: Rectangle {
		color: colors.control_preferredPaneBackground
	}
	contentItem: RowLayout {
		spacing: Constants.component_spacing

		TintableIcon {
			id: icon

			sourceSize.height: Style.dimens.small_icon_size
			tintColor: label.color
			tintEnabled: false
		}
		GText {
			id: label

			Accessible.ignored: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			color: colors.text
		}
	}

	StatefulColors {
		id: colors

		statefulControl: root
	}
}
