var codify = require('./codify');

var a = 0;

exports['single digit tests'] = function(test) {
    var code = codify.toCode(0);
    test.equal(code, '0');
    test.equal(0, codify.toInt(code));

    code = codify.toCode(1);
    test.equal(code, '1');
    test.equal(1, codify.toInt(code));

    code = codify.toCode(10);
    test.equal(code, 'A');
    test.equal(10, codify.toInt(code));

    code = codify.toCode(35);
    test.equal(code, 'Z');
    test.equal(35, codify.toInt(code));

    test.done();
};

exports['double digit tests'] = function(test) {
    var code = codify.toCode(36);
    test.equal(code, '10');
    test.equal(36, codify.toInt(code));

    code = codify.toCode(71);
    test.equal(code, '1Z');
    test.equal(71, codify.toInt(code));

    test.done();
};

exports['random tests'] = function(test) {
    for (var i = 0; i < 1000; i++) {
        var val = Math.floor(Math.random()*1000000000);
        var code = codify.toCode(val);
        var calculated = codify.toInt(code);
        test.equal(calculated, val);
    }
    test.done();
};

exports['size'] = function(test) {
    var code = codify.toCode(0,5);
    test.equal(code, '00000');
    test.equal(codify.toInt(code), 0);
    test.done();
};