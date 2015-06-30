/*
 * Contact Form Plugin
 *
 * Author: Farid Hadi
 * Copyright Farid Hadi
 *
*/

jQuery(document).ready(function($) {
	"use strict";
	
	var contactFormDefaults=new Array();
	contactFormDefaults['msg']=$('.contactForm span#msg').html();
	$('.contactForm input, .contactForm textarea').each(function() {
		if($(this).attr('placeholder')) {
			$(this).val($(this).attr('placeholder'));
		}
	});
	
	$('.contactForm input[type="text"], .contactForm input[type="email"], .contactForm textarea').focus(function() {
		$(this).addClass('inputHighlight').removeClass('errorOutline');
		if($(this).attr('required')) {
			$('.contactForm span#msg').html(udt_global_vars.contact_form_required_fields_label_ajax).removeClass('errorMsg successMsg');
		} else {
			$('.contactForm span#msg').html(contactFormDefaults['msg']).removeClass('errorMsg successMsg');
		}
		if($(this).val()==$(this).attr('placeholder')) {
			$(this).val('');
		}
	});
	$('.contactForm input[type="text"], .contactForm input[type="email"], .contactForm textarea').blur(function() {
		$(this).removeClass('inputHighlight');
		$('.contactForm span#msg').html(contactFormDefaults['msg']).removeClass('errorMsg successMsg');
		if($(this).val()=='') {
			$(this).val($(this).attr('placeholder'));
		}
	});
	
	$('.contactForm input[type="text"], .contactForm input[type="email"], .contactForm textarea').hover(function() {
			$(this).addClass('inputHighlight');
		},
		function() {	
			$(this).not(':focus').removeClass('inputHighlight');
		}
	);
	
	$('.contactForm').submit(function() {
		$('.contactForm .submit').attr("disabled", "disabled");
		$('#msg').html('<span class="loading-animation"></span>').removeClass('errorMsg successMsg');
		var isError=false;
		$('.contactForm input, .contactForm textarea').each(function() {
			if($(this).attr('required') && ($.trim($(this).val())==$(this).attr('placeholder') || $.trim($(this).val())=='')) {
				$(this).addClass('errorOutline');
				$('#msg').html(udt_global_vars.contact_form_warning).addClass('errorMsg');
				isError=true;
			}
			if($(this).attr('type')=='email') {
				var reg = /^([A-Za-z0-9_\-\.])+\@([A-Za-z0-9_\-\.])+\.([A-Za-z]{2,4})$/;
				if(reg.test($(this).val())==false) {
					$(this).addClass('errorOutline');
					if(!isError) {
						$('#msg').html(udt_global_vars.contact_form_email_warning).addClass('errorMsg');
					}
					isError=true;
				}
			}
		});
		if(isError) {
			$('.contactForm .submit').removeAttr("disabled");
			return false;
		} else {
			var name = $('#contactName').val(), email = $('#contactEmail').val(), subject = $('#contactSubject').val(), message = $('#contactMessage').val();
			$.ajaxSetup ({
				cache: false
			});
			var dataString = 'contactName='+ name + '&contactEmail=' + email + '&contactSubject=' + subject + '&contactMessage=' + message + '&submitContact=true&ajax=1';
			$.ajax({
				type: "POST",
				url: $('#contactForm').attr('action'),
				data: dataString,
				success: function(msg) {
					// Check to see if the mail was successfully sent
					if(msg=='Mail sent') {
						// Update the progress bar
						$('#msg').html(udt_global_vars.contact_form_success_message).addClass('successMsg');
						// Reset the subject field and message textbox
						if($('#contactSubject').attr('placeholder')) {
							$('#contactSubject').val($('#subject').attr('placeholder'));
						} else {
							$('#contactSubject').val('');
						}
						if($('#contactMessage').attr('placeholder')) {
							$('#contactMessage').val($('#message').attr('placeholder'));
						} else {
							$('#contactMessage').val('');
						}
					} else {
						$('#msg').html(udt_global_vars.contact_form_error).addClass('errorMsg');
						$('.contactForm .submit').attr("disabled", "");
					}
					// Activate the submit button
					$('.contactForm .submit').removeAttr("disabled");
				},
				error: function(ob,errStr) {
					$('#msg').html(udt_global_vars.contact_form_error).addClass('errorMsg');
					//Activate the submit button
					$('.contactForm .submit').removeAttr("disabled");
				}
			});
			return false;
		}
	});
});