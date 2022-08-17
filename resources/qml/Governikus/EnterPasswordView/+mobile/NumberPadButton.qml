/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Button {
	id: numberPadButton

	property bool visualPrivacy: false
	property string a11yDisabledText: qsTr("Disabled")

	implicitHeight: 36
	implicitWidth: 36
	Layout.fillHeight: true
	Layout.fillWidth: true

	Accessible.name: numberPadButton.enabled ? text : a11yDisabledText
	Accessible.onPressAction: if (numberPadButton.enabled) clicked()

	contentItem: GText {
		visible: numberPadButton.icon.source == "" && numberPadButton.enabled

		Accessible.ignored: true

		text: numberPadButton.text
		textStyle: Style.text.title
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: TintableIcon {
		visible: numberPadButton.icon.source != "" && numberPadButton.enabled
		anchors.centerIn: numberPadButton

		sourceSize.height: numberPadButton.implicitHeight
		source: numberPadButton.icon.source
	}

	Rectangle {
		id: darkLayer

		height: width
		radius: width / 2
		anchors.centerIn: parent

		color: Constants.black
		opacity: 0.1

		SequentialAnimation on width {
			running: !visualPrivacy && numberPadButton.down
			alwaysRunToEnd: true
			PropertyAction { target: darkLayer; property: "opacity"; value: 0.1 }
			NumberAnimation { from: 0; to: Math.SQRT2 * numberPadButton.width }
		}

		NumberAnimation on opacity {
			running: !visualPrivacy && !numberPadButton.down
			from: 0.1
			to: 0.0
		}
	}
}
