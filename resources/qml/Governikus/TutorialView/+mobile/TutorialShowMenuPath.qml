/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: root

	property alias backgroundIcon: icon.source
	property alias newSectionImage: newSection.source
	property double newSectionPointerX: 1
	property double newSectionPointerY: 0

	height: width * 0.6

	Image {
		id: icon
		height: 0.85 * parent.height
		width: 0.54 * parent.width
		x: 0.2167 * parent.width
		y: 0
		z: -1
	}
	TutorialZoomTriangle {
		height: startMenu.height
		opacity: 0.3
		width: 0.083 * parent.width
		x: startMenu.x + startMenu.width
		y: startMenu.y
		z: 1
	}
	TutorialImage {
		id: startMenu
		centerX: 0.483 + 0.1525
		centerY: 0.15 + 0.4
		source: "qrc:///images/tutorial/screenshot_start_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		width: root.width * 0.3
	}
	TutorialZoomTriangle {
		height: newSection.height
		opacity: 0.3
		pointerXRatio: root.newSectionPointerX
		pointerYRatio: root.newSectionPointerY
		width: 0.083 * parent.width
		x: newSection.x + newSection.width
		y: newSection.y
		z: 1
	}
	TutorialImage {
		id: newSection
		centerX: 0.125 + 0.1525
		centerY: 0.075 + 0.4
		width: root.width * 0.3
	}
	TutorialImage {
		centerX: 0.808 + 0.0667
		centerY: 0.4 + 0.155
		source: "qrc:///images/tutorial/phone.svg"
		width: root.width / 7.5
	}
}
