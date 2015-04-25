onload = function(){
    var canvas = document.getElementById("canvas");
    var ctx = canvas.getContext("2d");

    for(var i=0; i<arcs.length; i++){
        ctx.moveTo(nodes[arcs[i].from].x, canvas.height - nodes[arcs[i].from].y);
        ctx.lineTo(nodes[arcs[i].to].x, canvas.height - nodes[arcs[i].to].y);
        ctx.stroke();
    }

    document.getElementById("node").innerHTML = "#node = " + nodes.length;
    document.getElementById("arc").innerHTML = "#arc = " + arcs.length;
    document.getElementById("xRange").innerHTML =
        "longitude = " + nodes.xRange;
    document.getElementById("yRange").innerHTML =
        "latitude = " + nodes.yRange;
}
