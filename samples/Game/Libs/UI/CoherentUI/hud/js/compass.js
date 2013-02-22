(function (window) {
	'use strict;'
	
	const POSTERS_PER_ROW = 16;
	const RING_RADIUS = 100;
	
	var lables = ['N', '&#9671;', 'NE', '&#9671;', 'E', '&#9671;', 'SE', '&#9671;', 'S', '&#9671;', 'SW', '&#9671;', 'W', '&#9671;', 'NW', '&#9671;']

	function setup_posters (row)
	{
		var posterAngle = 360 / lables.length;
		for (var i = 0; i < lables.length; i ++) {
			var poster = window.document.createElement('div');
			poster.className = 'poster';
			// compute and assign the transform for this poster
			var transform = 'rotateY(' + (posterAngle * i) + 'deg) translateZ(' + RING_RADIUS + 'px)';
			poster.style.webkitTransform = transform;
			// setup the number to show inside the poster
			var content = poster.appendChild(window.document.createElement('p'));
			content.innerHTML = lables[i];
			// add the poster to the row
			row.appendChild(poster);
		}
	}

	function init ()
	{
		setup_posters(window.document.getElementById('compassImage'));
		rotate(0);
	}

	// call init once the window.document is fully loaded
	window.addEventListener('load', init, false);

	function normalizeAngle(a) {
		var t = a + 360;
		return t - Math.floor(t / 360) * 360;
	}

	
	function rotate(rotation) {
		var x = window.document.getElementById('compassImage');

		rotation = normalizeAngle(rotation);
		x.style.webkitTransform = 'rotateY(' + (-rotation) + 'deg)';
		
		var c = x.children;
		var n = c.length;
		var angle = 360 / n;
		var n1 = normalizeAngle(rotation + 90);
		var n2 = normalizeAngle(rotation + 270);
		var nl = n1;
		var nr = n2;
		var invert = 0;
		
		if (nl > nr) {
			invert = 180;
			nl = n2;
			nr = n1;
		}
		
		for (var i = 0; i < n; ++i) {
			var a = normalizeAngle(i * angle + invert);
			if (a >= nl && a <= nr) {
				c[i].style.display = 'none';
			}
			else
			{
				c[i].style.display = 'block';
			}
		}
	}
	
	window.Compass = { SetRotation: rotate };
}(window));