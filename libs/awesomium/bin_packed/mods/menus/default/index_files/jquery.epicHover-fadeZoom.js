// UDT rollover-set 1 - version 1.1 - jquery 1.7.1.
// Type: Fade Zoom + Caption
// Copyright - UDTHEMES

(function ($) {
	
	"use strict";

	$.fn.epicHoverFadeZoom = function (options) {
		
		/*rollover defaults
		-----------------------------*/
		var defaults = {			
			overlayColor:'#000',					//Hex: #xxxx
			fontColor:'#fff',						//Hex: #xxxx
			captionDirection:'leftToRight',			//String: caption slide in/out direction
			opacity: 0.8,							//Integer: 0-1
			transitionSpeed:1500,					//Integer: transitions speed, in milliseconds
			easing:'swing',							//String: easing method - see http://jqueryui.com/demos/effect/easing.html
			padding: 3,								//Integer: padding
			mobileActive:false,						//Boolean: whether to activate/deactivate for mobile
			zoomFactor:1.2,
			rolloverLogo: "images/logo.png"
		};		
		//overwrite or merge user eh_options
		var eh_options = $.extend({},defaults, options);

		/*mobile check
		-----------------------------*/
		var isMobile = false;
		if( navigator.userAgent.match(/Android/i) || navigator.userAgent.match(/webOS/i) || navigator.userAgent.match(/iPhone/i) || navigator.userAgent.match(/iPad/i)|| navigator.userAgent.match(/iPod/i) || navigator.userAgent.match(/BlackBerry/i)){
				isMobile = true;
		}
		return this.each(function() {
			var	thumb = $(this),
				thumbW,
				thumbH,
				overlay,
				image,
				imgRatio,
				newW,
				newH,
				marginL,
				marginT,
				captionContainer,
				captionDistance=50;
							
			/*actions
			-----------------------------*/
			//check if device is mobile & if it should be active or not
			if(isMobile === true && eh_options.mobileActive === false){return false;}
			if(eh_options.zoomFactor!==1){
				image = $(this).find('a').children('img').clone();
				$(this).children('a').append(image);
				image.addClass('animationImage');
			}
			/*construct*/
			$(this).children('a').append('<div class="rollover-wrapper"><div class="thumb-rollover"><div class="thumbInfo"></div></div></div>');
			if($(this).children('a').attr('data-caption')){
				$(this).find('.thumbInfo').append('<span><span class="thumb-stripe"></span>'+$(this).children('a').attr('data-caption')+'</span>');
				$('.thumb-stripe').css({backgroundColor:eh_options.fontColor});
			}else{
				$(this).find('.thumbInfo').append('<img class="graphic" src='+eh_options.rolloverLogo+' />');
			}
			
			//overlay color
			var overlay_color_hex=eh_options.overlayColor.replace('#','');
			var r, g, b;
			if(overlay_color_hex.length==3) {
				r = parseInt( overlay_color_hex.substr(0,1) +''+ overlay_color_hex.substr(0,1) ,16);
				g = parseInt( overlay_color_hex.substr(1,1) +''+ overlay_color_hex.substr(1,1) ,16);
				b = parseInt( overlay_color_hex.substr(2,1) +''+ overlay_color_hex.substr(2,1) ,16);
			} else {
				r = parseInt( overlay_color_hex.substr(0,2) ,16);
				g = parseInt( overlay_color_hex.substr(2,2) ,16);
				b = parseInt( overlay_color_hex.substr(4,2) ,16);
			}

			$('.rollover-wrapper').css({ 'background-color' : eh_options.overlayColor });
			$('.rollover-wrapper').css({ 'background-color' : 'rgba( ' + r + ', ' + g + ', ' + b + ', '+ eh_options.opacity +')'});
			
			//overlay color
			$('.thumbInfo').css({color:eh_options.fontColor});
			/*actions*/
			thumb.on({
				mouseenter: function () {
					//size ref & scaling
					if($(this).hasClass('no_overlay')) { return false; }
					thumbW = $(this).width();
					thumbH = $(this).height();
					imgRatio= thumbW / thumbH;
					newH = Math.floor((thumbW / imgRatio)*eh_options.zoomFactor);
					newW = Math.floor(thumbH*imgRatio*eh_options.zoomFactor);
					marginL = (newW-thumbW)/2;
					marginT = (newH-thumbH)/2;
					//set ref.
					overlay = $(this).find('.rollover-wrapper');
					image = $(this).find('.animationImage');
					captionContainer = overlay.find('.thumbInfo').css({paddingLeft:'0',paddingTop:'0',paddingBottom:'0',paddingRight:'0'});
					//size the overlay
					if(eh_options.padding !==0 && jQuery(document).width()>767){
						overlay.css({width:thumbW-(eh_options.padding*2)+'px',height:thumbH-(eh_options.padding*2)+'px',left:eh_options.padding+'px',top: eh_options.padding+'px'});
					}else{
						overlay.css({width:thumbW+1,height:thumbH+1,top:0,left:0});
					}
					//animate
					if(eh_options.captionDirection ==='leftToRight'){
						captionContainer.css({opacity:0,paddingRight:captionDistance+'px'}).stop().animate({opacity:1,paddingRight:'0px'});
					}else if(eh_options.captionDirection ==='rightToLeft'){
						captionContainer.css({opacity:0,paddingLeft:captionDistance+'px'}).stop().animate({opacity:1,paddingLeft:'0px'});
					}else if(eh_options.captionDirection ==='topToBottom'){
						captionContainer.css({opacity:0,paddingBottom:captionDistance+'px'}).stop().animate({opacity:1,paddingBottom:'0px'});
					}else if(eh_options.captionDirection ==='bottomToTop'){
						captionContainer.css({opacity:0,paddingTop:captionDistance+'px'}).stop().animate({opacity:1,paddingTop:'0px'});
					}else{
						captionContainer.css({opacity:0}).stop().animate({opacity:1});
					}
					if(eh_options.opacity!=0){
						overlay.stop().animate({opacity:1},eh_options.transitionSpeed, eh_options.easing);
					}
					if(eh_options.zoomFactor!==1){
						image.css({maxWidth:'none',display:'block'}).stop().animate({width: newW + 'px',height: newH + 'px',marginLeft:-marginL+'px',marginTop:-marginT+'px'}, eh_options.zoomSpeed, eh_options.easing);
					}
				},
				mouseleave: function () {
					//animate 
					if(eh_options.captionDirection ==='leftToRight'){
						captionContainer.stop().animate({opacity:0,paddingLeft:captionDistance+'px'});
					}else if(eh_options.captionDirection ==='rightToLeft'){
						captionContainer.stop().animate({opacity:0,paddingRight:captionDistance+'px'});
					}else if(eh_options.captionDirection ==='topToBottom'){
						captionContainer.stop().animate({opacity:0,paddingTop:captionDistance+'px'});
					}else if(eh_options.captionDirection ==='bottomToTop'){
						captionContainer.stop().animate({opacity:0,paddingBottom:captionDistance+'px'});
					}else{
						captionContainer.stop().animate({opacity:0});
					}
					overlay.stop().animate({opacity:0},eh_options.transitionSpeed,eh_options.easing,function(){
						$(this).css({width:'100%',height:'100%'});
					});
					image.stop().animate({height:thumbH,width:thumbW,marginLeft:0,marginTop:0},eh_options.transitionSpeed/1.5, eh_options.easing,function(){
						$(this).css({display:'none',width:'100%',maxWidth:'100%',height:'auto'});
					});
				}
			});
		});
	};
})(jQuery);