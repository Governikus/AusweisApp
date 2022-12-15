/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

CheckBox {
	id: control

	property alias maximumLineCount: description.maximumLineCount
	property alias textStyle: description.textStyle

	padding: 0

	contentItem: GText {
		id: description
		Accessible.ignored: true
		activeFocusOnTab: false
		elide: Text.ElideRight
		leftPadding: control.indicator.width + control.spacing
		maximumLineCount: 1
		text: control.text
		textStyle: Style.text.normal
		verticalAlignment: Text.AlignVCenter
	}
	indicator: Rectangle {
		border.color: enabled ? Style.color.accent : Style.color.button_disabled
		border.width: Math.max(ApplicationModel.scaleFactor * 4, 1)
		color: enabled ? (control.checked ? Style.color.button : Style.color.transparent) : Style.color.button_disabled
		implicitHeight: ApplicationModel.scaleFactor * 33
		implicitWidth: implicitHeight
		radius: Math.max(ApplicationModel.scaleFactor * 4, 1)

		TintableIcon {
			anchors.fill: parent
			anchors.margins: Math.max(ApplicationModel.scaleFactor * 4, 1)
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/checkbox_indicator.svg"
			tintColor: Style.color.button_text
			visible: control.checked
		}
	}

	Accessible.onPressAction: {
		toggle();
		toggled();
	}
}
