
// Sends the form data and reads the result from the server asynchronously
function send_data( form_id ) {
	// Put the form into a nice JSON
	var cmd_data = {}
	var fields = $(form_id).serializeArray();
	$.each( fields, function( i, pair ){
		cmd_data[pair.name] = ( isNaN( Number(pair.value) ) ? pair.value : Number(pair.value));
	});

	console.log(cmd_data);

	$( form_id ).on( 'submit', function( event ){ 
		event.preventDefault();
	});

	var jqxhr = $.post( 
		$(form_id).attr('action'),
		JSON.stringify(cmd_data),
		function( result ){ 
			console.log(result); 
		},
	);

	jqxhr.done( function( data ) {
	});

	return false;
}

// Show the divs in the given form with the given id, hide the others
function show_cmd_params( form_id, param_id ){
	var params = $(form_id).find( '[name="cmd_param"]:not(:disabled)');
	$.each( params, function( i, cmd_param ){
		$(cmd_param).hide();
		$(cmd_param).find(':input').prop('disabled', true);
	} );

	$(param_id).show();
	$(param_id).find(':input').prop('disabled', false);
}

function init_script(){

	// Setup the listener to the CMD selector select
	$('#cmd_selector').on('change', function( event ){
		var selected = $('#cmd_selector option:selected').text();
		switch( selected ){
			case 'BREW':
				show_cmd_params('#teaform', '#BREW_params')
				break;
			case 'STATE':
				show_cmd_params('#teaform', null)
				break;
		}
	});
}