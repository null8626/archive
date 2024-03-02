alert("Please use the following code on mobile app to prevent errors and scale issues.");
/* THE REST OF THE BUTTON EVENTS*/
function googlesearch(){
    var gsearch = document.getElementById("inputbox1").value;
    if (gsearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://www.google.com/search?q='+gsearch;
    }
}
function youtubesearch(){
    var ysearch = document.getElementById("inputbox2").value;
    if (ysearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://www.youtube.com/results?q='+ysearch;
    }
}
function wikipediasearch(){
    var wsearch = document.getElementById("inputbox3").value;
    if (wsearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://en.wikipedia.org/w/index.php?search='+wsearch+'&title=Special%3ASearch&go=Go&ns0=1';
    }
}
function ccsearch(){
    var vccsearch = document.getElementById("inputbox4").value;
    if (vccsearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://en.wikipedia.org/w/index.php?search='+wsearch+'&title=Special%3ASearch&go=Go&ns0=1';
    }
}
function wikihowsearch(){
    var wikisearch = document.getElementById("inputbox5").value;
    if (wikisearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://www.wikihow.com/wikiHowTo?search='+wikisearch;
    }
}
function instagramsearch(){
    var isearch = document.getElementById("inputbox6").value;
    if (isearch == "") {
        alert("Please do not leave it empty.");
    } else {
        window.location.href='https://www.instagram.com/explore/tags/'+isearch+'/';
    }
}
/* COPYRIGHT INFO */
function copyrightinfo(){
    document.getElementById("copyright").innerHTML = "This text cannot be changed with HTML modification. Please do NOT steal this code without permission.";
}
/* TEMPORARY */
function tempInfoBack(){
    document.getElementById("copyright").innerHTML = "© 2020 null8626 Programming. - ORIGINAL CODE";
}