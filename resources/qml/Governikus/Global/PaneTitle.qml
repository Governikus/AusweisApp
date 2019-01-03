import QtQuick 2.10


Text {
	anchors.left: parent.left
	anchors.right: parent.right
	visible: text !== ""
	font.pixelSize: Constants.pane_title_font_size
	color: Constants.blue
	elide: Text.ElideRight
}
