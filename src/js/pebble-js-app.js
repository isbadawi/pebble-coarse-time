Pebble.addEventListener("ready", function(e) {
});

Pebble.addEventListener('showConfiguration', function(e) {
  console.log('About to open configuration!')
  Pebble.openURL('http://badawi.io/pebble/coarse_configuration.html')
});

Pebble.addEventListener('webviewclosed', function(e) {
  var config = JSON.parse(decodeURIComponent(e.response));
  console.log('Config returned: ' + JSON.stringify(config));
  console.log('About to send config to Pebble!');

  Pebble.sendAppMessage(config,
    function(e) {
      console.log('Message delivered! Transaction ID: ' + e.data.transactionId);
    },
    function(e) {
      console.log('Delivery failed! Transaction ID: ' + e.data.transactionId);
      console.log('Error is: ' + e.error.message);
    }
  );
});
