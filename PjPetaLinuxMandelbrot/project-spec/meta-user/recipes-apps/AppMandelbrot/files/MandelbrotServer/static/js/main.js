// for windows
// var SERVER_URL = "http://localhost:8080/"
// for ZYBO
// var SERVER_URL = "http://192.168.1.87:8080/"
// SERVER_URL is retrieved from dynamic_info.js which is created by main.py

setInterval(function() {
	callApi(
		SERVER_URL + "getStatus",
		{
			"dummy": "dummy",
		},
		function (o) {
			document.getElementById("img-mandelbrot").src = "static/00.jpg?" + (new Date() - 0);
			// console.log(o.responseText);
			var retJson = eval('new Object(' + o.responseText + ')');
			document.getElementById("textarea-position").innerHTML = retJson.position
		});
}, 100);

function pressMoveButton(id) {
	callApi(
		SERVER_URL + "move",
		{
			"deltaX": id == "button-left" ? "-0.1" : id == "button-right" ? "0.1": "0.0",
			"deltaY": id == "button-down" ? "-0.1" : id == "button-up" ? "0.1": "0.0",
			"zoom": id == "button-zoomin" ? "0.9" : id == "button-zoomout" ? "1.1": "1.0",
		},
		function (o) {
		});
}

function pressResetButton(id) {
	callApi(
		SERVER_URL + "reset",
		{
			"dummy": "dummy",
		},
		function (o) {
		});
}

function pressColorButton(id) {
	callApi(
		SERVER_URL + "color",
		{
			"dummy": "dummy",
		},
		function (o) {
		});
}

function callApi(url, jsonObj, callback) {
	var xhr = new XMLHttpRequest();
	xhr.open('POST', url);
	xhr.setRequestHeader('Content-Type', 'application/json');
	xhr.setRequestHeader('Accept', 'application/json');

	xhr.onreadystatechange = (function(myxhr) {
		return function() {
			if (xhr.readyState == 4 && xhr.status == 200) {
				callback(myxhr);
			}
		}
	})(xhr);

	xhr.send(JSON.stringify(jsonObj));
}
