var server = 'mydomain.com';
var BOSH_SERVICE = 'http://127.0.0.1:7070/http-bind/';
var ROOM = 'prova@conference.' + server;
var ROOM_SERVICE = 'conference.' + server;
var connection = null;

function log(msg) {
  $('#log').append('<div></div>').append(document.createTextNode(msg));
  console.log(msg);
}

function onConnect(status) {
  if (status == Strophe.Status.CONNECTING) {
    log('Strophe is connecting.');
  } else if (status == Strophe.Status.CONNFAIL) {
    log('Strophe failed to connect.');
    $('#connect').get(0).value = 'connect';
  } else if (status == Strophe.Status.DISCONNECTING) {
    log('Strophe is disconnecting.');
  } else if (status == Strophe.Status.DISCONNECTED) {
    log('Strophe is disconnected.');
    $('#connect').get(0).value = 'connect';
  } else if (status == Strophe.Status.CONNECTED) {
    log('Strophe is connected.');
    $('#to').get(0).value = connection.jid; // full JID
    // set presence
    connection.send($pres());
    // set handlers
    connection.addHandler(onMessage, null, 'message', null, null, null);
    connection.addHandler(onSubscriptionRequest, null, "presence", "subscribe");
    connection.addHandler(onPresence, null, "presence");
    connection.si_filetransfer.addFileHandler(fileHandler);
    connection.ibb.addIBBHandler(ibbHandler);
    
    listRooms();
  }
}

function onMessage(msg) {
  var to = msg.getAttribute('to');
  var from = msg.getAttribute('from');
  var type = msg.getAttribute('type');
  var elems = msg.getElementsByTagName('body');

  if (type == "chat" && elems.length > 0) {
    var body = elems[0];
    log('CHAT: I got a message from ' + from + ': ' + Strophe.getText(body));
  }
  // we must return true to keep the handler alive.  
  // returning false would remove it after it finishes.
  return true;
}

function sendMessage(msg) {
  log('CHAT: Send a message to ' + $('#to').get(0).value + ': ' + msg);

  var m = $msg({
    to: $('#to').get(0).value,
    from: $('#jid').get(0).value,
    type: 'chat'
  }).c("body").t(msg);
  connection.send(m);
}

function setStatus(s) {
  log('setStatus: ' + s);
  var status = $pres().c('show').t(s);
  connection.send(status);
}

function subscribePresence(jid) {
  log('subscribePresence: ' + jid);
  connection.send($pres({
    to: jid,
    type: "subscribe"
  }));
}

function getPresence(jid) {
  log('getPresence: ' + jid);
  var check = $pres({
    type: 'probe',
    to: jid
  });
  connection.send(check);
}

function getRoster() {
  log('getRoster');
  var iq = $iq({
    type: 'get'
  }).c('query', {
    xmlns: 'jabber:iq:roster'
  });
  connection.sendIQ(iq, rosterCallback);
}

function rosterCallback(iq) {
  log('rosterCallback:');
  $(iq).find('item').each(function() {
    var jid = $(this).attr('jid'); // The jabber_id of your contact
    // You can probably put them in a unordered list and and use their jids as ids.
    log('	>' + jid);
  });
}

function onSubscriptionRequest(stanza) {
  if (stanza.getAttribute("type") == "subscribe") {
    var from = $(stanza).attr('from');
    log('onSubscriptionRequest: from=' + from);
    // Send a 'subscribed' notification back to accept the incoming
    // subscription request
    connection.send($pres({
      to: from,
      type: "subscribed"
    }));
  }
  return true;
}

function onPresence(presence) {
  log('onPresence:');
  var presence_type = $(presence).attr('type'); // unavailable, subscribed, etc...
  var from = $(presence).attr('from'); // the jabber_id of the contact
  if (!presence_type) presence_type = "online";
  log('	>' + from + ' --> ' + presence_type);
  if (presence_type != 'error') {
    if (presence_type === 'unavailable') {
      // Mark contact as offline
    } else {
      var show = $(presence).find("show").text(); // this is what gives away, dnd, etc.
      if (show === 'chat' || show === '') {
        // Mark contact as online
      } else {
        // etc...
      }
    }
  }
  return true;
}

function listRooms() {
  connection.muc.listRooms(mydomain, function(msg) {
    log("listRooms - success: ");
    $(msg).find('item').each(function() {
      var jid = $(this).attr('jid'),
        name = $(this).attr('name');
      log('	>room: ' + name + ' (' + jid + ')');
    });
  }, function(err) {
    log("listRooms - error: " + err);
  });
}

function enterRoom(room) {
  log("enterRoom: " + room);
  connection.muc.init(connection);
  connection.muc.join(room, $('#jid').get(0).value, room_msg_handler, room_pres_handler);
  //connection.muc.setStatus(room, $('#jid').get(0).value, 'subscribed', 'chat');
}

function room_msg_handler(a, b, c) {
  log('MUC: room_msg_handler');
  return true;
}

function room_pres_handler(a, b, c) {
  log('MUC: room_pres_handler');
  return true;
}

function exitRoom(room) {
  log("exitRoom: " + room);
  //TBD
}

// file
var sid = null;
var chunksize;
var data;
var file = null;
var aFileParts, mimeFile, fileName;

function handleFileSelect(evt) {
	var files = evt.target.files; // FileList object
    file = files[0];
}

function sendFileClick() {
	sendFile(file);
	
	readAll(file, function(data) {
		log("handleFileSelect:");
		log("	>data="+data);
		log("	>data.len="+data.length);
	});
}

