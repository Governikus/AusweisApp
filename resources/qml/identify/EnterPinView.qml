import QtQuick 2.5

import ".."
import "../global"

SectionPage
{
	property alias state: pinpad.state

	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") }

	PinPad {
		id: pinpad
		anchors.fill: parent

		onPinEntered: {
			authModel.continueWorkflow()
			identifyView.push(identifyProgressView)
		}
	}
}
