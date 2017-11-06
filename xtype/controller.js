
var log = function(x){
	document.getElementById('status').innerHTML = x;
}

try{

log('Starting ...');

var dom = null;
var win = null;

var emit = function(x, y, shoot){
	if(x > 0 && y > 0){
		win.ig.input.mouse.x = x;
		win.ig.input.mouse.y = y;
	}
	win.ig.input.actions.shoot = shoot;
};

var frame = document.getElementById('iframe');
frame.addEventListener('load', function (){
	log('Frame loaded ...');
	dom = frame.contentDocument;
	win = frame.contentWindow;
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

var clear_press = null;
var before_emit = function(x,y){
	try{

		clearTimeout(clear_press);
		clear_press = setTimeout(function(){
			emit(-1, -1, false);
		},100);

		var w = 478;
		var h = 715;

		var fakeX =  Math.round((x*w)/info.size.x);
		var fakeY =  Math.round((y*h)/info.size.y);

		emit( fakeX , fakeY, true);

	}catch(e){
		sys.puts(e);
	}
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
			if(players_received.length > 0) {
				d = players_received[0].match(/(\w)\s*:\s*(\d+)x(\d+)x\d+/i);
				if(options.ignoreColor){
					d[1] = 'w';
				}
				before_emit( parseInt(d[2]) , parseInt(d[3]) );
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