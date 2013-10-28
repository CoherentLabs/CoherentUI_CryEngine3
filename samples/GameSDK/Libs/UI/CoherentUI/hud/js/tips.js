(function (window, engine, $, _) {
    'use strict';
    
    var tips = ['Go Go Go!', 'HTML5', 'CSS3', 'JavaScript'],
        fadingTime = 500, // ms
        rollInTime = 3000, // ms
        displayTime = 2000, // ms
        tipTemplate;
    
    function GetRollerOut(item) {
        return function () {
            window.setTimeout(function () {
                $(item.children()[1]).fadeOut(fadingTime, function () {
                    item.addClass('rollOut');
                    
                    window.setTimeout(function () {
                        item.fadeOut(fadingTime, function () { item.detach(); });
                    }, rollInTime + 2 * fadingTime);
                });
            }, displayTime);
        };
    }

    function GetRollerIn(item) {
        return function () {
            item.addClass('rollIn');
            
            window.setTimeout(function () {
                $(item.children()[1]).fadeIn(fadingTime, GetRollerOut(item));
            }, rollInTime);
        };
    }
    
    function ShowTip(tip) {
        var item = $(tipTemplate({text: tip})),
            box = $('#tipBox');
        
        item.fadeIn(fadingTime, GetRollerIn(item));
        box.append(item);
    }
    
    function ShowRandomTip() {
        var index = Math.floor(Math.random() * tips.length);
        ShowTip(tips[index]);
        window.setTimeout(ShowRandomTip, 16000 + Math.random() * 4000);
    }
    
    engine.on('Ready', function () {
        tipTemplate = _.template($('#tipTemplate').html());
    });
    engine.on('Ready', ShowRandomTip);
    
})(window, engine, $, _);