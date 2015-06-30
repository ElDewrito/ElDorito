/*------------------------------------------------------
// epicSlider - version 2.0
// Design: unlimitDesign
// Author: unlimitDesign
// Copyright - unlimitDesign
------------------------------------------------------*/
;(function ($) {

	"use strict";

	var isMobile=false,playerLoading=false,ytPlayer={},ytEvent=[],vEvent=[];
	
	//user agent
	if( navigator.userAgent.match(/Android/i) || navigator.userAgent.match(/webOS/i) ||navigator.userAgent.match(/iPhone/i) || navigator.userAgent.match(/iPad/i)|| navigator.userAgent.match(/iPod/i) || navigator.userAgent.match(/BlackBerry/i)){
		isMobile = true;
	}
	//feature detection
	if(!document.createElement('svg').getAttributeNS){
		$('html').addClass('no-svg');
	}
	var testVideo = document.createElement('video');
	var html5Video = (testVideo.play)? true:false;
	/*youtube*/
	var tag = document.createElement('script');
	tag.src = "https://www.youtube.com/iframe_api";
	var firstScriptTag = document.getElementsByTagName('script')[0];
	firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);
	window.onYouTubeIframeAPIReady = function() {
		$('[data-media="youtube"] iframe').each(function() {
				var ytiframe = $(this).attr('id');
				$(this).parent('.video').addClass('video-inactive');
				ytEvent[ytiframe]='paused';
				ytPlayer[ytiframe] = new YT.Player(ytiframe, {
					events: {
						'onReady': onPlayerReady,
						'onStateChange': onPlayerStateChange
						}
				});
				function onPlayerReady(event) {
				}
				function onPlayerStateChange(event) {
					if (event.data === YT.PlayerState.PLAYING) {
						ytEvent[ytiframe]='playing';
							if(!isMobile){
								if($('#es-direction-nav').length){
									$('#es-direction-nav').removeClass('es-nav-disabled');
								}else{
									$('#es-control-nav').removeClass('es-nav-disabled');
								}
								playerLoading=false;
							}
					}else if (event.data === YT.PlayerState.PAUSED) {
						ytEvent[ytiframe]='paused';
					}
				}
		});
	};
	/*vimeo*/
	jQuery(document).ready(function($){
		$('[data-media="vimeo"] iframe').each(function(){
			var t= $(this).attr('id');
			var vp = $('#'+t)[0],
				vimeoPlayer = $f(vp);
				vEvent[t] = 'paused';
				$(this).parent('.video').addClass('video-inactive');
			vimeoPlayer.addEvent('ready', function() {
				vimeoPlayer.addEvent('play', onPlay);
				vimeoPlayer.addEvent('pause', onPause);
				vimeoPlayer.addEvent('playProgress', onPlayProgress);
			});
			function onPlay(id) {
				vEvent[t] = 'playing';
			}
			function onPause(id) {
				vEvent[t] = 'paused';
			}
			function onPlayProgress(data, id) {	
				if(!isMobile){
					if($('#es-direction-nav').length){
						$('#es-direction-nav').removeClass('es-nav-disabled');
					}else{
						$('#es-control-nav').removeClass('es-nav-disabled');
					}
					playerLoading=false;
				}
			}
		});
	});
	$.fn.epicSlider = function (options) {

		/*slider defaults
		-----------------------------*/
		var defaults = {
			loop:true,									//Boolean: whether slideshow should loop or not	
			slideShow:false,							//Boolean: use slideshow or not
			autoPlay:false,								//Boolean: autoplay uplon load or not
			slideShowInterval:2500,						//Integer: slideshow cycling speed, in milliseconds
			transitionSpeed:750,						//Integer: transitions speed, in milliseconds
			shuffleSlides:false,						//Boolean: add pattern or not
			stack:false,								//Boolean: whether slides should stack
			parallax:false,								//Boolean: whether parallax effect should be used or not
			parallaxFactor:0.3,							//Integer:0-.4
			fullscreenControl:true,						//Boolean: whether fullscreen button should appear or not
			fullscreen:false,							//Boolean: whether slider should take height of window
			imageScaling:true,							//Boolean: whether image should scale
			easing:'easeInOutQuint',					//String: easing method - see http://jqueryui.com/demos/effect/easing.html
			fx:'leftToRight',							//String: none, fade, leftToRight, topToBottom 
			fxmobile:'leftToRight',						//String: mobile effect -  none, fade, leftToRight, topToBottom 
			pattern:true,								//Boolean: use overlay pattern or not
			directionNav:false,							//Boolean: direction nav or control nav
			muteBackgroundVideo:true,					//Boolean: mute background video
			loaded:function(){},						//Callback: when slider has loaded
			before: function(){},						//Callback: before slide animates
			after: function(){},						//Callback: after slide animates
			last: function(){}							//Callback: last slide reached				
		};

		//overwrite or merge user es_options
		var es_options = $.extend({},defaults, options);
		
		/*internals
		-----------------------------*/
		var	slider = $(this),
			slides = slider.find('ul#slides'),
			currentSlide = slides.find('li').eq(0), 
			slideLength = slides.find('li').length,
			slideCount = slides.find('li').length,
			running = false,
			nextSlide,
			prevSlide,
			navNext,
			navPrev,
			navPlay,
			winW,
			winH,
			newW,
			newH,
			timer,
			slideArray=[],
			slideIndex=1,
			navArray=[],
			navIndex=1,
			i=0,
			j=0,
			k=0,
			progress,
			controlNavClicked=false;

		/*preload
		-----------------------------*/
		slides.children('li').each(function(){
			i++;
			var targetSlide = $(this);
				targetSlide.append('<div class="es-background"></div>');
			var background = targetSlide.children('.es-background');
			var imgSrc = $(this).attr('data-image');
			var targetCaption = targetSlide.find('.es-caption');
			var bkgVideo = false;
			var slideLink='';
			//check video tag
			if(targetSlide.find('video').length && html5Video){
				bkgVideo = true;
				if(es_options.muteBackgroundVideo){
					$('video').prop('muted', true); //mute video
				}
			}
			//set slide ids
			$(this).attr('id','slide-'+i);
			slideArray.push($(this).attr('id'));
			//set bg color
			if($(this).attr('data-slide-bgcolor')){
				background.css({backgroundColor:$(this).attr('data-slide-bgcolor')});
			}
			//set slide link
			if($(this).attr('data-link')){
				slideLink = $(this).attr('data-link');
			}
			//set image
			if($(this).attr('data-image')){
				//load imgs
				$('<img class="es-image" />')
					.load(function(){
						slideCount--;
							if(!slideCount){
								init();	
							}
							if(!bkgVideo || isMobile){
								if(slideLink){
									background.append('<a href="'+slideLink+'"></a>');
									background.find('a').append($(this));
								}else{
									background.append($(this));
								}
							}else{
								targetSlide.find('.es-video-background').append($(this));
								targetSlide.find(background).remove();
							}
							resize();
					})
					.attr('src',imgSrc)
					.on('error', function(){
						slider.css({background:'none'}).append('<div class="es-error-wrapper"><div class="es-message">Whopsy, an error occured...</div></div>');
						$('.es-error-wrapper').fadeIn();
						$(this).hide(); //hide broken img icon in ie
						if(es_options.directionNav){
							$('#es-direction-nav').hide();
						}else{
							$('#es-control-nav').hide();
						}
					});
			}else{
				slideCount--;
				if(!slideCount){
					init();	
				}
			}
			//set bg pattern
			if($(this).attr('data-bg-pattern')){
				background.css({backgroundImage:'url('+$(this).attr('data-bg-pattern')+')'});
			}
			//set caption width
			if(targetCaption.attr('data-caption-width')){
				targetCaption.css({width:targetCaption.attr('data-caption-width')});
			}
			//set caption color preset
			if(targetCaption.attr('data-caption-color')){
				targetCaption.addClass(targetCaption.attr('data-caption-color'));
			}
			//set caption position
			if(targetCaption.attr('data-caption-position')){
				targetCaption.addClass(targetCaption.attr('data-caption-position'));
			}
			//set caption type
			if(targetCaption.attr('data-caption')){
				targetCaption.addClass(targetCaption.attr('data-caption'));
				if(targetCaption.attr('data-caption-position')==='center' || targetCaption.attr('data-caption-position')==='center-left' || targetCaption.attr('data-caption-position')==='center-right' || targetCaption.attr('data-caption-position')==='top-center' || targetCaption.attr('data-caption-position')==='bottom-center'){
					var captionContent = targetCaption.clone();
					targetCaption.remove();
					$(this).append('<div class="es-table"><div class="es-table-cell"></div></div>');
					captionContent.appendTo($(this).find('.es-table-cell'));
					if(targetCaption.attr('data-caption-position')==='top-center'){
						$(this).find('.es-table-cell').addClass('top-align');
					}else if(targetCaption.attr('data-caption-position')==='bottom-center'){
						$(this).find('.es-table-cell').addClass('bottom-align');
					}
				}
			}else{
				targetSlide.children('p').wrapAll('<div class="es-caption default bottom-right"></div');
			}
			//set bg video
			if($(this).find('video').length && html5Video){
				targetSlide.find('.es-video-background').attr({'poster':$(this).attr('data-image'),'loop':true,'autobuffer':'autobuffer','preload':'preload','muted':'muted'});
				if($(this).is(':first-child')){
					targetSlide.find('.es-video-background')[0].play();
					targetSlide.find('.es-video-background').css({position:'absolute'});
				}
				if(isMobile){
					targetSlide.find('.es-video-background').remove();
				}
			}
			//set overlay pattern
			if(es_options.pattern) {
				if(!bkgVideo || !html5Video){
					background.append('<div class="es-overlay"></div>');
				}else{
					//targetSlide.append('<div class="es-overlay"></div>');
				}
			}
		});	
		/*initiate slider
		-----------------------------*/	
		function init(){
			slider.css({background:'none'});
			if(es_options.fullscreen && $(window).width() > 768){
				slider.css({height:$(window).height()+'px'});
				resize();
			}
			deviceMobile();					
			es_options.loaded.call(slider);
			//shuffle
			if(es_options.shuffleSlides){
				var targetSlide = slides.children('li');
				targetSlide.sort(function() { 
					return (Math.round(Math.random())-0.5); 
				}); 
				slides.children().remove();
				for(var i=0; i < slideLength; i++){
					slides.append(targetSlide[i]);
				}
				targetSlide.each(function(){
					k++;
					$(this).attr('id','slide-'+k);
				});
				currentSlide = slides.children('li').eq(0);
			}
			//animate first slide
			currentSlide.addClass('es-slide-active').css({visibility:'visible'}).animate({opacity:1});
			//construct nav
			if(es_options.directionNav){
				if(slideLength > 1){
					if(!es_options.slideShow && !es_options.fullscreenControl) {
						//prev & next
						slider.append('<div id="es-direction-nav"><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button nav-last"><div id="es-next"></div></div></div>');
					}else if(!es_options.slideShow && es_options.fullscreenControl){
						//prev,next,fullscreen
						slider.append('<div id="es-direction-nav" class="three-controls"><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button"><div id="es-next"></div></div><div class="es-nav-button nav-last"><div class="es-fullscreen"></div></div></div>');
					}else if(es_options.slideShow && !es_options.autoPlay && !es_options.fullscreenControl){
						//play,prev,next
						slider.append('<div id="es-direction-nav" class="three-controls"><div class="es-nav-button"><div id="es-progress-button"></div><div id="es-play"></div></div><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button nav-last"><div id="es-next"></div></div></div>');
					}else if(es_options.slideShow && es_options.autoPlay && !es_options.fullscreenControl){
						//play.active,prev,next
						slider.append('<div id="es-direction-nav" class="three-controls"><div class="es-nav-button"><div id="es-progress-button"></div><div id="es-play" class="active"></div></div><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button nav-last"><div id="es-next"></div></div></div>');
					}else if(es_options.slideShow && !es_options.autoPlay && es_options.fullscreenControl){
						//play,prev,next,fullscreen
						slider.append('<div id="es-direction-nav" class="four-controls"><div class="es-nav-button"><div id="es-progress-button"></div><div id="es-play"></div></div><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button"><div id="es-next"></div></div><div class="es-nav-button nav-last"><div class="es-fullscreen"></div></div></div>');
					}else{
						//play.active,prev,next,fullscreen
						slider.append('<div id="es-direction-nav" class="four-controls"><div class="es-nav-button"><div id="es-progress-button"></div><div id="es-play" class="active"></div></div><div class="es-nav-button"><div id="es-prev"></div></div><div class="es-nav-button"><div id="es-next"></div></div><div class="es-nav-button nav-last"><div class="es-fullscreen"></div></div></div>');
					}
				}else{
					if(es_options.fullscreenControl) {
						//fullscreen
						slider.append('<div id="es-direction-nav" class="one-controls"><div class="es-nav-button nav-last"><div class="es-fullscreen"></div></div></div>');
					}
				}
				//set arrow direction & state
				if(es_options.fx !== 'topToBottom'){
					$('#es-next').addClass('right');
					$('#es-prev').addClass('left');
				}else{
					$('#es-next').addClass('up');
					$('#es-prev').addClass('down');
				}
				//cache the navs and progress
				navNext = $('#es-next');	
				navPrev = $('#es-prev');	
				navPlay = $('#es-play');
				progress = $('#es-progress-button');
				//disable prev if loop is false
				if(!es_options.loop){
					navPrev.parent().addClass('es-button-disabled');
				}
				//nav but evt
				navNext.each(function(){
					$(this).on('click', function() {
						epicMedia('pause');
						if(running === true){slider.killTimer();}
						currentSlide = slides.children('li.es-slide-active');
						nextSlide = currentSlide.next('li');
						slide('next');
					});
				})
				navPrev.each(function(){
					$(this).on('click', function () {
						epicMedia('pause');
						if(running === true){slider.killTimer();}
						currentSlide = slides.children('li.es-slide-active');
						prevSlide = currentSlide.prev('li');
						slide('prev');
					});
				});
				navPlay.each(function(){
					navPlay.on('click',function () {
						if($(this).parent().hasClass('es-button-disabled') || playerLoading===true){return false;}
						progress.css({width:'0px'});
						if (running === false) {
							setTimer();
							$(this).addClass('active');
							running = true;
						}else {
							slider.killTimer();
							$(this).removeClass('active');
							running = false;
						}
					});
				});
			}else{
				if(slideLength > 1){
					slider.append('<div id="es-control-nav"><ul></ul></div><div id="es-progress-bar"></div>');
					if(es_options.fullscreenControl){
						$('#es-control-nav').append('<div class="es-fullscreen"></div>');
					}
					progress = $('#es-progress-bar');
					for(var l=1; l < slideLength+1; l++){
						$('#es-control-nav ul').append('<li><a class="es-cn" id="es-cn-'+l+'" href="#">'+l+'</a></li>');
						navArray.push('es-cn-'+l);
						$('#es-control-nav ul li:first-child a').addClass('es-cn-active');
					}
					$('.es-cn').each(function(){
						$(this).on('click', function() {
							if(currentSlide.is(':animated') || playerLoading===true){return false;}
							if($(this).hasClass('es-cn-active')){
								return false;
							}else{
								controlNavClicked=true;
								$('#es-control-nav').find('a').removeClass('es-cn-active');
								$(this).addClass('es-cn-active');
							}
							/*kill slideshow*/
							if(running === true){slider.killTimer();}
							es_options.slideShow = false;
							/*get and compare array positions*/
							currentSlide = slides.children('li.es-slide-active');
							navIndex = $.inArray($(this).attr('id'), navArray)+1;
							slideIndex = $.inArray(currentSlide.attr('id'), slideArray)+1;
							if(navIndex === slideIndex){
								return false; 
							}else if(navIndex >= slideIndex){
								nextSlide = slides.find('#slide-'+navIndex);
								slide('next');
							}else if(navIndex <= slideIndex){
								prevSlide = slides.find('#slide-'+navIndex);
								slide('prev');
							}	
							return false;
						});
					});
				}else{
					slider.append('<div id="es-control-nav"></div>');
					if(es_options.fullscreenControl){
						$('#es-control-nav').append('<div class="es-fullscreen"></div>');
					}
				}
			}
			//fullscreen button
			$('.es-fullscreen').on('click',function(){
				slider.toggleClass('fullscreen');
				$('body').toggleClass('fullscreen');
				$('#content-wrapper').toggleClass('fullscreen-content');
				if(isMobile){
					$('#content-wrapper').toggleClass('transform');
				}
				$(this).toggleClass('active');
				resize();
				return false;
			});
			//key events 
			$(document).keydown(function(e){
				controlNavClicked=false;
				currentSlide = slides.children('li.es-slide-active');
				if(es_options.fx !== 'topToBottom'){
					switch(e.which){
						case 37:
							prevSlide = currentSlide.prev('li');
							slide('prev');
							slider.killTimer();
							break;	
						case 39:
							nextSlide = currentSlide.next('li');
							slide('next');
							slider.killTimer();
							break;
					}
				}else{
					switch(e.which){
						case 38:
							nextSlide = currentSlide.next('li');
							slide('next');
							slider.killTimer();
							break;
						case 40:
							prevSlide = currentSlide.prev('li');
							slide('prev');
							slider.killTimer();
							break;
					}
				}
			});
			//media handling
			$('[data-media=vimeo],[data-media=youtube]').each(function(){
				j++;
				$(this).append('<a href="" id="es-video-close-'+j+'" class="es-video-close"></a>');
				$(this).find('a.es-video-play').attr('id','es-video-play-'+j);
				$('#es-video-play-'+j).on('click', function() {
					$(this).parents('li').eq(0).find('.video').css({zIndex:4}).removeClass('video-inactive');
					$(this).parents('li').eq(0).find('.es-caption').fadeOut();
					$(this).parents('li').eq(0).find('.es-video-close').delay(500).fadeIn();
					$(this).parents('li').eq(0).find('.es-background').fadeOut(400);
					if(running === true){slider.killTimer();}
					if(es_options.directionNav === true && !isMobile){
						$('#es-direction-nav').addClass('es-nav-disabled');
					}else{
						$('#es-control-nav').addClass('es-nav-disabled');
					}
					if(!isMobile){
						playerLoading=true;
						epicMedia('play');
					}
					return false;
				});
				$('#es-video-close-'+j).on('click', function() {
					$(this).parents('li').eq(0).find('.es-caption').fadeIn();
					$(this).parents('li').eq(0).find('.es-background').fadeIn(400);
					$(this).fadeOut('fast');
					epicMedia('pause');
					$(this).parents('li').eq(0).find('.video').css({zIndex:0}).addClass('video-inactive');
					return false;
				});
			});
			//check if slidehow is active & if it should autoplay
			if(es_options.autoPlay){
				setTimer();
				running = true;
			}
			$('.es-next-slide').on('click',function(){
				if(running === true){slider.killTimer();}
				currentSlide = slides.children('li.es-slide-active');
				nextSlide = currentSlide.next('li');
				controlNavClicked=false;
				slide('next');
				return false;
			});
			$('.es-prev-slide').on('click', function () {
				if(running === true){slider.killTimer();}
				currentSlide = slides.children('li.es-slide-active');
				prevSlide = currentSlide.prev('li');
				controlNavClicked=false;
				slide('prev');
				return false;
			});
		}
		/*mobile check
		-----------------------------*/
		function deviceMobile() {
			//check for device	
			if(isMobile){
				//swap effect
				es_options.fx = es_options.fxmobile;
				//mobile swipe actions from http://www.codingjack.com/playground/swipe/ - MIT/GPLv.2
				$(slider).touchSwipe(swipe);
			}
		}
		function swipe(direction) {
			slider.killTimer();
			currentSlide = slides.children('li.es-slide-active');
			controlNavClicked=false;
			if(direction === 'left'){
				nextSlide = currentSlide.next('li');
				slide('next');
			}else{
				prevSlide = currentSlide.prev('li');
				slide('prev');
			}
		} 
		/*image & media scaling
		-----------------------------*/
		function resize(){
			slider.find('.es-image,.es-video-background').each(function(){
				winW = slider.width();
				winH = slider.height();
				var winRatio = winW/winH;
				var mediaH = $(this).height();
				var mediaW = $(this).width();
				var mediaRatio= mediaW / mediaH;
				if(es_options.imageScaling && !$(this).parents('li').eq(0).is('[data-no-cropping]')){
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
			if(es_options.fullscreen && $(window).width() > 768){
				slider.css({height:$(window).height()+'px'});
			}
			resize();
		});
		/*slideshow timer & progress
		-----------------------------*/
		var setTimer = function(){
			if(slideLength > 1){
				startProgress();
				timer  =  setInterval(function(){ 
					startProgress();
					currentSlide = slides.children('li.es-slide-active');
					nextSlide = currentSlide.next('li');
					controlNavClicked=false;
					slide('next');
				}, es_options.slideShowInterval);
			}
		}		
		var startProgress = function(){
			if(es_options.directionNav){
				progress		
						.show()
						.stop(true,true)
						.animate({width:'40px'},es_options.slideShowInterval , function(){
						$(this).css({width:'0px'});
				});
			}else{
				progress	
						.show()
						.stop(true,true)
						.animate({width:'100%'},es_options.slideShowInterval, function(){
						$(this).css({width:'0px'});
				});
			}
		}		
		slider.killTimer = function (){
			clearInterval(timer);
			if(es_options.directionNav){
				slider.find('#es-progress-button').stop().fadeOut('fast');
				$('#es-play').removeClass('active');
			}else{
				slider.find('#es-progress-bar').stop().fadeOut('fast');
			}
			
			running=false;
		};
		/*media handling
		-----------------------------*/		
		function epicMedia(mediaStatus){
			currentSlide = slides.children('li.es-slide-active');
			var mediaType = currentSlide.attr('data-media');
			if(currentSlide.find('.video').length && currentSlide.is(':visible')){
				var mediaPlayer = currentSlide.find('iframe').attr('id');
				//switch
				if(mediaType ==='vimeo'){
					if(mediaStatus ==='play'){
						$f($('#'+ mediaPlayer)[0]).api('play');
					}else{
						if(vEvent[mediaPlayer]==='playing'){
							$f($('#'+ mediaPlayer)[0]).api('pause');
							vEvent[mediaPlayer]='paused';
						}
					}
				}else if(mediaType ==='youtube'){
					if(mediaStatus ==='play'){
						ytPlayer[mediaPlayer].playVideo();
					}else{
						if(ytEvent[mediaPlayer]==='playing'){
							ytPlayer[mediaPlayer].pauseVideo();
							ytEvent[mediaPlayer]='paused';
						}
					}
				}	
			}else if(currentSlide.find('.es-video-background').length && currentSlide.is(':visible')){
				var fsmediaPlayer = currentSlide.find('.es-video-background').attr('id');
				if(html5Video){
					if(mediaStatus ==='play'){
						$('#'+fsmediaPlayer)[0].play();
						$('#'+fsmediaPlayer).css({position:'absolute'});
					}else{
						$('#'+fsmediaPlayer)[0].pause();
					}
				}
			}
		}
		/*slide handling
		-----------------------------*/
		function slide(dir){
			//check number of slides
			if(slideLength <= 1){
				return false;
			}
			es_options.before.call(slider);
			if(currentSlide.is(':animated') || playerLoading===true){return false;}
			if(currentSlide.find('.es-background').is(':visible')){
				currentSlide.find('.video').addClass('video-inactive');
			}
			if(dir === 'next'){
				epicMedia('pause');
				//if @ second to last image & looping is off, disable play/next button
				if(slideLength-1 === $.inArray(currentSlide.attr('id'), slideArray)+1 && !es_options.loop){
					es_options.last.call(slider);
					progress.stop().fadeOut('fast');						//kill progress animation
					if(running === true){slider.killTimer();}		//kill timer
					if(es_options.directionNav){
						navNext.parent().addClass('es-button-disabled');		//disable next 
						navPlay.parent().addClass('es-button-disabled');		//disable play
					}
				}
				//check nav type
				if(es_options.directionNav){
					if(navPrev.parent().hasClass('es-button-disabled')){navPrev.parent().removeClass('es-button-disabled');}
				}else if(controlNavClicked===false){
					var nextCn = $('#es-control-nav').find('a.es-cn-active').parent('li').next('li');
					if(nextSlide.length !== 0) {$('#es-control-nav').find('a.es-cn-active').removeClass('es-cn-active');}
					nextCn.find('a').addClass('es-cn-active');
				}
				//reached limit
				if(nextSlide.length === 0) { 
					es_options.last.call(slider);
					if(!es_options.loop){
						return false;										//prevent clicking & sliding
					}else{							
						nextSlide = slides.children('li:first-child');		//move to first slide
						$('#es-control-nav').find('a.es-cn-active').removeClass('es-cn-active');
						$('#es-control-nav').find('li:first-child').children('a').addClass('es-cn-active');
					}
				}
				//none
				if(es_options.fx ==='none'){
					currentSlide
								.removeClass('es-slide-active')
								.css({opacity:0,visibility:'hidden'});
					nextSlide
								.addClass('es-slide-active')
								.css({opacity:1,visibility:'visible'});
								es_options.after.call(slider);
								if(nextSlide.find('.es-video-background').length){epicMedia('play');}
				//fade
				}else if(es_options.fx ==='fade'){
					currentSlide
								.removeClass('es-slide-active')
								.css({zIndex:0})
								.animate({opacity:1},es_options.transitionSpeed,es_options.easing,function(){
									$(this).css({opacity:0,visibility:'hidden'});
								});
					nextSlide
								.addClass('es-slide-active')
								.css({opacity:0,zIndex:1,visibility:'visible'})
								.show()
								.animate({opacity:1},es_options.transitionSpeed,es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});		
				//slide right to left
				}else if(es_options.fx === 'leftToRight'){
					if(es_options.stack){
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({left:0},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else if(es_options.parallax){
						currentSlide
									.css({zIndex:1})
									.removeClass('es-slide-active')
									.animate({left:-(es_options.parallaxFactor*1000),width:0},es_options.transitionSpeed, es_options.easing, function(){
										$(this).css({opacity:0,visibility:'hidden'});
									});
					}else{
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({left:-winW},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}
					if(es_options.parallax){
						nextSlide
								.addClass('es-slide-active')
								.css({width:'100%',left:es_options.parallaxFactor*1000,zIndex:0,opacity:1,visibility:'visible'})
								.animate({width:'100%',left:0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}else{
						nextSlide
								.addClass('es-slide-active')
								.css({left:winW, zIndex:1,opacity:1,visibility:'visible'})
								.animate({left:0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}
					//slide top/bottom
				}else if(es_options.fx ==='topToBottom'){
					if(es_options.stack){
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({top:0},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else if(es_options.parallax){
						currentSlide
									.css({zIndex:1})
									.removeClass('es-slide-active')
									.animate({top:-(es_options.parallaxFactor*1000),height:es_options.parallaxFactor*1000},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else{
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({top:winH},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});	
					}
					if(es_options.parallax){
						nextSlide
								.addClass('es-slide-active')
								.css({height:'100%',top:es_options.parallaxFactor*1000, zIndex:0,opacity:1,visibility:'visible'})
								.animate({height:'100%',top:0},es_options.transitionSpeed,es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}else{	
						nextSlide
								.addClass('es-slide-active')
								.css({top:-winH, zIndex:1,opacity:1,visibility:'visible'})
								.animate({top:0},es_options.transitionSpeed,es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}
				}
			}else{
				epicMedia('pause');
				//check nav type
				if(es_options.directionNav){
					if(navNext.parent().hasClass('es-button-disabled')){ navNext.parent().removeClass('es-button-disabled');}
					if(navPlay.parent().hasClass('es-button-disabled')){navPlay.parent().removeClass('es-button-disabled');}
					if($.inArray(currentSlide.attr('id'), slideArray)===1 && es_options.loop === false){
						navPrev.parent().addClass('es-button-disabled');
					}
				}else if(controlNavClicked===false){
					var prevCn = $('#es-control-nav').find('a.es-cn-active').parent('li').prev('li');
					if (prevSlide.length !== 0) {$('#es-control-nav').find('a.es-cn-active').removeClass('es-cn-active');}
					prevCn.find('a').addClass('es-cn-active');
				}
				//reached limit	
				if (prevSlide.length === 0) { 
					if(!es_options.loop){
						if(es_options.directionNav){navPrev.parent().addClass('es-button-disabled');}	//disable prev upon start
							return false;																		//prevent clicking & sliding
						}else{
							prevSlide = slides.children('li:last-child');										//move to last slide
							$('#es-control-nav').find('a.es-cn-active').removeClass('es-cn-active');
							$('#es-control-nav').find('li:last-child').children('a').addClass('es-cn-active');
						}
					}	
				//none
				if(es_options.fx ==='none'){
					currentSlide
								.removeClass('es-slide-active')
								.css({opacity:0,visibility:'hidden'});
					prevSlide
								.addClass('es-slide-active')
								.css({opacity:1,visibility:'visible'});
								es_options.after.call(slider);
								if(prevSlide.find('.es-video-background').length){epicMedia('play');}
				//fade
				}else if(es_options.fx ==='fade'){
					currentSlide
								.removeClass('es-slide-active')
								.css({zIndex:0})
								.animate({opacity:1},es_options.transitionSpeed, es_options.easing,function(){
									$(this).css({opacity:0,visibility:'hidden'});
								});
					prevSlide
							.addClass('es-slide-active')
							.css({opacity:0,zIndex:1,visibility:'visible'})
							.animate({opacity:1},es_options.transitionSpeed, es_options.easing, function(){
								es_options.after.call(slider);
								if($(this).find('.es-video-background').length){epicMedia('play');}
							});   
				//slide left to right
				}else if(es_options.fx === 'leftToRight'){
					if(es_options.stack){
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({left:0},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else if(es_options.parallax){
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({width:'100%',left:es_options.parallaxFactor*1000},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else{
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({left:winW},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}
					if(es_options.parallax){
						prevSlide
								.addClass('es-slide-active')
								.css({width:0,left:-(es_options.parallaxFactor*1000),zIndex:1,opacity:1,visibility:'visible'})
								.animate({width:'100%',left:0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}else{
						prevSlide
								.addClass('es-slide-active')
								.css({left:-winW, zIndex:1,opacity:1,visibility:'visible'})
								.animate({left:0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}
				//slide top/bottom		
				}else if(es_options.fx ==='topToBottom'){
					if(es_options.stack){
						currentSlide	
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({top:0},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else if(es_options.parallax){
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({height:'100%',top:es_options.parallaxFactor*1000},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}else{
						currentSlide
									.css({zIndex:0})
									.removeClass('es-slide-active')
									.animate({top:-winH},es_options.transitionSpeed, es_options.easing, function(){$(this).css({opacity:0,visibility:'hidden'});});
					}
					if(es_options.parallax){
						prevSlide
								.addClass('es-slide-active')
								.css({height:es_options.parallaxFactor*1000,top:-(es_options.parallaxFactor*1000), zIndex:1,opacity:1,visibility:'visible'})
								.show()
								.animate({height:'100%',top:0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}else{	
						prevSlide
								.addClass('es-slide-active')
								.css({top:winH, zIndex:1,opacity:1,visibility:'visible'})
								.show()
								.animate({top: 0},es_options.transitionSpeed, es_options.easing, function(){
									es_options.after.call(slider);
									if($(this).find('.es-video-background').length){epicMedia('play');}
								});
					}
				}
			}
		}
	};
})(jQuery);