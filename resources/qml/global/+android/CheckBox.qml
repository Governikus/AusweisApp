import QtQuick 2.5

import "."

Rectangle {
	property bool checked

	height: Utils.dp(20)
	width: height

	color: checked ? Constants.accent_color : "white"
	border.color: checked ? Constants.accent_color : "black"
	border.width: Utils.dp(2)
	radius: Utils.dp(2)


	Canvas {
	  id:canvas
	  anchors.fill: parent
	  anchors.margins: Utils.dp(3)
	  visible: checked
	  onPaint:{
		 var ctx = canvas.getContext('2d');
		  ctx.beginPath();
		  ctx.moveTo(0, width/2);
		  ctx.lineWidth = Utils.dp(2);
		  ctx.lineTo(2*width/5, height-ctx.lineWidth);
		  ctx.lineTo(width, 0);
		  ctx.strokeStyle = "white"
		  ctx.stroke();
	  }
	}
}
