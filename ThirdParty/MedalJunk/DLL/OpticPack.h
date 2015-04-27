#pragma once

#include "OpticException.h"
#include "xzip/XUnzip.h"
#include <string>
#include <memory>
#include <map>
#include <cstdint>
#include <boost/serialization/strong_typedef.hpp>

namespace Optic {

typedef std::uint8_t imageBin;
typedef std::uint8_t audioBin;

typedef std::pair<size_t, std::shared_ptr<const imageBin>> OpticImage;
typedef std::pair<size_t, std::shared_ptr<const audioBin>> OpticAudio;

class OpticPackException : public OpticException {
public:
	OpticPackException() : OpticException("An unknown exception occured!") { }
	OpticPackException(std::string msg) : OpticException(msg) { };
};

struct OpticPackMeta {
	std::string name;
	std::string author;
	std::string apiVersion;
	std::string description;
};

class OpticPack {
	mutable HZIP handle;
	mutable OpticPackMeta metadata;
	mutable std::map<std::string, OpticAudio> audio;
	mutable std::map<std::string, OpticImage> images;

	OpticPack& operator=(const OpticPack&);
	OpticPack(const OpticPack&);
	void loadAssets() const;
	static std::map<std::string, std::string> parseMeta(std::unique_ptr<char[]> buffer);
	
	template<typename T>
	static std::pair<size_t, std::unique_ptr<T[]>> fetchFile(HZIP handle, const std::string& file,
		bool text = false);

	static OpticPackMeta loadMeta(HZIP handle);
	static HZIP open(const std::string& fileName);

public:
    typedef std::map<std::string, OpticAudio>::iterator audio_iterator;
	typedef std::map<std::string, OpticImage>::iterator images_iterator;
	audio_iterator audio_begin() { return audio.begin(); }
	images_iterator images_begin() { return images.begin(); }
	audio_iterator audio_end() { return audio.end(); }
	images_iterator images_end() { return images.end(); }

	~OpticPack();
	std::string getLua() const;
	OpticPackMeta meta() const;
	OpticPack(const std::string& name);
	OpticAudio fetchAudio(const std::string& file) const;
	OpticImage fetchImage(const std::string& file) const;

	static OpticPackMeta meta(const std::string& file);
};

};