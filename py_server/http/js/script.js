class Letsbrew {

  constructor() {
    this.device = null;
	this.brewing_rx_buffer = null;
    this.brewing_tx_buffer = null;
    this.onDisconnected = this.onDisconnected.bind(this);
  }
  
  request() {
    let options = {
        acceptAllDevices : true,
        optionalServices : ['d973f2e0-b19e-11e2-9e96-0800200c9a66']
    };

    return navigator.bluetooth.requestDevice(options)
    .then(device => {
      this.device = device;
      this.device.addEventListener('gattserverdisconnected', this.onDisconnected);
    });
  }
  
  connect() {
    if (!this.device) {
      return Promise.reject('Device is not connected.');
    }
    return this.device.gatt.connect();
  }
  
  disconnect() {
    if (!this.device) {
      return Promise.reject('Device is not connected.');
    }
    return this.device.gatt.disconnect();
  }

  send_cmd( command ){
	if (!this.device) {
      return Promise.reject('Device is not connected.');
    }
    return this.brewing_tx_buffer.writeValue((new TextEncoder()).encode(command));
  }

  onDisconnected() {
    console.log('Device is disconnected.');
  }
}

// Global letsbrew handler
var letsbrew;

// Sends the form data and reads the result from the server asynchronously
function send_data_web( form_id ) {
	// Put the form into a nice JSON
	command = stringify_form( form_id );

	$( form_id ).on( 'submit', function( event ){ 
		event.preventDefault();
	});

	var jqxhr = $.post( 
		$(form_id).attr('action'),
		command,
		function( result ){ 
			console.log(result); 
		},
	);

	jqxhr.done( function( data ) {
	});

	return false;
}

// Sends the form data and reads the result from the bt asynchronously
function send_data_bt( form_id ) {
	// Put the form into a nice JSON
	command = stringify_form( form_id );

	$( form_id ).on( 'submit', function( event ){ 
		event.preventDefault();
	});

	letsbrew.send_cmd( command)

	return false;
}

function stringify_form( form_id ){
	var cmd_data = {};
	var fields = $(form_id).serializeArray();
	$.each( fields, function( i, pair ){
		cmd_data[pair.name] = ( isNaN( Number(pair.value) ) ? pair.value : Number(pair.value));
	});

	return JSON.stringify(cmd_data);
}

// Show the divs in the given form with the given id, hide the others
function show_cmd_params( form_id, param_id ){
	var params = $(form_id).find( '[name="cmd_param"]:not(:disabled)');
	$.each( params, ( i, cmd_param ) => {
		$(cmd_param).hide();
		$(cmd_param).find(':input').prop('disabled', true);
	} );

	$(param_id).show();
	$(param_id).find(':input').prop('disabled', false);
}

function init_script(){

	// Setup the listener to the CMD selector select
	$('select[name="CMD"]').on('change', ( event ) => {
		var jq_target =  $(event.target);
		var selected = jq_target.find("option:selected").text();
		var form_id = jq_target.next('div[name="cmd_param"]').prop("id");
		var param_id = jq_target.next('div[name="cmd_param"]').prop("id");
		
		switch( selected ){
			case 'BREW':
				show_cmd_params(form_id, 'BREW_params')
				break;
			case 'STATE':
				show_cmd_params(form_id, null)
				break;
		}
	});


	// Setup the listener for the tab selector
	$('.tab_select_button').on('click', (event) => {
		var show_tab_id = $(event.target).data('tab_select');
		$('#tab_selector').find('.selected_tab_button').toggleClass('selected_tab_button');
		
		$(event.target).toggleClass('selected_tab_button');
		$( '#center_column' ).find('.center_tab:visible').toggle();
		$( '#' + show_tab_id ).toggle();
	} );

	
	letsbrew = new Letsbrew();

	$('#find_brewy').on('click', event => {
	  letsbrew.request()
	  .then( device => letsbrew.connect())
	  .then( server => { return server.getPrimaryService('d973f2e0-b19e-11e2-9e96-0800200c9a66'); } )
	  .then( service => {
	  	// Save the characteristic as write and read buffer
	  	service.getCharacteristic('d973f2e1-b19e-11e2-9e96-0800200c9a66').then( characteristic => {letsbrew.brewing_tx_buffer = characteristic; } ).catch(error => { console.log(error) });
		service.getCharacteristic('d973f2e2-b19e-11e2-9e96-0800200c9a66').then( characteristic => {letsbrew.brewing_rx_buffer = characteristic; } ).catch(error => { console.log(error) });

		console.log(letsbrew);
	  })
	  .catch(error => { console.log(error) });
	});

}