(function (window, engine, $) {
	'use strict';
	
	var map;
	var myMarker;
	
	function InitializeMap()
	{
		map = new OpenLayers.Map('map');
		
		//
		// Coordinate systems relative to Forest.jpg
		//
		//    OpenLayers               CryEngine
		//                                                    
		//        ^ +Y                     ^ -X    
		//        |                        |
		//  -X    |    +X            -Y    |    +Y
		//  <-----+----->            <-----+----->
		//        |                        |
		//        |                        |
		//        v -Y                     v +X
		//
		
		var graphic = new OpenLayers.Layer.Image(
			'Forest',
			'img/Forest.jpg',
			new OpenLayers.Bounds(6, -1240, 1024, -200),
			new OpenLayers.Size(2048, 2048),
			{numZoomLevels: 4}
		);
		
		graphic.events.on({
			loadstart: function() {
				OpenLayers.Console.log("loadstart");
			},
			loadend: function() {
				OpenLayers.Console.log("loadend");
			}
		});

		map.addLayer(graphic);
		map.addControl(new OpenLayers.Control.LayerSwitcher());
		map.zoomToMaxExtent();
		
		var markers = new OpenLayers.Layer.Markers( "Markers" );
		map.addLayer(markers);
		
		// Some decent default coordinates around the spawn location
		var markerDefaultCoordinates = new OpenLayers.LonLat(350,-850);
		myMarker = new OpenLayers.Marker(markerDefaultCoordinates);
		map.setCenter(markerDefaultCoordinates);
		markers.addMarker(myMarker);
		
		// Bind engine methods
		engine.on('SetPlayerPositionOnMap', function (x, y) {
			MoveMarker(y, -x);
		});
		
		engine.on('SetPlayerRotationOnMap', function (deg) {
			SetMarkerRotation(deg);
		});
		
		engine.on('ShowMap', function (show) {
			ShowMap(show);
		});
	}
	
	function MoveMarker(lon, lat){
		var newPx = map.getLayerPxFromViewPortPx(
						map.getPixelFromLonLat(
							new OpenLayers.LonLat(lon, lat)
								.transform(map.displayProjection, map.projection)
						)
					);
		myMarker.moveTo(newPx);
		
		// Ensure the marker is visible after the move
		var vpExtents = map.getExtent();
		if (
			vpExtents.left > myMarker.lonlat.lon ||
			vpExtents.right < myMarker.lonlat.lon ||
			vpExtents.bottom > myMarker.lonlat.lat ||
			vpExtents.top < myMarker.lonlat.lat
			)
		{
			map.setCenter(myMarker.lonlat);
		}
	}
	
	function SetMarkerRotation(deg)
	{
		myMarker.icon.imageDiv.style.webkitTransform = "rotate(" + deg + "deg)";
	}
	
	function ShowMap(show)
	{
		$("#map").css("display", show ? "block" : "none");
	}

	InitializeMap();
	ShowMap(false);
	
})(window, engine, $);