import QtQuick 2.10


Column {
	property alias label: label.text
	property alias text: body.text
	property alias textFormat: body.textFormat
	property alias textUppercase: body.font.capitalization

	signal linkActivated(string link)

	width: Math.max(label.implicitWidth, body.implicitWidth)

	Text {
		id: label
		anchors.left: parent.left
		anchors.right: parent.right
		font.pixelSize: Constants.normal_font_size
		color: Constants.blue
		wrapMode: Text.WordWrap
	}

	Text {
		id: body
		color: Constants.black
		anchors.left: parent.left
		anchors.right: parent.right
		font.pixelSize: Constants.normal_font_size
		wrapMode: Text.WordWrap
		onLinkActivated: parent.linkActivated(link)
	}
}
