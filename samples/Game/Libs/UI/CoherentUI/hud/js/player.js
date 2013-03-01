(function (window, _, engine, $) {
    'use strict';
    
    function CreateHealthBar(id, ticks) {
        var t = _.template($('#healthBarTemplate').html());
        return $(t({id: id, ticks: ticks}));
    }
    
    function SetHealthBar(value) {
        var ticks = $('#healthBarTicks > div.HealthBarTick'),
            length = ticks.length,
            limit = Math.round(value * length / 100),
            i;
        
        for (i = 0; i < limit; ++i) {
            $(ticks[i]).addClass('Filled');
        }
        for (i = limit; i < length; ++i) {
            $(ticks[i]).removeClass('Filled');
        }
    }
    
    function SetHealthDigits(value) {
        var healthDigits = $('#health > div > img'),
            digits = healthDigits.length;

        for (var i = 0; i < digits; ++i) {
            var digit = value % 10;
            $(healthDigits[i]).css('margin-left', -1 + -24 * digit);
            value = (value - digit) / 10;
        }
    }
        
    function SetHealth(value) {
        var health = (value > 0) ? (value < 100 ? value : 99) : 0;
        Player.health = health;
        SetHealthBar(health);
        SetHealthDigits(health);
    }
    
    engine.on('OnPlayerHealthChanged', SetHealth);
        
    engine.on('Ready', function () {
        $('#healthBar').html(CreateHealthBar('healthBarTicks', 10));
        SetHealth(99);
    });
    
    window.Player = { health : 99 };
})(window, _, engine, $);