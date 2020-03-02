var gpio = require("pigpio").Gpio;
var http = require("http");
var url = require("url");

var pistons = [{"pin": 17, "state": false, "time": 200, "keyword": "piston1"}, {"pin": 18, "state": false, "time": 500, "keyword": "piston2"}];

for (var id in pistons) {pistons[id]["gpio"] = new gpio(pistons[id]["pin"], {mode: gpio.OUTPUT});}

var refresh = function() {
	for (var id in pistons) {	pistons[id]["gpio"].digitalWrite(pistons[id].state);}
}

var clear = function () {
	for (var id in pistons) {	pistons[id]["state"] = false;}

	refresh();
}

var activate = function (piston) {
	piston["state"] = true;
	piston["gpio"].digitalWrite(piston.state);
}

var desactivate = function (piston) {
	piston["state"] = false;
	piston["gpio"].digitalWrite(piston.state);
}

var procedure = function (piston) {
	activate(piston);
	setTimeout(desactivate, piston.time, piston);
}

var keywords = {};

for (var id in pistons) {keywords[pistons[id]["keyword"]] = pistons[id]}

refresh();

http.createServer(function (req, res) {
	var q = req.url;
	try {
//		procedure(keywords[q])
		var u = url.parse(q, true).query;
		console.log(u);
		id = parseInt(u["piston"]) - 1
		if(u["state"] == "on")  	{activate(pistons[id]);}
		else if(u["state"] == "imp")	{procedure(pistons[id]);}
		else if(u["state"] == "query")  {res.end(pistons[id]["state"]);}
		else				{desactivate(pistons[id]);}
		res.end("Ok");
	} catch (error) {
		console.log(error);
		res.end("Error : "+error);
	}
}).listen(8000);

console.log("server listening");
