(function ($, window, engine) {
    'use strict';
    
    var sparkData = [],
        sparkIndex = 0,
        sparkMult = 3,
        sparkChange = 50;
    
    function GenerateSparklineData(samples, remove) {
        if (remove !== null) {
            sparkData.splice(0, remove);
        }
        for (var s = sparkData.length; s < samples; ++s) {
            sparkData.push(Math.sin(sparkIndex++ * sparkMult) * 16);
            if (sparkIndex == sparkChange) {
                sparkMult = 3 + Math.random() * (sparkMult < 3? 0.5 : -0.5);
                sparkChange += Math.floor(4 * sparkMult * s / 16);
            }
        }
        return sparkData;
    }
    
    function Sparkles(samples, remove)
    {
        $('#oscilloscope').sparkline(GenerateSparklineData(samples, remove), {
            type: 'line',
            lineColor: '#82DF06',
            fillColor: false,
            spotColor: false,
            minSpotColor: false,
            maxSpotColor: false,
            chartRangeMin: -18,
            chartRangeMax: 18,
            height: '100%',
            width: '96%',
            disableInteraction: true
        });
    }
    
    function SparkAnimate() {
        Sparkles(100, 1);
    }
    
    engine.on('Ready', function () {
        Sparkles(100);
        window.setInterval(SparkAnimate, 100);
    });
})($, window, engine);