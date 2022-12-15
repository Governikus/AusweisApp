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

	property string a11yDisabledText: qsTr("Disabled")
	property bool visualPrivacy: false

	Accessible.name: numberPadButton.enabled ? text : a11yDisabledText
	Layout.fillHeight: true
	Layout.fillWidth: true
	implicitHeight: 36
	implicitWidth: 36

	background: TintableIcon {
		anchors.centerIn: numberPadButton
		source: numberPadButton.icon.source
		sourceSize.height: numberPadButton.implicitHeight
		visible: numberPadButton.icon.source != "" && numberPadButton.enabled
	}
	contentItem: GText {
		Accessible.ignored: true
		horizontalAlignment: Text.AlignHCenter
		text: numberPadButton.text
		textStyle: Style.text.title
		verticalAlignment: Text.AlignVCenter
		visible: numberPadButton.icon.source == "" && numberPadButton.enabled
	}

	Accessible.onPressAction: if (numberPadButton.enabled)
		clicked()

	Rectangle {
		id: darkLayer
		anchors.centerIn: parent
		color: Constants.black
		height: width
		opacity: 0.1
		radius: width / 2

		NumberAnimation on opacity  {
			from: 0.1
			running: !visualPrivacy && !numberPadButton.down
			to: 0.0
		}
		SequentialAnimation on width  {
			alwaysRunToEnd: true
			running: !visualPrivacy && numberPadButton.down

			PropertyAction {
				property: "opacity"
				target: darkLayer
				value: 0.1
			}
			NumberAnimation {
				from: 0
				to: Math.SQRT2 * numberPadButton.width
			}
		}
	}
}
