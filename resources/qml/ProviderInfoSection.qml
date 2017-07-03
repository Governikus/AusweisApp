import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import "global"
import "identify"

Rectangle {

	property string imageSource
	property string title
	property string name
	property bool showForwardAction: true

	width: parent.width
	height: Utils.dp(40)
	color: "white"
	clip: true

	Image
	{
		id: image
		anchors.verticalCenter: parent.verticalCenter

		height: parent.height
		width: parent.width * 0.15

		source: imageSource
		fillMode: Image.PreserveAspectFit
	}
	LabeledText {
		id: providerTitle
		anchors.verticalCenter: image.verticalCenter
		anchors.left: image.right
		anchors.right: forwardAction.right

		label: title
		text: name.length > 0 ? name : qsTr("Touch for more details")
	}
	Text {
		id: forwardAction
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(18)
		anchors.verticalCenter: parent.verticalCenter

		text: ">"
		font.pixelSize: Utils.sp(22)
		color: Constants.grey
		visible: showForwardAction
	}
	SeparatorLine {
		anchors.left: providerTitle.left
		anchors.right: providerTitle.right
	}

	MouseArea {
		anchors.fill: parent
		enabled: showForwardAction
		onClicked: push(certificateDescriptionPage, {providerName: name})
	}

	CertificateDescriptionPage {
		id: certificateDescriptionPage
		visible: false
	}
}
