import QtQuick 2.5

import ".."

SectionPage
{
	property alias state: pinpad.state

	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: changePinModel.cancelWorkflow() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Change PIN") }

	PinPad {
		id: pinpad
		anchors.fill: parent

		onPinEntered: {
			changePinModel.continueWorkflow()
			pinView.push(pinProgressView)
		}
	}
}
