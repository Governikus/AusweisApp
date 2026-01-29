/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	property bool inactive: false
	property var percent
	property int size: Style.dimens.icon_size

	Accessible.name: inactive ?
	//: ALL_PLATFORMS
	qsTr("Link quality unavailable.") :
	//: ALL_PLATFORMS %1 is replaced with a number between 0 and 100
	qsTr("%1% link quality.").arg(percent)
	Accessible.role: Accessible.StaticText
	implicitHeight: size
	implicitWidth: size

	RemoteIcon {
		source: "qrc:///animations/icon_remote_1.svg"
		threshold: 25
	}
	RemoteIcon {
		source: "qrc:///animations/icon_remote_2.svg"
		threshold: 50
	}
	RemoteIcon {
		source: "qrc:///animations/icon_remote_3.svg"
		threshold: 75
	}
	RemoteIcon {
		source: "qrc:///animations/icon_remote_4.svg"
		threshold: 100
	}

	component RemoteIcon: TintableIcon {
		required property int threshold

		sourceSize.height: root.size
		tintColor: root.inactive || root.percent < (SettingsModel.useAnimations ? threshold : 20) ? Style.color.remoteIndicator.disabled_unchecked : Style.color.remoteIndicator.basic_unchecked
	}
}
