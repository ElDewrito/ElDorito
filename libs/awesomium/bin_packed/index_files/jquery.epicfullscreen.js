/*------------------------------------------------------
// epicFullscreen - version 2.0 - jquery 1.8.3.
// Design: unlimitDesign
// Author: unlimitDesign
// Copyright - unlimitDesign
------------------------------------------------------*/
;(function ($) {
	
	"use strict";
	
	$.fn.epicFullscreen = function (options) {

		/*fullscreen defaults
		-----------------------------*/
		var defaults = {
			background:'#000',		//Background color
			opacity : 0.4,			//Integer: 0-1
			pattern: true,			//Boolean: use overlay pattern or not
			orientation:'',			//image orientation
			loaded:function(){}		//Callback: when plugin has loaded
		};
		
		//overwrite or merge user options
		var ef_options = $.extend({},defaults, options);

		/*internals
		-----------------------------*/
		var	fullscreen = $(this),
			media,
			winW,
			winH,
			newW,
			newH;

		/*preload
		-----------------------------*/
		if($(this).attr('data-image') && $(this).attr('data-image') !==''){
			media = $(this).attr('data-image');
		}else{
			return false;
		}
		$('<img class="ef-image" />')
			.load(function(){
				fullscreen.append($(this));
				resize();
				$(this).css({opacity:ef_options.opacity});
				fullscreen.animate({opacity:1},1000);
				ef_options.loaded.call(fullscreen);
			})
			.attr('src',media)
			.on('error', function(){
				$('body').append('<div class="ef-error-wrapper"><div class="ef-message">Whopsy, an error occured...</div></div>');
				$('.ef-error-wrapper').fadeIn();
				$(this).hide(); //hide broken img icon in ie
			});

		/*set other options
		-----------------------------*/
		fullscreen.css({backgroundColor:ef_options.background});
		if(ef_options.pattern){
			fullscreen.append('<div class="ef-overlay"></div>');
		}
			
		/*fullscreen/resizing
		-----------------------------*/
		function resize(){
			$('.ef-image').each(function(){
				winW = $(window).width();
				winH = $(window).height();
				var winRatio = winW/winH;
				var mediaH = $(this).height();
				var mediaW = $(this).width();
				var mediaRatio= mediaW / mediaH;
				if(ef_options.orientation !=='portrait'){
					if (winRatio > mediaRatio) {
						newH = Math.round(winW / mediaRatio);
						$(this).css({width: winW + 'px',height: newH + 'px',left:0+'px',top:-(newH-winH)/2 + 'px'});
					} else {
						newW = Math.round(winH*mediaRatio);
						$(this).css({width: newW + 'px',height: winH + 'px',left:-(newW-winW)/2+'px',top:0 + 'px'});
					}
				}else{
					if (winRatio > mediaRatio) {
						newW = Math.round(winH*mediaRatio);
						$(this).css({width: newW+'px',height: winH + 'px',left:'50%',top:0+'px',marginLeft:-(newW/2)+'px',marginTop:0});
					}else{
						newW = Math.round(winW);
						newH = Math.round(winW / mediaRatio);
						$(this).css({width: newW+'px',height: 'auto',left:'50%',top:'50%',marginLeft:-(newW/2)+'px',marginTop:-(newH/2)+'px'});
					}
				}
			});
		}
		$(window).bind('resize',function(){
			resize();
		});
    };
})(jQuery);