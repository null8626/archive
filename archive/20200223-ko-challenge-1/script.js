var link = document.querySelector('b');
function func() {
    var validInt = /[0123456789]/;
    var integer = prompt('Please input any number.');
    if (integer=="") {
        alert('Nothing is entered.');
    }
    if (integer.match(validInt)) {
        var integerArray = [];
        for (var i = 1; i <= integer; i++) {
        integerArray.push(i);
        }
        var num = integerArray.filter(d => /0/.test(d));
        document.getElementById('textbox').value = num;
        document.getElementById('answer').innerHTML = "Total of numbers containing 0: " + num.length;
        document.getElementById('textbox').style.visibility = "visible";
    } else {
        alert('That is not a number!');
    }
}