function sendFile(file) {
	var to = $('#to').get(0).value;
	var filename = file.name;
	var filesize = file.size;
	var mime = file.type;
	chunksize = filesize;
	sid = connection._proto.sid;
	log('sendFile: to=' + to);
	// send a stream initiation
	connection.si_filetransfer.send(to, sid, filename, filesize, mime, function(err) {
		fileTransferHandler(file, err);
	});
}

function fileHandler(from, sid, filename, size, mime) {
  // received a stream initiation
  // save to data and be prepared to receive the file.
  log("fileHandler: from=" + from + ", file=" + filename + ", size=" + size + ", mime=" + mime);
  mimeFile = mime;
  fileName = filename;
  return true;
}

function ibbHandler(type, from, sid, data, seq) {
  log("ibbHandler: type=" + type + ", from=" + from);
  switch (type) {
    case "open":
      // new file, only metadata
      aFileParts = [];
      break;
    case "data":
      log("	>seq=" + seq);
      log("	>data=" + data);
      aFileParts.push(data);
      // data
      break;
    case "close":
      // and we're done
      var data = "data:" + mimeFile + ";base64,";
      for (var i = 0; i < aFileParts.length; i++) {
        data += aFileParts[i].split(",")[1];
      }
      log("	>data=" + data);
      log("	>data.len=" + data.length);
      if (mimeFile.indexOf("png") > 0) {
        var span = document.createElement('span');
        span.innerHTML = ['<img class="thumb" src="', data, '" title=""/>'].join('');
      } else {
        var span = document.createElement('span');
        span.innerHTML = ['<a class="link" download="' + fileName + '" href="', data, '" title="">download</a>'].join('');
      }
      document.getElementById('list').insertBefore(span, null);
    default:
      throw new Error("shouldn't be here.")
  }
  return true;
}

function fileTransferHandler(file, err) {
  log("fileTransferHandler: err=" + err);
  if (err) {
    return console.log(err);
  }
  var to = $('#to').get(0).value;
  chunksize = file.size;

  chunksize = 20 * 1024;

  // successfully initiated the transfer, now open the band
  connection.ibb.open(to, sid, chunksize, function(err) {
    log("ibb.open: err=" + err);
    if (err) {
      return console.log(err);
    }


    readChunks(file, function(data, seq) {
      sendData(to, seq, data);
    });
  });
}

function readAll(file, cb) {
    var reader = new FileReader();

    // If we use onloadend, we need to check the readyState.
    reader.onloadend = function(evt) {
      if (evt.target.readyState == FileReader.DONE) { // DONE == 2
        cb(evt.target.result);
      }
    };

    reader.readAsDataURL(file);
}

function readChunks(file, callback) {
  var fileSize = file.size;
  var chunkSize = 20 * 1024; // bytes
  var offset = 0;
  var block = null;
  var seq = 0;

  var foo = function(evt) {
    if (evt.target.error === null) {
      offset += chunkSize; //evt.target.result.length;
      seq++;
      callback(evt.target.result, seq); // callback for handling read chunk
    } else {
      console.log("Read error: " + evt.target.error);
      return;
    }
    if (offset >= fileSize) {
      console.log("Done reading file");
      return;
    }

    block(offset, chunkSize, file);
  }

  block = function(_offset, length, _file) {
    log("_block: length=" + length + ", _offset=" + _offset);
    var r = new FileReader();
    var blob = _file.slice(_offset, length + _offset);
    r.onload = foo;
    r.readAsDataURL(blob);
  }

  block(offset, chunkSize, file);
}

function sendData(to, seq, data) {
  // stream is open, start sending chunks of data
  connection.ibb.data(to, sid, seq, data, function(err) {
    log("ibb.data: err=" + err);
    if (err) {
      return console.log(err);
    }
    // ... repeat calling data
    // keep sending until you're ready you've reached the end of the file
    connection.ibb.close(to, sid, function(err) {
      log("ibb.close: err=" + err);
      if (err) {
        return console.log(err);
      }
      // done
    });
  });
}



function rawInput(data) {
  console.log('RECV: ' + data);
}

function rawOutput(data) {
  console.log('SENT: ' + data);
}

$(document).ready(function() {

  $('#jid').get(0).value = "pippo@"+server;
  $('#pass').get(0).value = "pippo";

  $('#connect').bind('click', function() {
    var url = BOSH_SERVICE;
    connection = new Strophe.Connection(url);
    connection.rawInput = rawInput;
    connection.rawOutput = rawOutput;
    var button = $('#connect').get(0);
    if (button.value == 'connect') {
      button.value = 'disconnect';
      connection.connect($('#jid').get(0).value, $('#pass').get(0).value, onConnect);
    } else {
      button.value = 'connect';
      connection.disconnect();
    }
  });

  $('#send').bind('click', function() {
    var msg = $('#msg').val();
    sendMessage(msg);
  });

  $('#btnGetPres').bind('click', function() {
    var jid = $('#to').val();
    getPresence(jid);
  });

  $('#btnSubPres').bind('click', function() {
    var jid = $('#to').val();
    subscribePresence(jid);
  });

  $('#btnRoster').bind('click', function() {
    getRoster();
  });

  $('#btnAway').bind('click', function() {
    setStatus('away');
  });

  $('#room').val(ROOM);

  $('#btnEnter').bind('click', function() {
    enterRoom($('#room').val());
  });

  $('#btnExit').bind('click', function() {
    exitRoom($('#room').val());
  });
  
  $('#btnSendFile').bind('click', function() {
    console.log('File clicked:');
    sendFileClick();
  });
  
  document.getElementById('file').addEventListener('change', handleFileSelect, false);

});