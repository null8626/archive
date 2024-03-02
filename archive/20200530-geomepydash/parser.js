function fixDat(word) {
    var arr = word.split("");
    var res = new Array(); // no this is not copied
    for (i = 0; i < arr.length; i++) {
        if (res.length==0) {
            res.push(arr[i].toUpperCase());
            continue;
        }
        res.push(arr[i]);
    }
    return res.join("");
}

var main = document.getElementById('docs');
var chars = document.getElementById('chars');
var misc = document.getElementById('misc');
fetch('./documentation.json')
.then (res => res.json())
.then (out => {
    var data = out;
    // for each in "main"
    var total = '';
    for (i = 0; i < data['main'].length; i++) {
        var total = total +'<strong style="font-size:30px;">'+ data['main'][i]['name'] + '</strong>';
        var total = total +'<p style="font-family:monospace;">'+data['main'][i]['src']+'</p>';
        var total = total +'<p style="font-size:20px;"><strong>Usage: </strong>'+data['main'][i]['usage']+'<br><strong>Returns format in: </strong>'+data['main'][i]['format']+'<br><strong>Parameters:</strong></p>';
        if (data['main'][i]['parameters'].length>0) {
            var count = 1;
            var total = total + '<ul>';
            for (j = 0; j < data['main'][i]['parameters'].length; j++) {
                var req = '<strong>(OPTIONAL)</strong>';
                if (data['main'][i]['parameters'][j]['required']==true) {
                    var req = '<strong>(REQUIRED)</strong>';
                } 
                var total = total + '<li><i>' + data['main'][i]['parameters'][j]['name'] + '</i><br>Format: <a style="font-family:monospace;">'+ data['main'][i]['parameters'][j]['type']+'</a><br>'+req+'</li>';
            }
            var total = total + '</ul><br>';
        } else {
            var total = total + '<strong><i>No parameters required.</i></strong><br>'
        }
        var total = total + '<hr>';
    }
    main.innerHTML = total;

    var names = ["char", "misc"];
    var namesDom = [chars, misc];
    var namesParms = ["expressions", "parameters"];
    for (c = 0; c < names.length; c++) { //C++ UWU
        var total = '<hr>';
        for (a = 0; a < data[names[c]].length; a++) {
            var total = total + '<br><a style="font-size:20px; font-family:monospace;">' + data[names[c]][a]['name'] + '</a><br><strong>'+fixDat(namesParms[c])+': </strong>';
            if (data[names[c]][a][namesParms[c]].length>0) {
                var total = total + '<ul>';
                for (j = 0; j < data[names[c]][a][namesParms[c]].length; j++) {
                    var total = total + '<li>'+data[names[c]][a][namesParms[c]][j]+'</li>';
                }
                var total = total + '</ul>';
            } else {
                var total = total + '<i>No '+fixDat(namesParms[c])+' required.</i><br>';
            }
        }
        namesDom[c].innerHTML = total;
    }
});