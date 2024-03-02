function flipresults(){
    var a = Math.floor(Math.random() * 2);
    document.getElementById("start").disabled = false;
    document.getElementById("start").style.cursor = "pointer";
    document.querySelector('#start').textContent = 'SPIN AGAIN';
    if (a==1){
        document.getElementById("results").innerHTML = "HEADS!";
        document.getElementById("results").style.color = "lightgreen";
        document.getElementById("coin").src = "https://adeletedgithubpage.github.io/sources/coinheads.gif";
    } else {
        document.getElementById("results").innerHTML = "TAILS!";
        document.getElementById("results").style.color = "red";
        document.getElementById("coin").src = "https://adeletedgithubpage.github.io/sources/cointails.gif";
    }
}
function flip(){
    /* WHILE FLIPPING THE COIN */
    document.getElementById("results").style.color = "white";
    document.getElementById("coin").src = "https://thumbs.gfycat.com/EssentialNaughtyGeese-size_restricted.gif";
    document.getElementById("results").innerHTML = "PLEASE WAIT...";
    document.querySelector('#start').textContent = '...';
    document.getElementById("start").disabled = true;
    document.getElementById("start").style.cursor = "not-allowed";
    var time = Math.floor((Math.random() * 4567) + 1234);
    setTimeout(flipresults, time);
}