/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style
import Governikus.Type

Image {
	id: img

	property bool inactive: false
	property var percent

	fillMode: Image.PreserveAspectFit
	source: {
		if (inactive) {
			return "qrc:///images/icon_remote_inactive_%1.svg".arg(Style.currentTheme.name);
		}
		if (!SettingsModel.useAnimations && percent >= 20) {
			return "qrc:///images/icon_remote_100_%1.svg".arg(Style.currentTheme.name);
		}
		if (percent >= 80) {
			return "qrc:///images/icon_remote_100_%1.svg".arg(Style.currentTheme.name);
		}
		if (percent >= 60) {
			return "qrc:///images/icon_remote_75_%1.svg".arg(Style.currentTheme.name);
		}
		if (percent >= 40) {
			return "qrc:///images/icon_remote_50_%1.svg".arg(Style.currentTheme.name);
		}
		if (percent >= 20) {
			return "qrc:///images/icon_remote_25_%1.svg".arg(Style.currentTheme.name);
		}
		return "qrc:///images/icon_remote_inactive_%1.svg".arg(Style.currentTheme.name);
	}
	sourceSize.width: Style.dimens.icon_size
}
