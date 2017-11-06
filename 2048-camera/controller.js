
var log = function(x){
	document.getElementById('status').innerHTML = x;
}

try{

log('Starting ...');

var dom = null;

var emit = function(make){
	var code;
	switch(make){
		case "up":
			code = 0;
			break;
		case "right":
			code = 1;
			break;
		case "down":
			code = 2;
			break;
		case "left":
			code = 3;
			break;
		default:
			return;
	}

	var script = document.createElement('script');
	script.innerHTML = 'inject.inputManager.emit("move", '+ code +');';
	dom.body.appendChild(script);
};

var frame = document.getElementById('iframe');
frame.addEventListener('load', function (){

	log('Frame loaded ...');

	dom = frame.contentDocument;
	var script = document.createElement('script');
	script.innerHTML = 'var inject = new GameManager(4, KeyboardInputManager, HTMLActuator, LocalStorageManager);'
	dom.body.appendChild(script);

	// Up 0
	// Right 1
	// Down 2
	// Left 3
	//inject.inputManager.emit("move", 0);

	var action = {
		73 : "up",
		76 : "right",
		75 : "down",
		74 : "left"
	};

	document.addEventListener('keydown', function(e){
		document.getElementById('status').innerHTML = e.which;
		if(!action[e.which]) return;
		emit(action[e.which]);
	});


});


var options = {
	ignoreColor : true,
	swipeTimeout : 200,
	breakGaps : false,
	maxLengthGap : 50,
	minimumDistanceEvent : 80,
	minimumSideSlideEvent : 80,
};
var info = {
	size : {
		x : 1,
		y : 1
	}
};


var sys = require('sys');
var net = require('net');
var sockets = [];
var firePlayerEvent = function(player){
try{
	var x = player.data.x;
	player.data.x = [];
	var y = player.data.y;
	player.data.y = [];
	var value;

	results = {
		x : [],
		y : []
	}

	var xlen = 0;
	for (var i = 1; i < x.length; i++) {
		value = x[i] - x[i-1];
		if(options.breakGaps && Math.abs(value) > options.maxLengthGap){
			results.x.push(xlen);
			xlen = 0;
			continue;
		}
		xlen += value;
	}
	results.x.push( xlen );

	var ylen = 0;
	for (var i = 1; i < y.length; i++) {
		value = y[i] - y[i-1];
		if(options.breakGaps && Math.abs(value) > options.maxLengthGap){
			results.y.push( ylen );
			ylen = 0;
			continue;
		}
		ylen += value;
	}
	results.y.push( ylen );

	var xlenPos = Math.max.apply(null, results.x);
	var xlenNeg = Math.min.apply(null, results.x);
	var ylenPos = Math.max.apply(null, results.y);
	var ylenNeg = Math.min.apply(null, results.y);

	var xlen = xlenPos;
	var xneg = false;
	if(xlen < Math.abs(xlenNeg)){
		xlen = Math.abs(xlenNeg);
		xneg = true;
	}

	var ylen = ylenPos;
	var yneg = false;
	if(ylen < Math.abs(ylenNeg)){
		ylen = Math.abs(ylenNeg);
		yneg = true;
	}

	//sys.puts("Swipe : "+ xlen +"x"+ ylen);

	if(xlen > options.minimumDistanceEvent && ylen < options.minimumSideSlideEvent){
		if(xneg){
			emit('left');
			sys.puts("Swipe : LEFT");
		}
		else{
			emit('right');
			sys.puts("Swipe : RIGHT");
		}
	}
	else if(ylen > options.minimumDistanceEvent && xlen < options.minimumSideSlideEvent){
		if(yneg){
			emit('up');
			sys.puts("Swipe : UP");
		}
		else{
			emit('down');
			sys.puts("Swipe : DOWN");
		}
	}

}catch(e){
	sys.puts(e);
}
};
var players = {
	r : {fire:null, data:{x:[],y:[]}},
	g : {fire:null, data:{x:[],y:[]}},
	b : {fire:null, data:{x:[],y:[]}},
	y : {fire:null, data:{x:[],y:[]}},
	w : {fire:null, data:{x:[],y:[]}}
};

var server = net.createServer(function(sock) {
	sys.puts('Connected: ' + sock.remoteAddress + ':' + sock.remotePort);
	sock.pipe(sock);
	sockets.push(sock);

	sock.on('data', function(data) {  // client writes message
		var sdata = data.toString('utf8');

		// On exit
		if(sdata.match(/^-exit/i)){
			sys.puts('exit command received: ' + sock.remoteAddress + ':' + sock.remotePort + '\n');
			sock.destroy();
			var idx = sockets.indexOf(sock);
			if (idx != -1) {
				delete sockets[idx];
			}
			return;
		}

		// Data
		if(sdata.match(/^-data:/i)){
			//sys.puts(sdata);
			var players_received = sdata.match(/\w\s*:\s*\d+x\d+x\d+/gi);
			var d;
			for (var i = 0; i < players_received.length; i++) {
				d = players_received[i].match(/(\w)\s*:\s*(\d+)x(\d+)x\d+/i);
				if(options.ignoreColor){
					d[1] = 'w';
				}
				if(players[d[1]]){
					players[d[1]].data.x.push(d[2]);
					players[d[1]].data.y.push(d[3]);
					clearTimeout(players[d[1]].fire);
					players[d[1]].fire = setTimeout((function(obj){
						return function(){
							firePlayerEvent(obj);
						};
					})(players[d[1]]), options.swipeTimeout);
				}
			}

			return;
		}

		// Data
		if(sdata.match(/^-size:/i)){
			var sizes = sdata.match(/-size:\s*(\d+)x(\d+)/i);
			info.size.x = sizes[1];
			info.size.y = sizes[2];
		}

		// -size: 320x240-reflect: 1
		//sys.puts(JSON.stringify(data));
		//sys.puts(sdata);
	});

	sock.on('end', function() { // client disconnects
		sys.puts('Disconnected: ' + data + data.remoteAddress + ':' + data.remotePort + '\n');
		var idx = sockets.indexOf(sock);
		if (idx != -1) {delete sockets[idx];}
	});

});

server.listen(6969, '127.0.0.1');


}catch(e){
	alert(e);
}