/* accordions */
jQuery(document).ready(function($) {
	$('.accordion > dd').hide();
	$('.accordion > dt').css({'border-bottom-left-radius':'3px','border-bottom-right-radius':'3px','margin-bottom':'3px'});
	
	$('.accordion > dt > a').click(function() {
		if($(this).parent().hasClass('active')) {
			$(this).parent().parent().children('dd').slideUp();
			$(this).parent().parent().children('dt').removeClass('active');
			$(this).parent().parent().children('dt').css({'border-bottom-left-radius':'3px','border-bottom-right-radius':'3px','margin-bottom':'3px'});
			$(this).parent().parent().children('dt').find('a span').html('+');
		} else {
			$(this).parent().parent().children('dd').slideUp();
			$(this).parent().parent().children('dt').removeClass('active');
			$(this).parent().parent().children('dt').css({'border-bottom-left-radius':'3px','border-bottom-right-radius':'3px','margin-bottom':'3px'});
			$(this).parent().parent().children('dt').find('a span').html('+');
			$(this).parent().animate({'border-bottom-left-radius':'0','border-bottom-right-radius':'0','margin-bottom':'0'});
			$(this).parent().addClass('active');
			$(this).parent().next().slideDown();
			$(this).children('span').html('&ndash;');
		}
		return false;
	});
	
});

/* tabs */
jQuery(document).ready(function($) {
	$('.tabs .tabs_content').hide();
	$('.tabs').each(function() {
		$(this).find('.tabs_content:first').show();
		$(this).find('.tabs_nav li:first').addClass('active');
		$(this).find('.tabs_nav li a').click(function() {
			$(this).parent().parent().children('li').removeClass('active');
			$(this).parent().addClass('active');
			$(this).parent().parent().parent().children('.tabs_content').hide();
			var currentTab = $(this).attr('data-label_ref');
			$(this).parent().parent().parent().children('.tabs_content').each(function() {
				if($(this).data('label_ref')==currentTab) { $(this).show(); }
			});
			return false;
		});
	});
});

/* pricing tables */
jQuery(document).ready(function($) {
	$(".pricing_table .pricing_table_col > ul > li:has(span.price), .pricing_table .pricing_table_col > ul > li:has(a.submit)").addClass("pricing_table_bg");
});