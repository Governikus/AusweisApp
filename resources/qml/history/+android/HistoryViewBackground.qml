import QtQuick 2.5

import "../global"

Canvas {
	id: canvas
	anchors.fill: parent
	property real start_x: parent.width / 2
	property real start_y: 0
	property real end_x: parent.width / 2
	property real end_y: height
	property bool dashed: true
	property real dash_length: Utils.dp(3)
	property real dash_space: Utils.dp(5)
	property real line_width: Utils.dp(1)
	property real stipple_length: (dash_length + dash_space) > 0 ? (dash_length + dash_space) : 8
	property color draw_color: Constants.blue

	onPaint: {
		// Get the drawing context
		var ctx = canvas.getContext('2d')
		// set line color
		ctx.strokeStyle = draw_color;
		ctx.lineWidth = line_width;
		ctx.beginPath();

		if (!dashed)
		{
			ctx.moveTo(start_x,start_y);
			ctx.lineTo(end_x,end_y);
		}
		else
		{
			var dashLen = stipple_length;
			var dX = end_x - start_x;
			var dY = end_y - start_y;
			var dashes = Math.floor(Math.sqrt(dX * dX + dY * dY) / dashLen);
			if (dashes == 0)
			{
				dashes = 1;
			}
			var dash_to_length = dash_length/dashLen
			var space_to_length = 1 - dash_to_length
			var dashX = dX / dashes;
			var dashY = dY / dashes;
			var x1 = start_x;
			var y1 = start_y;

			ctx.moveTo(x1,y1);

			var q = 0;
			while (q++ < dashes) {
				x1 += dashX*dash_to_length;
				y1 += dashY*dash_to_length;
				ctx.lineTo(x1, y1);
				x1 += dashX*space_to_length;
				y1 += dashY*space_to_length;
				ctx.moveTo(x1, y1);

			}

		}

		ctx.stroke();

	}
}
