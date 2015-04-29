var nodes, arcs;

onload = function(){
    var datafile_button = document.getElementById("button");
    var canvas = document.getElementById("canvas");
    var ctx = canvas.getContext("2d");

    document.getElementById("datafile").value = "USA-road-d.NY-central.json";

    datafile_button.onclick = function(){
        var oldfile = document.getElementById("datasrc");
        if(oldfile){
            document.getElementsByTagName("head")[0].removeChild(oldfile);
        }

        var datafile = document.getElementById("datafile").value;
        var ref = document.createElement("script");
        ref.setAttribute("id", "datasrc");
        ref.setAttribute("type", "text/javascript");
        ref.setAttribute("src", datafile);

        document.getElementsByTagName("head")[0].appendChild(ref);

        ref.onload = draw;

    }

    function draw(){
        ctx.beginPath();
        ctx.clearRect(0,0,canvas.width,canvas.height);

        for(var i=10; i<11; i++){
            console.log(arcs.length);
            ctx.moveTo(nodes[arcs[i].from].x, canvas.height - nodes[arcs[i].from].y);
            ctx.lineTo(nodes[arcs[i].to].x, canvas.height - nodes[arcs[i].to].y);
            ctx.stroke();
        }

        document.getElementById("node").innerHTML = "#node = " + nodes.length;
        document.getElementById("arc").innerHTML = "#arc = " + arcs.length;
    }
    /*
    document.getElementById("xRange").innerHTML =
        "longitude = " + nodes.xRange;
    document.getElementById("yRange").innerHTML =
        "latitude = " + nodes.yRange;
    */
}

