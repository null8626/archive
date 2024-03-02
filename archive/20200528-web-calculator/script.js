/*
Hello guys!
I am back after a loooong time being inactive.
Sorry! (i spent a long time coding a discord bot, lel)

Otherwise, i have improved a lot in JavaScript, so here ya go :D
*/

function addnum(n) {
    if (document.getElementById('main').textContent=='Calculator') {
        var operation = '';
    } else {
        var operation = document.getElementById('main').textContent;
    }
    var operation = operation + n.toString();
    document.getElementById('main').innerHTML = operation;
}
function execute() {
    var toExecute = document.getElementById('main').textContent;
    var temp = toExecute.split("");
    var total = [];
    for (i = 0; i < temp.length; i++) {
        if (temp[i]=='×') {
            total.push('*');
        } else if (temp[i]=='÷') {
            total.push('/');
        } else if (temp[i]=='^') {
            total.push('**');
        } else {
            total.push(temp[i])
        }
    }
    var toExecute = total.join("");
    try {
        eval(toExecute);
    } catch (err) {
        alert('ERROR! '+err);
        document.getElementById("main").innerHTML = "";
    }
    if (eval(toExecute)==undefined) {
        alert('ERROR!');
    } else {
        document.getElementById('main').innerHTML = eval(toExecute);
    }
}